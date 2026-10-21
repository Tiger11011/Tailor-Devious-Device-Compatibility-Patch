#include "PCH.h"
#include "PreviewAnimations.h"
#include "HavokStringAddress.h"
#include "PreviewIdlePolicy.h"
#include "PartialPoseCompatibility.h"
#include "OAR/OpenAnimationReplacerAPI-Conditions.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <intrin.h>

namespace PreviewAnimations {
    namespace {
        enum class Pose : std::uint8_t {
            ElbowTie, FrontCuffs, BreastYoke, Elbowbinder, Yoke, Armbinder, PetSuit
        };

        constexpr std::uint32_t Bit(Pose pose) {
            return std::uint32_t{1} << static_cast<std::uint8_t>(pose);
        }

        struct KeywordRule {
            std::string_view plugin;
            RE::FormID localID;
            Pose pose;
        };

        // Exact predicates from the installed DD NG "DD to OAR" presets and
        // submods 100001..100006 / 100300. Only their classification is read.
        constexpr std::array rules{
            KeywordRule{"Devious Devices - Integration.esm", 0x08A76C, Pose::ElbowTie},
            KeywordRule{"Devious Devices - Integration.esm", 0x063AD9, Pose::FrontCuffs},
            KeywordRule{"Devious Devices - Integration.esm", 0x062538, Pose::BreastYoke},
            KeywordRule{"Devious Devices - Integration.esm", 0x062539, Pose::Elbowbinder},
            KeywordRule{"Devious Devices - Assets.esm", 0x02C531, Pose::Yoke},
            KeywordRule{"Devious Devices - Assets.esm", 0x00CA3A, Pose::Armbinder},
            KeywordRule{"Devious Devices - Integration.esm", 0x060A46, Pose::Armbinder},
            KeywordRule{"Devious Devices - Integration.esm", 0x086C1D, Pose::PetSuit}
        };

        std::atomic_bool ready{false};
        std::once_flag initialized;
        std::shared_mutex stateMutex;
        std::unordered_map<RE::FormID, std::uint32_t> actorPoses;
        std::array<std::atomic_uint64_t,7> evaluations{},matches{};
        using NotifyFn=bool(*)(RE::IAnimationGraphManagerHolder*,const RE::BSFixedString&);
        std::atomic<NotifyFn> previousNotify{nullptr};
        std::once_flag idleGuard;
        std::atomic_uint idleReports{0};
        using PickIdleFn=bool(*)(void*,RE::Actor*,const void*);
        std::atomic<PickIdleFn> previousPickIdle{nullptr};
        std::atomic_uint pickReports{0};
        bool IsPetPreview(RE::Actor* actor){
            if(!actor)return false;
            const std::shared_lock lock(stateMutex);auto it=actorPoses.find(actor->GetFormID());
            return it!=actorPoses.end() && (it->second&Bit(Pose::PetSuit));
        }
        bool PickNewIdle(void* self,RE::Actor* actor,const void* payload){
            // The reviewed handler actually consumes RDX=Actor*. Its unused
            // this/payload registers stay opaque and are forwarded unchanged.
            if(actor && PreviewIdlePolicy::BlockAutomaticReselection(IsPetPreview(actor),actor->IsPlayerRef())){
                if(pickReports.fetch_add(1)<64)
                    logger::info("Pet preview handled PickNewIdle without requesting a random idle: actor={:08X}; Character event forwarding and other handlers unchanged",actor->GetFormID());
                // Original PickNewIdleHandler returns true after setting the
                // new-idle flag. Report it handled, without setting that flag.
                return true;
            }
            auto next=previousPickIdle.load(std::memory_order_acquire);
            return next?next(self,actor,payload):false;
        }
        void InstallPickIdleGuard(){
            // The actor-event factory's PickNewIdleHandler, not the global
            // animation dispatcher or AI update. Exact 1170 binary, RTTI,
            // CommonLib IHandlerFunctor slot and real callsite were reviewed.
            if(REL::Module::get().version()!=REL::Version{1,6,1170,0}){
                logger::warn("Pet PickNewIdle handler guard unavailable: this runtime has not been reviewed");return;
            }
            REL::Relocation<std::uintptr_t> table{RE::VTABLE_PickNewIdleHandler[0]};
            const auto base=REL::Module::get().base();
            const auto current=*reinterpret_cast<std::uintptr_t*>(table.address()+sizeof(std::uintptr_t));
            constexpr std::array<unsigned char,16> prefix{0x48,0x83,0xec,0x28,0x48,0x8b,0x8a,0xf8,0,0,0,0x48,0x85,0xc9,0x74,7};
            if(table.address()!=base+0x18b9db8 || current!=base+0x7b9620 ||
                std::memcmp(reinterpret_cast<const void*>(current),prefix.data(),prefix.size())){
                logger::warn("Pet PickNewIdle handler guard skipped: reviewed entry differs; existing handler left untouched");return;
            }
            previousPickIdle.store(reinterpret_cast<PickIdleFn>(current),std::memory_order_release);
            auto replaced=table.write_vfunc(1,&PickNewIdle);
            previousPickIdle.store(reinterpret_cast<PickIdleFn>(replaced),std::memory_order_release);
            logger::info("Pet PickNewIdle source guard installed: exact 1170 handler; per-NPC preview scope; no AI disable or graph-asset edits");
        }
        bool Notify(RE::IAnimationGraphManagerHolder* holder,const RE::BSFixedString& event){
            auto* ref=static_cast<RE::TESObjectREFR*>(holder);
            auto* actor=ref?ref->As<RE::Actor>():nullptr;
            const bool pet=IsPetPreview(actor);
            const std::string_view name=event.c_str()?event.c_str():"";
            if(PreviewIdlePolicy::Suppress(pet,actor && actor->IsPlayerRef(),name)){
                if(idleReports.fetch_add(1)<64){
                    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
                    HMODULE module=nullptr;
                    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCWSTR>(caller),&module);
                    std::array<wchar_t,32768> path{};if(module)GetModuleFileNameW(module,path.data(),static_cast<DWORD>(path.size()));
                    logger::info("Pet preview suppressed competing idle actor={:08X} event='{}' caller={}+{:X}",actor->GetFormID(),name,module?std::filesystem::path(path.data()).filename().string():"unknown",caller-reinterpret_cast<std::uintptr_t>(module));
                }
                return false;
            }
            auto next=previousNotify.load(std::memory_order_acquire);
            return next?next(holder,event):false;
        }
        void InstallIdleGuard(){
            std::call_once(idleGuard,[]{
                // Same holder vtable and ABI as OAR's own Character notify
                // hook. Chain the live entry, including OAR/other wrappers.
                REL::Relocation<std::uintptr_t> table{RE::VTABLE_Character[3]};
                previousNotify.store(reinterpret_cast<NotifyFn>(*reinterpret_cast<std::uintptr_t*>(table.address()+sizeof(std::uintptr_t))),std::memory_order_release);
                auto replaced=table.write_vfunc(1,&Notify);
                previousNotify.store(reinterpret_cast<NotifyFn>(replaced),std::memory_order_release);
                logger::info("Pet preview idle guard installed on Character holder; exact HH events only, existing notify chain retained");
                InstallPickIdleGuard();
            });
        }

        // OAR evaluates conditions on animation threads. The caller has already
        // reduced engine armor data to numeric state on the main thread.
        bool Matches(RE::FormID actorID, Pose pose) {
            if (!ready.load(std::memory_order_acquire) || actorID == 0) return false;
            const std::shared_lock lock(stateMutex);
            const auto found = actorPoses.find(actorID);
            if(found==actorPoses.end())return false;
            const auto index=static_cast<std::size_t>(pose);
            ++evaluations[index];
            const bool match=(found->second & Bit(pose))!=0;
            if(match && matches[index].fetch_add(1)==0)
                logger::info("OAR preview condition first actual match: actor={:08X} class={}",actorID,index);
            return match;
        }

        template <Pose SelectedPose>
        class PreviewCondition final : public Conditions::CustomCondition {
        public:
            static constexpr std::string_view CONDITION_NAME = [] {
                if constexpr (SelectedPose == Pose::ElbowTie) return "TailorDDPreviewElbowTie"sv;
                else if constexpr (SelectedPose == Pose::FrontCuffs) return "TailorDDPreviewFrontCuffs"sv;
                else if constexpr (SelectedPose == Pose::BreastYoke) return "TailorDDPreviewBreastYoke"sv;
                else if constexpr (SelectedPose == Pose::Elbowbinder) return "TailorDDPreviewElbowbinder"sv;
                else if constexpr (SelectedPose == Pose::Yoke) return "TailorDDPreviewYoke"sv;
                else if constexpr (SelectedPose == Pose::Armbinder) return "TailorDDPreviewArmbinder"sv;
                else return "TailorDDPreviewPetSuit"sv;
            }();

            RE::BSString GetName() const override { return CONDITION_NAME.data(); }
            RE::BSString GetDescription() const override {
                return "The actor is previewing a matching cosmetic DD armor in the active Tailor fitting session.";
            }
            constexpr REL::Version GetRequiredVersion() const override { return {0, 6, 7, 0}; }

        protected:
            bool EvaluateImpl(RE::TESObjectREFR* actor, RE::hkbClipGenerator*, void*) const override {
                return actor && Matches(actor->GetFormID(), SelectedPose);
            }
        };

        template <Pose SelectedPose>
        bool Register() {
            using Condition = PreviewCondition<SelectedPose>;
            const auto result = OAR_API::Conditions::AddCustomCondition<Condition>();
            if (result != OAR_API::Conditions::APIResult::OK) {
                logger::error("OAR preview condition {} registration failed ({}); preview poses disabled",
                    Condition::CONDITION_NAME, static_cast<unsigned>(result));
                return false;
            }
            return true;
        }
    }

    void Init() {
        std::call_once(initialized, [] {
            // Request the documented interface rather than calling PAR/OAR
            // internal functions or accepting an unknown binary layout.
            if (!OAR_API::Conditions::GetAPI(OAR_API::Conditions::InterfaceVersion::V3)) {
                logger::warn("OAR Conditions API V3 unavailable; cosmetic fitting remains enabled without preview poses");
                return;
            }
            // Complete registration even after a failure, but leave every
            // condition false unless the entire set was registered successfully.
            const std::array results{
                Register<Pose::ElbowTie>(), Register<Pose::FrontCuffs>(),
                Register<Pose::BreastYoke>(), Register<Pose::Elbowbinder>(),
                Register<Pose::Yoke>(), Register<Pose::Armbinder>(), Register<Pose::PetSuit>()
            };
            const bool success = std::ranges::all_of(results, [](bool result) { return result; });
            ready.store(success, std::memory_order_release);
            if (success) logger::info("OAR preview conditions registered: 7 classes; no DD gameplay keywords or controls changed");
        });
    }

    bool Ready() { return ready.load(std::memory_order_acquire); }

    void Trace(RE::Actor* actor){
        if(!actor)return;
        {const std::shared_lock lock(stateMutex);auto it=actorPoses.find(actor->GetFormID());if(it==actorPoses.end() || !(it->second&Bit(Pose::PetSuit)))return;}
        RE::BSTSmartPointer<RE::BSAnimationGraphManager> manager;
        if(!actor->GetAnimationGraphManager(manager) || !manager)return;
        struct Clip {std::string behavior,node,path,boundPath;unsigned bindingIndex,mode;float time,speed;bool ended;};std::vector<Clip> clips;
        {
            RE::BSSpinLockGuard lock(manager->GetRuntimeData().updateLock);
            for(const auto& graph:manager->graphs){
                auto* behavior=graph?graph->behaviorGraph:nullptr;
                if(!behavior || !behavior->activeNodes || behavior->activeNodes->size()>4096)continue;
                const auto* setup=graph->characterInstance.setup.get();
                const auto* characterData=setup?setup->data.get():nullptr;
                const auto* strings=characterData?characterData->stringData.get():nullptr;
                for(const auto& info:*behavior->activeNodes){
                    auto* node=info.nodeClone;if(!node)continue;
                    auto* type=node->GetClassType();
                    if(!type || !type->name || std::string_view(type->name)!="hkbClipGenerator")continue;
                    auto* clip=static_cast<RE::hkbClipGenerator*>(node);
                    // This resolved CommonLib build's hkStringPtr::data() uses
                    // a 32-bit mask and truncates high pointer bits. Keep the
                    // correction inside our adapter, not in the dependency.
                    const auto* name=HavokStringAddress::Read(clip->name);
                    const auto* path=HavokStringAddress::Read(clip->animationName);
                    const auto* owner=info.behavior?HavokStringAddress::Read(info.behavior->name):nullptr;
                    // OAR changes this binding index, not animationName. Read
                    // its actual path while the graph is locked, then retain
                    // only text/scalars. A matching condition alone does not
                    // prove which replacement won.
                    const auto* bound=HavokStringAddress::ReadIndexed(strings?&strings->animationNames:nullptr,clip->animationBindingIndex);
                    clips.push_back({owner?owner:"",name?name:"",path?path:"",bound?bound:"",clip->animationBindingIndex,clip->mode.underlying(),clip->localTime,clip->playbackSpeed,clip->atEnd});
                    if(clips.size()>=32)break;
                }
                if(clips.size()>=32)break;
            }
        }
        logger::info("Pet active-clip snapshot actor={:08X} clips={} conditionEvaluated={} matched={}",actor->GetFormID(),clips.size(),evaluations[6].load(),matches[6].load());
        for(const auto& clip:clips)logger::info("Pet active clip actor={:08X} behavior='{}' node='{}' original='{}' bound='{}' binding={} mode={} time={} speed={} atEnd={}",actor->GetFormID(),clip.behavior,clip.node,clip.path,clip.boundPath,clip.bindingIndex,clip.mode,clip.time,clip.speed,clip.ended);
    }

    void Set(RE::FormID actorID, const std::vector<RE::TESObjectARMO*>& renderedSources) {
        if (!actorID) return;
        PartialPoseCompatibility::SetPreview(actorID);
        std::uint32_t mask = 0;
        if (Ready() && !renderedSources.empty()) {
            if (auto* data = RE::TESDataHandler::GetSingleton()) {
                for (const auto& rule : rules) {
                    auto* keyword = data->LookupForm<RE::BGSKeyword>(rule.localID, rule.plugin);
                    if (!keyword) continue;
                    for (const auto* source : renderedSources) {
                        if (source && source->HasKeyword(keyword)) {
                            mask |= Bit(rule.pose);
                            break;
                        }
                    }
                }
            }
        }

        bool changed=false;
        if(mask&Bit(Pose::PetSuit))InstallIdleGuard();
        {
            const std::unique_lock lock(stateMutex);
            const auto old = actorPoses.find(actorID);
            const auto previous = old == actorPoses.end() ? 0 : old->second;
            changed=mask!=previous;
            if (mask) actorPoses.insert_or_assign(actorID, mask);
            else actorPoses.erase(actorID);
        }
        if (changed) {
            logger::info("OAR cosmetic preview snapshot: actor={:08X} classes={:02X}", actorID, mask);
            // Refresh the existing idle once per real preview-state change.
            // DD's own EndThirdPersonAnimation uses this same graph event;
            // OAR evaluates replacements when a new clip activates. Never hold
            // our state lock while the graph may synchronously evaluate it.
            if(auto* actor=RE::TESForm::LookupByID<RE::Actor>(actorID);actor && actor->Is3DLoaded() && !actor->IsDead())
                logger::info("OAR preview idle reevaluation actor={:08X} accepted={}",actorID,actor->NotifyAnimationGraph("IdleForceDefaultState"));
        }
    }

    void Clear(RE::FormID actorID) {
        if (!actorID) return;
        PartialPoseCompatibility::Clear(actorID);
        const std::unique_lock lock(stateMutex);
        if (actorPoses.erase(actorID)) logger::info("OAR cosmetic preview snapshot cleared: actor={:08X}", actorID);
    }

    void ClearAll() {
        PartialPoseCompatibility::ClearAll();
        const std::unique_lock lock(stateMutex);
        if (!actorPoses.empty()) logger::info("OAR cosmetic preview snapshots cleared: {} actors", actorPoses.size());
        for(unsigned i=0;i<7;++i){
            auto count=evaluations[i].exchange(0);auto passed=matches[i].exchange(0);
            if(count)logger::info("OAR actual condition evaluations class={} evaluated={} matched={}",i,count,passed);
        }
        actorPoses.clear();
        idleReports.store(0);
        pickReports.store(0);
    }
}
