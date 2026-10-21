#include "PCH.h"
#include "PartialPoseCompatibility.h"
#include "PartialPoseContext.h"
#include <atomic>
#include <mutex>

namespace PartialPoseCompatibility {
namespace {
using ActorApplyFn=bool(*)(const void*,RE::FormID,RE::NiAVObject*);
using ApplyFn=void(*)(const void*,RE::NiAVObject*);
using EvalFn=bool(*)(const void*,RE::Actor*);
ActorApplyFn originalActorApply=nullptr;
ApplyFn originalApply=nullptr;
EvalFn evaluate=nullptr;
std::once_flag initialized;
std::atomic_bool installed{false};
std::atomic<RE::FormID> previewActor{0};
std::atomic_uint rejectedReports{0},acceptedReports{0};
std::atomic_uint ownerReports{0};

bool ApplyToActor(const void* opaqueCache,RE::FormID id,RE::NiAVObject* root){
    RE::Actor* actor=nullptr;
    if(id && id!=0x14 && id==previewActor.load(std::memory_order_acquire) && root){
        auto* ref=root->GetUserData();
        auto* candidate=ref?ref->As<RE::Actor>():nullptr;
        if(candidate && candidate->GetFormID()==id && !candidate->IsPlayerRef())actor=candidate;
        else if(ownerReports.fetch_add(1)<4)
            logger::warn("PAR fitting actor/root identity unavailable actor={:08X}; original cached application retained",id);
    }
    // An unrelated nested actor call must mask the outer context too.
    PartialPoseContext::Scope scope({actor,id,root});
    return originalActorApply(opaqueCache,id,root);
}
void Apply(const void* opaqueReplacer,RE::NiAVObject* root){
    if(PartialPoseContext::Matches(previewActor.load(std::memory_order_acquire),root)){
        auto* actor=static_cast<RE::Actor*>(const_cast<void*>(PartialPoseContext::current->actor));
        // Call PAR's own condition evaluator. Its opaque object stays owned by
        // the original cache/loop. No copied implementation or object layout.
        const bool allowed=evaluate(opaqueReplacer,actor);
        auto& reports=allowed?acceptedReports:rejectedReports;
        const auto limit=allowed?4u:64u;
        if(reports.fetch_add(1)<limit)
            logger::info("PAR fitting cached pose rechecked actor={:08X} replacer={} currentAllowed={}; {}",actor->GetFormID(),opaqueReplacer,allowed,allowed?"original pose retained":"stale bone override skipped");
        if(!allowed)return;
    }
    originalApply(opaqueReplacer,root);
}

std::string Hash(const std::filesystem::path& path){
    std::ifstream file(path,std::ios::binary);
    std::vector<unsigned char> data{std::istreambuf_iterator<char>(file),{}};
    std::array<unsigned char,32> digest{};
    if(data.empty() || file.bad() || BCryptHash(BCRYPT_SHA256_ALG_HANDLE,nullptr,0,data.data(),static_cast<ULONG>(data.size()),digest.data(),static_cast<ULONG>(digest.size()))<0)return {};
    constexpr char hex[]="0123456789abcdef";std::string text;
    for(auto c:digest){text+=hex[c>>4];text+=hex[c&15];}return text;
}
void Init(){
    // Exact local PAR 0.2.0 DLL + matching PDB and real actor/loop callsites.
    // Public reference: github.com/naitro2010/PartialAnimationReplacer.
    // No third-party headers, implementation, JSON or DLL are redistributed.
    auto module=GetModuleHandleW(L"PartialAnimationReplacer.dll");
    if(!module){logger::info("PAR fitting adapter inactive: optional PAR runtime absent");return;}
    std::array<wchar_t,32768> path{};
    const auto length=GetModuleFileNameW(module,path.data(),static_cast<DWORD>(path.size()));
    if(!length || length>=path.size() || Hash(path.data())!="fa07d8355ee075c4d42a02a7c426524e88d1c027998b0446660c58b768b8030a"){
        logger::warn("PAR fitting adapter skipped: unreviewed runtime; original PAR untouched");return;
    }
    const auto base=reinterpret_cast<std::uintptr_t>(module);
    auto* actorEntry=reinterpret_cast<void*>(base+0x86cb0);
    auto* applyEntry=reinterpret_cast<void*>(base+0x82920);
    auto* evalEntry=reinterpret_cast<void*>(base+0x828f0);
    constexpr unsigned char actorPrefix[]{0x40,0x56,0x48,0x83,0xec,0x20,0x4c,0x8b,0x11,0x48,0xb8,0x25,0x23,0x22,0x84,0xe4};
    constexpr unsigned char applyPrefix[]{0x48,0x8b,0xc4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20};
    constexpr unsigned char evalPrefix[]{0x48,0x83,0xec,0x28,0x48,0x8b,0x49,0x40,0x48,0x85,0xc9,0x74,0x13};
    if(std::memcmp(actorEntry,actorPrefix,sizeof(actorPrefix)) ||
       std::memcmp(applyEntry,applyPrefix,sizeof(applyPrefix)) ||
       std::memcmp(evalEntry,evalPrefix,sizeof(evalPrefix))){
        logger::warn("PAR fitting adapter skipped: reviewed entry changed; existing hooks left untouched");return;
    }
    if(MH_CreateHook(actorEntry,reinterpret_cast<void*>(&ApplyToActor),reinterpret_cast<void**>(&originalActorApply))!=MH_OK){
        logger::warn("PAR fitting adapter unavailable: actor hook could not be prepared");return;
    }
    if(MH_CreateHook(applyEntry,reinterpret_cast<void*>(&Apply),reinterpret_cast<void**>(&originalApply))!=MH_OK){
        MH_RemoveHook(actorEntry);logger::warn("PAR fitting adapter unavailable: pose hook preparation rolled back");return;
    }
    evaluate=reinterpret_cast<EvalFn>(evalEntry);
    bool enabled=MH_QueueEnableHook(actorEntry)==MH_OK && MH_QueueEnableHook(applyEntry)==MH_OK;
    if(!enabled || MH_ApplyQueued()!=MH_OK){
        MH_DisableHook(actorEntry);MH_DisableHook(applyEntry);
        MH_RemoveHook(actorEntry);MH_RemoveHook(applyEntry);
        logger::warn("PAR fitting adapter unavailable: activation rolled back");return;
    }
    installed.store(true,std::memory_order_release);
    logger::info("PAR fitting live-condition adapter installed: exact PAR 0.2.0; active NPC and root only; valid cached poses and other actors retained");
}
}
void SetPreview(RE::FormID id){
    std::call_once(initialized,Init);
    if(!installed.load(std::memory_order_acquire))return;
    auto old=previewActor.exchange(id,std::memory_order_acq_rel);
    if(old!=id)logger::info("PAR fitting scope actor={:08X}; ordinary outfits included",id);
}
void Clear(RE::FormID id){
    auto expected=id;
    if(id && previewActor.compare_exchange_strong(expected,0,std::memory_order_acq_rel))logger::info("PAR fitting scope cleared actor={:08X}",id);
}
void ClearAll(){
    const auto old=previewActor.exchange(0,std::memory_order_acq_rel);
    if(old)logger::info("PAR fitting scope cleared actor={:08X}; normal cached application resumed",old);
    acceptedReports.store(0);rejectedReports.store(0);
    ownerReports.store(0);
}
}
