#include "PCH.h"
#include "MuAppearance.h"
#include "AppearanceTrace.h"
#include "MuPresetPath.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <unordered_map>
namespace MuAppearance {namespace {
// This optional adapter reads only the current selection. It does not ask Mu
// to choose another preset, change actor data, or reload global configuration.
struct PresetPrefix {std::string name,path,tint,extra,body;std::byte bodyData[104];std::string npcName,npcShortName;RE::FormID race;};
static_assert(offsetof(PresetPrefix,path)==32 && offsetof(PresetPrefix,race)==328 && sizeof(std::shared_ptr<void>)==16);
using CurrentFn=void*(*)(void*,std::shared_ptr<void>*,RE::TESNPC*);
using AppliedFn=bool(*)(void*,RE::Actor*);
CurrentFn current=nullptr;AppliedFn applied=nullptr;void* manager=nullptr;
struct Face {
    std::uintptr_t node=0;std::string name,tint;std::uint32_t feature=0;
    std::array<float,3> color{};
    bool operator==(const Face&)const=default;
};
struct State {Face face;bool inFlight=false;};
std::unordered_map<RE::FormID,State> states;std::recursive_mutex mutex;std::uint64_t generation=0;
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
    // Mu's verified NiNodeUpdate handler queues face/body skin and hair updates;
    // notifying it does not rebuild the head or trigger a new preset selection.
    if(auto* source=SKSE::GetNiNodeUpdateEventSource()){
        SKSE::NiNodeUpdateEvent event{actor};source->SendEvent(&event);
        logger::info("Mu compatibility: sent appearance update notification for {:08X}",actor->GetFormID());
    }
}
class Callback final:public RE::BSScript::IStackCallbackFunctor {
public:Callback(RE::ActorHandle actor,std::uint64_t stamp):actor(actor),stamp(stamp){}
void operator()(RE::BSScript::Variable result)override{
    const bool ok=result.IsBool() && result.GetBool();auto handle=actor;auto gen=stamp;
    SKSE::GetTaskInterface()->AddTask([handle,gen,ok]{std::lock_guard lock(mutex);if(gen!=generation)return;auto ref=handle.get();auto* actor=ref.get();if(!actor)return;auto& state=states[actor->GetFormID()];state.inFlight=false;if(ok){state.face=Head(actor);Notify(actor);}logger::info("Mu compatibility: restoring currently selected face on {:08X}: success={}",actor->GetFormID(),ok);AppearanceTrace::Snapshot(actor,"after Mu face restore");});
}
void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&)override{}
private:RE::ActorHandle actor;std::uint64_t stamp;
};
void Check(RE::Actor* actor){
    if(!current || !actor || actor->IsPlayerRef() || !actor->Is3DLoaded())return;
    auto it=states.find(actor->GetFormID());if(it==states.end() || it->second.inFlight)return;
    Notify(actor);const auto now=Head(actor);
    // Covering the head can leave no face to capture. When it returns we must
    // restore the selected preset instead of permanently rejecting the actor.
    if(!now.node || now==it->second.face)return;
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
    AppearanceTrace::Snapshot(actor,"before Mu face restore");it->second.inFlight=true;
    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback{new Callback(actor->GetHandle(),generation)};
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
    logger::info("Optional Mu 2.1.4 appearance adapter enabled; reads current selected preset without changing Mu files");
}
void Clear(){std::lock_guard lock(mutex);++generation;states.clear();}
void Before(RE::Actor* actor){
    std::lock_guard lock(mutex);if(!current || !actor || actor->IsPlayerRef() || states.contains(actor->GetFormID()))return;
    std::shared_ptr<void> preset;current(manager,&preset,actor->GetActorBase());
    if(!preset){logger::info("Mu compatibility: no current saved selection for actor {:08X}; no guessed preset used",actor->GetFormID());return;}
    const auto* info=reinterpret_cast<const PresetPrefix*>(preset.get());
    states.try_emplace(actor->GetFormID(),State{Head(actor)});
    logger::info("Mu compatibility: captured selected preset '{}' for {:08X}; runtimeApplied={}",info->path,actor->GetFormID(),applied(manager,actor));
}
void After(RE::Actor* actor){
    std::lock_guard lock(mutex);if(!current || !actor || actor->IsPlayerRef())return;
    const bool alreadyTracked=states.contains(actor->GetFormID());
    Before(actor);
    if(!states.contains(actor->GetFormID()))return;
    // If the first observation comes from a completion callback, it is not
    // a known-good pre-change face. Restore the selected preset once visible.
    if(!alreadyTracked)states[actor->GetFormID()].face={};
    auto handle=actor->GetHandle();auto stamp=generation;
    std::thread([handle,stamp]{for(auto wait:{300,1200,2500}){std::this_thread::sleep_for(std::chrono::milliseconds(wait));SKSE::GetTaskInterface()->AddTask([handle,stamp]{std::lock_guard lock(mutex);if(stamp!=generation)return;auto ref=handle.get();if(auto* actor=ref.get())Check(actor);});}}).detach();
}
}
