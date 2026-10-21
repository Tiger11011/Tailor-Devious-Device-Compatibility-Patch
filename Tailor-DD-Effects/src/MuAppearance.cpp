#include "PCH.h"
#include "MuAppearance.h"
#include "AppearanceTrace.h"
#include "MuPresetPath.h"
#include "MuBodyTint.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
namespace MuAppearance {namespace {
// This optional adapter reads the current selection. If a known-good selected
// preset's native NPC state regresses after load, the guarded recovery asks Mu
// to reapply that same object; it never chooses a new preset or reloads settings.
struct PresetPrefix {std::string name,path,tint,extra,body;std::byte bodyData[104];std::string npcName,npcShortName;RE::FormID race;};
static_assert(offsetof(PresetPrefix,path)==32 && offsetof(PresetPrefix,race)==328 && sizeof(std::shared_ptr<void>)==16);
using CurrentFn=void*(*)(void*,std::shared_ptr<void>*,RE::TESNPC*);
using AppliedFn=bool(*)(void*,RE::Actor*);
CurrentFn current=nullptr;AppliedFn applied=nullptr;void* manager=nullptr;
// Exact Mu 2.1.4 PDB/decompile signature: bool ApplyPreset(Actor*,shared_ptr<Preset>).
using ApplySelectedFn=bool(*)(void*,RE::Actor*,std::shared_ptr<void>);
ApplySelectedFn applySelected=nullptr;
struct Face {
    std::uintptr_t node=0;std::string name,tint;std::uint32_t feature=0;
    std::array<float,3> color{};
    bool operator==(const Face&)const=default;
};
// Geometry identities, not texture/material values: Mu/MDNM update those values
// in response to our event. Tracking their output would feed back into itself.
std::vector<std::uintptr_t> Geometry(RE::Actor* actor){
    std::vector<std::uintptr_t> result;
    if(auto* root=actor?actor->Get3D():nullptr)RE::BSVisit::TraverseScenegraphGeometries(root,[&](RE::BSGeometry* g){
        result.push_back(reinterpret_cast<std::uintptr_t>(g));return RE::BSVisit::BSVisitControl::kContinue;
    });
    std::sort(result.begin(),result.end());return result;
}
struct State {Face face;bool inFlight=false;std::vector<std::uintptr_t> notifiedGeometry;std::uint64_t checks=0;std::string restoringStem;RE::NiPointer<RE::NiSourceTexture> selectedTint;std::uint64_t restoreSerial=0;bool restoreNotified=false;};
std::unordered_map<RE::FormID,State> states;std::recursive_mutex mutex;std::uint64_t generation=0;
// Numeric identities only, retained across same-process reloads for actors
// actually visited by Tailor. No scene pointers or outfit data cross loads.
std::unordered_map<RE::FormID,RE::FormID> scopedActors;
std::unordered_set<RE::FormID> bodyReconciled;
struct NativeBaseline {RE::FormID base;std::string stem;std::uint64_t signature;};
std::unordered_map<RE::FormID,NativeBaseline> nativeBaselines;
std::unordered_set<RE::FormID> nativeRequested;
std::uint64_t NativeSignature(RE::TESNPC* npc){
    std::uint64_t hash=14695981039346656037ULL;
    auto add=[&](const void* ptr,std::size_t count){auto* bytes=static_cast<const unsigned char*>(ptr);for(std::size_t i=0;i<count;++i){hash^=bytes[i];hash*=1099511628211ULL;}};
    if(npc && npc->faceData)add(npc->faceData,sizeof(RE::TESNPC::FaceData));
    if(npc && npc->headParts && npc->numHeadParts>0)for(int i=0;i<npc->numHeadParts;++i){auto id=npc->headParts[i]?npc->headParts[i]->GetFormID():0;add(&id,sizeof(id));}
    return hash;
}
Face Head(RE::Actor* actor);
std::string TextureKey(std::string value){
    std::transform(value.begin(),value.end(),value.begin(),[](unsigned char c){return c=='/'?'\\':static_cast<char>(std::tolower(c));});return value;
}
bool RecoverNativeSelection(RE::Actor* actor,const char* stage){
    if(!actor || actor->IsPlayerRef() || !actor->Is3DLoaded() || !current || !applySelected)return false;
    auto* npc=actor->GetActorBase();if(!npc)return false;
    auto old=nativeBaselines.find(actor->GetFormID());if(old==nativeBaselines.end() || old->second.base!=npc->GetFormID())return false;
    std::shared_ptr<void> preset;current(manager,&preset,npc);if(!preset)return false;
    const auto* info=reinterpret_cast<const PresetPrefix*>(preset.get());auto stem=ExportedPresetStem(info->path);
    if(!stem || *stem!=old->second.stem || RE::TESForm::LookupByID<RE::TESRace>(info->race)!=actor->GetRace())return false;
    if(NativeSignature(npc)==old->second.signature)return false;
    // Never reset live preview geometry. Keep the old-world snapshot numeric;
    // reapply only the identical currently selected Mu object, once per load.
    using OpenFn=bool(*)();
    auto open=reinterpret_cast<OpenFn>(GetProcAddress(GetModuleHandleW(L"Tailor.dll"),"IsTailorOpen"));
    if(open && open())return true;
    if(nativeRequested.contains(actor->GetFormID()))return true;
    nativeRequested.insert(actor->GetFormID());
    AppearanceTrace::Snapshot(actor,"before selected Mu native-state reapply");
    const bool accepted=applySelected(manager,actor,preset);
    logger::info("Mu native-state recovery actor={:08X} stage='{}' current selected='{}' accepted={}; no preset reselection requested",actor->GetFormID(),stage,info->path,accepted);
    return true;
}
void RestoreBody(RE::Actor* actor,const char* stage){
    if(!current || !actor || actor->IsPlayerRef() || !actor->Is3DLoaded())return;
    auto* npc=actor->GetActorBase();if(!npc)return;
    auto scope=scopedActors.find(actor->GetFormID());
    if(scope==scopedActors.end() || scope->second!=npc->GetFormID() || bodyReconciled.contains(actor->GetFormID()))return;
    std::shared_ptr<void> preset;current(manager,&preset,npc);if(!preset)return;
    const auto* info=reinterpret_cast<const PresetPrefix*>(preset.get());
    auto stem=ExportedPresetStem(info->path);
    auto* expectedRace=RE::TESForm::LookupByID<RE::TESRace>(info->race);
    if(!stem || !expectedRace || expectedRace!=actor->GetRace())return;
    // A saved Mu selection alone does not establish current ownership. Another
    // appearance system may have replaced it. Defer while hidden/mismatched.
    const auto head=Head(actor);
    const auto expectedTint="Textures\\CharGen\\Exported\\"+*stem+".dds";
    if(!head.node || TextureKey(head.tint)!=TextureKey(expectedTint))return;
    // Mu's IsAppliedActor may be false after co-save reload even though its
    // current selection exists. Body colour recovery must not depend on that
    // transient flag or send a NiNodeUpdate to a handler which rejects it.
    const auto resource="SKSE\\Plugins\\CharGen\\Exported\\"+*stem+".jslot";
    AppearanceTrace::Snapshot(actor,stage);
    if(MuBodyTint::Restore(actor,resource)){
        bodyReconciled.insert(actor->GetFormID());
        AppearanceTrace::Snapshot(actor,"after selected Mu body colour recovery");
    }
}
Face Head(RE::Actor* actor){
    Face face;auto* root=actor?actor->GetFaceNodeSkinned():nullptr;if(!root)return face;
    RE::BSVisit::TraverseScenegraphGeometries(root,[&](RE::BSGeometry* g){
        std::string name=g->name.c_str();auto lower=name;std::transform(lower.begin(),lower.end(),lower.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
        if(lower.find("head")!=std::string::npos){
            face.node=reinterpret_cast<std::uintptr_t>(g);face.name=name;
            auto* shader=g->lightingShaderProp_cast();auto* material=shader?shader->material:nullptr;
            if(material && material->GetType()==RE::BSShaderMaterial::Type::kLighting){
                face.feature=static_cast<std::uint32_t>(material->GetFeature());
                if(material->GetFeature()==RE::BSShaderMaterial::Feature::kFaceGen){
                    auto* tint=static_cast<RE::BSLightingShaderMaterialFacegen*>(material)->tintTexture.get();
                    if(tint)face.tint=tint->name.c_str();
                }else if(material->GetFeature()==RE::BSShaderMaterial::Feature::kFaceGenRGBTint){
                    const auto& color=static_cast<RE::BSLightingShaderMaterialFacegenTint*>(material)->tintColor;
                    face.color={color.red,color.green,color.blue};
                }
            }
            return RE::BSVisit::BSVisitControl::kStop;
        }
        return RE::BSVisit::BSVisitControl::kContinue;
    });return face;
}
void Notify(RE::Actor* actor){
    if(!actor || !applied || !applied(manager,actor))return;
    auto state=states.find(actor->GetFormID());if(state==states.end())return;
    auto geometry=Geometry(actor);
    if(geometry==state->second.notifiedGeometry)return;
    // Mu's verified NiNodeUpdate handler queues face/body skin and hair updates;
    // notifying it does not rebuild the head or trigger a new preset selection.
    if(auto* source=SKSE::GetNiNodeUpdateEventSource()){
        state->second.notifiedGeometry=std::move(geometry);
        SKSE::NiNodeUpdateEvent event{actor};source->SendEvent(&event);
        logger::info("Mu compatibility: sent appearance update notification for {:08X}",actor->GetFormID());
    }
}
bool RefreshSelectedTint(RE::Actor* actor,const std::string& stem){
    const auto resource="Textures\\CharGen\\Exported\\"+stem+".dds";
    auto it=states.find(actor->GetFormID());if(it==states.end())return false;
    auto saved=it->second.selectedTint;
    // Reuse only a live texture observed on this actor's known-good selected
    // Mu face in this world. No guessed loader ABI or whole-material reload.
    if(!saved || TextureKey(saved->name.c_str())!=TextureKey(resource))return false;
    auto* root=actor->GetFaceNodeSkinned();if(!root)return false;
    const auto face=Head(actor);bool refreshed=false;
    RE::BSVisit::TraverseScenegraphGeometries(root,[&](RE::BSGeometry* geometry){
        if(face.name!=geometry->name.c_str())return RE::BSVisit::BSVisitControl::kContinue;
        auto* shader=geometry->lightingShaderProp_cast();auto* raw=shader?shader->material:nullptr;
        if(!raw || raw->GetFeature()!=RE::BSShaderMaterial::Feature::kFaceGen)return RE::BSVisit::BSVisitControl::kStop;
        auto* material=static_cast<RE::BSLightingShaderMaterialFacegen*>(raw);
        // FaceGen's A0 tintTexture is confirmed by the local CommonLib layout
        // and logs. Preserve normal/diffuse/overlays and the other material
        // channels; the valid selected texture survives head reconstruction.
        material->tintTexture=saved;
        refreshed=true;
        logger::info("Mu compatibility: refreshed selected face tint actor={:08X} head='{}' resource='{}'",actor->GetFormID(),face.name,resource);
        return RE::BSVisit::BSVisitControl::kStop;
    });return refreshed;
}
void VerifyFaceRestore(RE::ActorHandle handle,std::uint64_t stamp,std::uint64_t serial,bool last){
    std::lock_guard lock(mutex);if(stamp!=generation)return;
    auto ref=handle.get();auto* actor=ref.get();if(!actor)return;
    auto it=states.find(actor->GetFormID());if(it==states.end() || !it->second.inFlight || it->second.restoreSerial!=serial)return;
    auto& state=it->second;
    std::shared_ptr<void> preset;current(manager,&preset,actor->GetActorBase());
    const auto* info=preset?reinterpret_cast<const PresetPrefix*>(preset.get()):nullptr;
    const auto stem=info?ExportedPresetStem(info->path):std::nullopt;
    if(!stem || *stem!=state.restoringStem || RE::TESForm::LookupByID<RE::TESRace>(info->race)!=actor->GetRace()){
        state.inFlight=false;logger::warn("Mu compatibility: selected preset changed during face restore; verification cancelled");return;
    }
    const auto expected="Textures\\CharGen\\Exported\\"+*stem+".dds";
    auto face=Head(actor);
    // Retain the existing optional Mu skin/hair refresh when its handler can
    // accept it, once after the new head exists rather than at API return.
    if(face.node && !state.restoreNotified && applied && applied(manager,actor)){
        state.restoreNotified=true;Notify(actor);face=Head(actor);
    }
    // The Papyrus return means the preset request was accepted, not that its
    // queued geometry/tint update completed. Keep the known-good baseline.
    if(face.node && TextureKey(face.tint)!=TextureKey(expected)){
        RefreshSelectedTint(actor,*stem);face=Head(actor);
    }
    const bool verified=face.node && TextureKey(face.tint)==TextureKey(expected) &&
        (state.face.name.empty() || face.name==state.face.name);
    AppearanceTrace::Snapshot(actor,"selected face restore verification");
    logger::info("Mu compatibility: face restore verification actor={:08X} tintVerified={} final={} actual='{}' expected='{}'",actor->GetFormID(),verified,last,face.tint,expected);
    if(last){
        state.inFlight=false;
        if(verified){state.face=face;state.notifiedGeometry=Geometry(actor);}
        else logger::warn("Mu compatibility: face restore NOT verified; damaged face was not accepted as baseline");
    }
}
class Callback final:public RE::BSScript::IStackCallbackFunctor {
public:Callback(RE::ActorHandle actor,std::uint64_t stamp,std::uint64_t serial):actor(actor),stamp(stamp),serial(serial){}
void operator()(RE::BSScript::Variable result)override{
    const bool ok=result.IsBool() && result.GetBool();auto handle=actor;auto gen=stamp;auto request=serial;
    SKSE::GetTaskInterface()->AddTask([handle,gen,request,ok]{
        std::lock_guard lock(mutex);if(gen!=generation)return;auto ref=handle.get();auto* actor=ref.get();if(!actor)return;
        auto it=states.find(actor->GetFormID());if(it==states.end() || it->second.restoreSerial!=request)return;
        logger::info("Mu compatibility: selected face request actor={:08X} accepted={} (completion not yet verified)",actor->GetFormID(),ok);
        if(!ok){it->second.inFlight=false;return;}
        std::thread([handle,gen,request]{for(auto delay:{200,500,1000,2000}){
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            SKSE::GetTaskInterface()->AddTask([handle,gen,request,last=delay==2000]{VerifyFaceRestore(handle,gen,request,last);});
        }}).detach();
    });
}
void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&)override{}
private:RE::ActorHandle actor;std::uint64_t stamp,serial;
};
void Check(RE::Actor* actor){
    if(!current || !actor || actor->IsPlayerRef() || !actor->Is3DLoaded())return;
    auto it=states.find(actor->GetFormID());if(it==states.end() || it->second.inFlight)return;
    if(RecoverNativeSelection(actor,"appearance check"))return;
    RestoreBody(actor,"visible selected Mu body colour check");
    const auto now=Head(actor);
    // A different allocation is normal after armor/3D reconstruction. It is
    // not a reason to call RaceMenu SetRace/replace the NPC's preset again.
    auto previous=it->second.face;auto semantic=now;previous.node=semantic.node=0;
    if(now.node && semantic==previous){it->second.face=now;Notify(actor);return;}
    // Covering the head can leave no face to capture. When it returns we must
    // restore the selected preset instead of permanently rejecting the actor.
    if(!now.node || now==it->second.face){Notify(actor);return;}
    std::shared_ptr<void> preset;current(manager,&preset,actor->GetActorBase());if(!preset)return;
    const auto* info=reinterpret_cast<const PresetPrefix*>(preset.get());auto stem=ExportedPresetStem(info->path);
    if(!stem){logger::warn("Mu compatibility: unsupported selected export path '{}'; no guessed preset used",info->path);return;}
    auto path=*stem;
    const auto presetResource="SKSE\\Plugins\\CharGen\\Exported\\"+path+".jslot";
    const auto tintResource="Textures\\CharGen\\Exported\\"+path+".dds";
    RE::BSResourceNiBinaryStream presetStream(presetResource),tintStream(tintResource);
    if(!presetStream.good() || !tintStream.good()){
        logger::warn("Mu compatibility: selected export incomplete: preset='{}' available={}, tint='{}' available={}; restore skipped",presetResource,presetStream.good(),tintResource,tintStream.good());return;
    }
    auto* race=RE::TESForm::LookupByID<RE::TESRace>(info->race);
    if(path.empty() || !race || race!=actor->GetRace()){logger::warn("Mu compatibility: current preset/race unavailable; no guessed preset used");return;}
    auto* vm=RE::BSScript::Internal::VirtualMachine::GetSingleton();if(!vm)return;
    const auto& before=it->second.face;
    logger::info("Mu compatibility: face changed on {:08X}: '{}' node={:X} -> '{}' node={:X}; tint='{}' -> '{}', shader={} -> {}, color=({},{},{}) -> ({},{},{}); restore selected preset '{}'",actor->GetFormID(),before.name,before.node,now.name,now.node,before.tint,now.tint,before.feature,now.feature,before.color[0],before.color[1],before.color[2],now.color[0],now.color[1],now.color[2],path);
    AppearanceTrace::Snapshot(actor,"before Mu face restore");it->second.inFlight=true;it->second.restoringStem=path;it->second.restoreNotified=false;
    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback{new Callback(actor->GetHandle(),generation,++it->second.restoreSerial)};
    auto requestGeneration=static_cast<std::int32_t>(generation);
    auto* args=RE::MakeFunctionArguments(std::move(actor),std::move(race),std::move(path),std::move(requestGeneration));
    if(!vm->DispatchStaticCall("TailorDDFreeWear","RestoreFace",args,callback))it->second.inFlight=false;
}
bool Allowed(RE::StaticFunctionTag*,RE::Actor* actor,std::int32_t requestGeneration){
    std::lock_guard lock(mutex);if(!actor || requestGeneration!=static_cast<std::int32_t>(generation))return false;
    auto it=states.find(actor->GetFormID());return it!=states.end() && it->second.inFlight;
}
}
bool RegisterPapyrus(RE::BSScript::IVirtualMachine* vm){vm->RegisterFunction("AppearanceAllowed","TailorDDFreeWear",Allowed);return true;}
void Init(){
    auto module=GetModuleHandleW(L"MuDynamicNPCReplacer.dll");if(!module)return;
    std::array<wchar_t,32768> path{};auto n=GetModuleFileNameW(module,path.data(),static_cast<DWORD>(path.size()));if(!n || n>=path.size())return;
    std::ifstream f(std::filesystem::path(path.data()),std::ios::binary);std::vector<unsigned char>b{std::istreambuf_iterator<char>(f),{}};std::array<unsigned char,32>digest{};
    if(b.empty() || BCryptHash(BCRYPT_SHA256_ALG_HANDLE,nullptr,0,b.data(),static_cast<ULONG>(b.size()),digest.data(),32)<0)return;
    constexpr char hex[]="0123456789abcdef";std::string hash;for(auto c:digest){hash+=hex[c>>4];hash+=hex[c&15];}
    if(hash!="009113664ae10e406914bc6fa8a394290aaa03cfd68367dce154e84543c7da0a"){logger::warn("Mu compatibility unavailable for unreviewed DLL {}",hash);return;}
    auto base=reinterpret_cast<std::uintptr_t>(module);
    constexpr std::array<unsigned char,24> prefix{72,137,92,36,8,85,86,87,65,84,65,85,65,86,65,87,72,129,236,144,0,0,0,73};
    constexpr std::array<unsigned char,24> appliedPrefix{65,87,72,131,236,48,76,139,249,72,133,210,117,9,50,192,72,131,196,48,65,95,195,72};
    if(std::memcmp(reinterpret_cast<void*>(base+0x73560),prefix.data(),24) || std::memcmp(reinterpret_cast<void*>(base+0x72c00),appliedPrefix.data(),24)){logger::error("Mu compatibility entry point changed; disabled");return;}
    manager=reinterpret_cast<void*>(base+0x179090);current=reinterpret_cast<CurrentFn>(base+0x73560);applied=reinterpret_cast<AppliedFn>(base+0x72c00);
    constexpr std::array<unsigned char,24> selectedPrefix{72,137,92,36,32,85,86,87,65,84,65,85,65,86,65,87,72,141,108,36,192,72,129,236};
    if(!std::memcmp(reinterpret_cast<void*>(base+0x61d80),selectedPrefix.data(),selectedPrefix.size()))applySelected=reinterpret_cast<ApplySelectedFn>(base+0x61d80);
    else logger::warn("Mu selected-preset reapply entry changed; native-state recovery disabled");
    logger::info("Optional Mu 2.1.4 appearance adapter enabled; reads current selected preset without changing Mu files");
}
void Clear(bool forgetActors){std::lock_guard lock(mutex);++generation;states.clear();bodyReconciled.clear();nativeRequested.clear();if(forgetActors){scopedActors.clear();nativeBaselines.clear();}}
void Before(RE::Actor* actor){
    std::lock_guard lock(mutex);if(!current || !actor || actor->IsPlayerRef() || states.contains(actor->GetFormID()))return;
    std::shared_ptr<void> preset;current(manager,&preset,actor->GetActorBase());
    if(!preset){logger::info("Mu compatibility: no current saved selection for actor {:08X}; no guessed preset used",actor->GetFormID());return;}
    const auto* info=reinterpret_cast<const PresetPrefix*>(preset.get());
    scopedActors[actor->GetFormID()]=actor->GetActorBase()->GetFormID();
    states.try_emplace(actor->GetFormID(),State{Head(actor),false,Geometry(actor)});
    if(auto stem=ExportedPresetStem(info->path)){
        const auto expected="Textures\\CharGen\\Exported\\"+*stem+".dds";
        if(applied(manager,actor) && TextureKey(states.at(actor->GetFormID()).face.tint)==TextureKey(expected))
            nativeBaselines.insert_or_assign(actor->GetFormID(),NativeBaseline{actor->GetActorBase()->GetFormID(),*stem,NativeSignature(actor->GetActorBase())});
        if(auto* root=actor->GetFaceNodeSkinned())RE::BSVisit::TraverseScenegraphGeometries(root,[&](RE::BSGeometry* geometry){
            auto* shader=geometry->lightingShaderProp_cast();auto* raw=shader?shader->material:nullptr;
            if(states.at(actor->GetFormID()).face.name==geometry->name.c_str() && raw && raw->GetFeature()==RE::BSShaderMaterial::Feature::kFaceGen){
                auto texture=static_cast<RE::BSLightingShaderMaterialFacegen*>(raw)->tintTexture;
                if(texture && TextureKey(texture->name.c_str())==TextureKey(expected))states.at(actor->GetFormID()).selectedTint=texture;
                return RE::BSVisit::BSVisitControl::kStop;
            }
            return RE::BSVisit::BSVisitControl::kContinue;
        });
    }
    logger::info("Mu compatibility: captured selected preset '{}' for {:08X}; runtimeApplied={}",info->path,actor->GetFormID(),applied(manager,actor));
    // Tailor's hook entry does not itself prove the caller's thread. Defer
    // the new body-tint mutation to the game task queue in every call path.
    const auto handle=actor->GetHandle();const auto stamp=generation;
    SKSE::GetTaskInterface()->AddTask([handle,stamp]{
        std::lock_guard lock(mutex);if(stamp!=generation)return;
        auto ref=handle.get();if(auto* target=ref.get())RestoreBody(target,"first Tailor selected Mu body colour check");
    });
}
void After(RE::Actor* actor){
    std::lock_guard lock(mutex);if(!current || !actor || actor->IsPlayerRef())return;
    const bool alreadyTracked=states.contains(actor->GetFormID());
    Before(actor);
    if(!states.contains(actor->GetFormID()))return;
    // If the first observation comes from a completion callback, it is not
    // a known-good pre-change face. Restore the selected preset once visible.
    if(!alreadyTracked)states[actor->GetFormID()].face={};
    auto handle=actor->GetHandle();auto stamp=generation;auto checks=++states.at(actor->GetFormID()).checks;
    std::thread([handle,stamp,checks]{for(auto wait:{300,1200,2500}){std::this_thread::sleep_for(std::chrono::milliseconds(wait));SKSE::GetTaskInterface()->AddTask([handle,stamp,checks]{std::lock_guard lock(mutex);if(stamp!=generation)return;auto ref=handle.get();if(auto* actor=ref.get()){auto it=states.find(actor->GetFormID());if(it!=states.end() && it->second.checks==checks)Check(actor);}});}}).detach();
}
void AfterLoad(const std::vector<RE::FormID>& managedActors){
    std::lock_guard lock(mutex);if(!current)return;
    for(auto id:managedActors)if(auto* actor=RE::TESForm::LookupByID<RE::Actor>(id))
        if(auto* npc=actor->GetActorBase())scopedActors[id]=npc->GetFormID();
    const auto stamp=generation;
    // Mu, RaceMenu and the engine finish their load work asynchronously. A
    // bounded settling window handles newly ready 3D without a permanent poll.
    std::thread([stamp]{for(auto wait:{1500,2500,5000}){
        std::this_thread::sleep_for(std::chrono::milliseconds(wait));
        {std::lock_guard lock(mutex);if(stamp!=generation)return;}
        SKSE::GetTaskInterface()->AddTask([stamp]{
            std::lock_guard lock(mutex);if(stamp!=generation)return;
            // Do not repaint every Mu NPC. Only bridge-managed / previously
            // fitted identities qualify, with base, race and head-tint guards.
            for(const auto& [id,base]:scopedActors)
                if(auto* actor=RE::TESForm::LookupByID<RE::Actor>(id)){
                    if(!RecoverNativeSelection(actor,"post-load check"))RestoreBody(actor,"post-load selected Mu body colour check");
                    // Record later settling checks even after the first repair
                    // succeeds, so a later overwrite is visible in the log.
                    if(auto* npc=actor->GetActorBase();npc && npc->GetFormID()==base)
                        AppearanceTrace::Snapshot(actor,"post-load Mu body colour settling observation");
                }
        });
    }}).detach();
}
}
