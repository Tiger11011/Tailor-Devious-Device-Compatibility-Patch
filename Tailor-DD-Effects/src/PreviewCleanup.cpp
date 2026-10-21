#include "PCH.h"
#include "PreviewCleanup.h"
#include <algorithm>
#include <chrono>
#include <cctype>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

// Ownership, same-root/biped and worn-copy guards adapted from ColdSun's
// Tailor src/outfit/PreviewOutfitCleanup.cpp, GPL-3.0-or-later. This independent
// compatibility helper retires only the exact captured clone, including stale
// clone references whose item/addon have already changed during replacement.
namespace PreviewCleanup {
namespace {
struct KnownOutfits {
    RE::Actor* actor = nullptr;
    RE::NiAVObject* root = nullptr;
    std::unordered_set<RE::BGSOutfit*> outfits;
    std::uint64_t operation = 0;
    Snapshot active;
};
std::mutex mutex;
std::unordered_map<RE::FormID, KnownOutfits> known;
std::uint64_t generation = 1, nextOperation = 0;

bool Below(RE::NiAVObject* root, RE::NiAVObject* node) {
    for (auto* parent = node ? node->parent : nullptr; parent; parent = parent->parent)
        if (parent == root) return true;
    return false;
}
RE::FormID Id(const RE::TESForm* form) { return form ? form->GetFormID() : 0; }
const char* Protection(RE::Actor* actor, RE::TESObjectARMO* armor) {
    if (!armor) return "unresolved-armor";
    if (armor == actor->GetSkin()) return "current-actor-skin";
    if (armor->HasKeywordString("zad_InventoryDevice")) return "genuine-DD-inventory";
    if (armor->HasKeywordString("zad_Lockable")) return "genuine-DD-rendered-or-lockable";
    // Head, hair, long hair, circlet, ears and face are outside this fix.
    constexpr std::uint32_t headSlots = (1u << 0) | (1u << 1) | (1u << 11) | (1u << 12) | (1u << 13) | (1u << 14);
    return (armor->GetSlotMask().underlying() & headSlots) != 0 ? "protected-head-or-face-slot" : nullptr;
}
bool Excluded(RE::Actor* actor, RE::TESObjectARMO* armor) { return Protection(actor, armor) != nullptr; }
bool ExcludedNode(RE::NiAVObject* node) {
    std::string name = node ? node->name.c_str() : "";
    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    for (const auto* token : {"skeleton", "npc root", "facegen", "femalehead", "malehead", "body [ovl", "hands [ovl", "feet [ovl"})
        if (name.find(token) != std::string::npos) return true;
    return false;
}
}

struct State {
    struct Part {
        RE::FormID armor = 0;
        RE::NiPointer<RE::NiAVObject> clone;
        bool detached = false;
    };
    RE::ActorHandle actor;
    RE::FormID actorId = 0, outgoing = 0, incoming = 0;
    RE::BGSOutfit* incomingForm = nullptr;
    RE::NiPointer<RE::NiAVObject> root;
    RE::BSTSmartPointer<RE::BipedAnim> biped;
    std::unordered_set<RE::FormID> outgoingItems, incomingItems;
    std::vector<Part> parts;
    std::uint64_t generation = 0, operation = 0;
    bool owned = false, applied = false;
};

namespace {
bool Current(const State& state, RE::Actor* actor) {
    std::lock_guard lock(mutex);
    const auto it = known.find(state.actorId);
    return state.generation == generation && it != known.end() && it->second.actor == actor &&
        it->second.operation == state.operation;
}

bool CurrentGeneration(RE::FormID actorId, std::uint64_t stamp, std::uint64_t operation) {
    std::lock_guard lock(mutex);
    const auto it = known.find(actorId);
    return stamp == generation && it != known.end() && it->second.operation == operation;
}

// Read fresh instances after every engine action; never retain ExtraDataList
// pointers across calls. Ordinary inventory copies count as worn protection too.
bool Inventory(const State& state, RE::Actor* actor, RE::FormID armorId, const char* stage) {
    bool worn = false;
    auto* changes = actor->GetInventoryChanges(true);
    if (!changes || !changes->entryList) {
        logger::warn("Preview cleanup actor={:08X} op={} stage={} armor={:08X}: inventory unavailable; protected", state.actorId, state.operation, stage, armorId);
        return true;
    }
    for (auto* entry : *changes->entryList) {
        if (!entry || Id(entry->object) != armorId || !entry->extraLists) continue;
        for (auto* extra : *entry->extraLists) {
            if (!extra) continue;
            auto* marker = extra->GetByType<RE::ExtraOutfitItem>();
            const bool instanceWorn = extra->GetWorn();
            worn |= instanceWorn;
            logger::info("Preview cleanup inventory actor={:08X} op={} stage={} armor={:08X} extra={} marker={:08X} outgoingOwned={} worn={} delta={}",
                state.actorId, state.operation, stage, armorId, static_cast<const void*>(extra), marker ? marker->id : 0,
                marker && marker->id == state.outgoing, instanceWorn, entry->countDelta);
        }
    }
    return worn;
}

bool BipedReferences(const State& state, RE::Actor* actor, RE::NiAVObject* clone, const char* stage) {
    bool referenced = false;
    auto biped = actor->GetBiped(false);
    if (!biped) return true;
    for (bool buffered : {false, true}) {
        const auto* slots = buffered ? biped->bufferedObjects : biped->objects;
        for (std::uint32_t i = 0; i < RE::BIPED_OBJECTS::kTotal; ++i) {
            const auto& slot = slots[i];
            auto* current = slot.partClone.get();
            const bool usesClone = clone && current &&
                (current == clone || Below(clone, current) || Below(current, clone));
            referenced |= usesClone;
            if (usesClone || i == 4 || i == 16)
                logger::info("Preview cleanup slot actor={:08X} op={} stage={} buffered={} slot={} item={:08X} addon={:08X} clone={} capturedClone={}",
                    state.actorId, state.operation, stage, buffered, i + 30, Id(slot.item), Id(slot.addon),
                    static_cast<const void*>(slot.partClone.get()), usesClone);
        }
    }
    return referenced;
}

void Inspect(const Snapshot& state, RE::Actor* actor, const char* stage) {
    if (!state || !actor || actor->IsDead() || !Current(*state, actor)) return;
    const bool sameGraph = state->root && state->biped && actor->Get3D(false) == state->root.get() &&
        actor->GetBiped(false).get() == state->biped.get();
    logger::info("Preview cleanup actor={:08X} op={} stage={} outgoing={:08X} incoming={:08X} current={:08X} knownOwned={} applied={} sameGraph={} captured={}",
        state->actorId, state->operation, stage, state->outgoing, state->incoming,
        Id(actor->GetActorBase() ? actor->GetActorBase()->defaultOutfit : nullptr),
        state->owned, state->applied, sameGraph, state->parts.size());
    if (!sameGraph) return;
    BipedReferences(*state, actor, nullptr, stage);
    for (const auto id : state->outgoingItems) Inventory(*state, actor, id, stage);
    for (const auto id : state->incomingItems)
        if (!state->outgoingItems.contains(id)) Inventory(*state, actor, id, stage);
    for (auto& part : state->parts) {
        if (part.detached) continue;
        auto* armor = RE::TESForm::LookupByID<RE::TESObjectARMO>(part.armor);
        auto* clone = part.clone.get();
        const bool worn = Inventory(*state, actor, part.armor, stage);
        const bool bipedUses = BipedReferences(*state, actor, clone, stage);
        const bool attached = Below(state->root.get(), clone);
        // A changed item/addon can retain the old clone (observed in ops 6/11
        // of the 0.6.7 log). A reference alone therefore does not prove that
        // the outgoing armor still owns it. Protect related but nonidentical
        // references; only exact captured pointers may be cleared.
        bool relatedReference=false;
        for(auto* slots:{state->biped->objects,state->biped->bufferedObjects})
            for(unsigned i=0;i<RE::BIPED_OBJECTS::kTotal;++i){
                auto* other=slots[i].partClone.get();
                if(other && other!=clone && (Below(clone,other)||Below(other,clone)))relatedReference=true;
            }
        const bool eligible = state->owned && state->applied && !Excluded(actor, armor) &&
            !ExcludedNode(clone) && !worn && !relatedReference && attached;
        const char* decision = !state->owned ? "outfit-not-observed-as-Tailor-owned" :
            !state->applied ? "original-call-not-successful" : Excluded(actor, armor) ? Protection(actor, armor) :
            ExcludedNode(clone) ? "protected-node" : worn ? "worn-copy-remains" : relatedReference ? "related-replacement-reference" :
            !attached ? "captured-clone-no-longer-under-original-root" : "exact-orphan";
        logger::info("Preview cleanup clone actor={:08X} op={} stage={} armor={:08X} clone={} parent={} worn={} bipedUses={} attached={} eligible={}",
            state->actorId, state->operation, stage, part.armor, static_cast<const void*>(clone),
            static_cast<const void*>(clone ? clone->parent : nullptr), worn, bipedUses, attached, eligible);
        logger::info("Preview cleanup decision actor={:08X} op={} stage={} armor={:08X}: {}", state->actorId, state->operation, stage, part.armor, decision);
        if (!eligible) continue;
        // Match Tailor's own PreviewParts::Retire: preserve replacement clones
        // and all item/addon/skin metadata; retire only this captured pointer.
        clone->parent->DetachChild(clone);
        part.detached = !clone->parent;
        unsigned cleared=0;
        if(part.detached)for(auto* slots:{state->biped->objects,state->biped->bufferedObjects})
            for(unsigned i=0;i<RE::BIPED_OBJECTS::kTotal;++i)
                if(slots[i].partClone.get()==clone){slots[i].partClone.reset();++cleared;}
        logger::info("Preview cleanup retired orphan actor={:08X} op={} stage={} armor={:08X} clone={} detached={}",
            state->actorId, state->operation, stage, part.armor, static_cast<const void*>(clone), part.detached);
        logger::info("Preview cleanup exact stale clone references cleared={} actor={:08X} op={}",cleared,state->actorId,state->operation);
    }
}
}

Snapshot Capture(RE::Actor* actor, RE::BGSOutfit* incoming) {
    if (!actor || actor->IsPlayerRef() || !actor->GetActorBase()) return {};
    auto state = std::make_shared<State>();
    state->actor = actor->GetHandle(); state->actorId = actor->GetFormID();
    state->root.reset(actor->Get3D(false)); state->biped = actor->GetBiped(false);
    auto* outgoing = actor->GetActorBase()->defaultOutfit;
    state->outgoing = Id(outgoing); state->incoming = Id(incoming); state->incomingForm = incoming;
    if (outgoing) for (auto* item : outgoing->outfitItems) if (item && item->IsArmor()) state->outgoingItems.insert(item->GetFormID());
    if (incoming) for (auto* item : incoming->outfitItems) if (item && item->IsArmor()) state->incomingItems.insert(item->GetFormID());
    {
        std::lock_guard lock(mutex);
        auto& row = known[state->actorId];
        if (row.actor != actor || row.root != state->root.get()) row = KnownOutfits{actor, state->root.get(), {}, 0, {}};
        state->owned = outgoing && outgoing->IsDynamicForm() && row.outfits.contains(outgoing);
        state->generation = generation; state->operation = row.operation = ++nextOperation;
        row.active.reset(); // all strong engine references are released on the game thread
    }
    auto* changes = actor->GetInventoryChanges(true);
    if (!state->owned)
        logger::info("Preview cleanup capture declined actor={:08X} op={} outgoing={:08X}: outfit-not-observed-as-Tailor-owned", state->actorId, state->operation, state->outgoing);
    if (state->owned && state->root && state->biped && changes && changes->entryList) {
        for (auto* entry : *changes->entryList) {
            auto* armor = entry && entry->object ? entry->object->As<RE::TESObjectARMO>() : nullptr;
            if (!armor || !state->outgoingItems.contains(armor->GetFormID())) continue;
            const auto* protection = Protection(actor, armor);
            const char* skipped = protection ? protection : !entry->extraLists ? "no-extra-instances" :
                state->incomingItems.contains(armor->GetFormID()) ? "piece-retained-in-incoming-outfit" : nullptr;
            if (skipped) {
                logger::info("Preview cleanup capture declined actor={:08X} op={} armor={:08X}: {}", state->actorId, state->operation, armor->GetFormID(), skipped);
                continue;
            }
            const bool marked = std::any_of(entry->extraLists->begin(), entry->extraLists->end(), [&](RE::ExtraDataList* extra) {
                auto* marker = extra ? extra->GetByType<RE::ExtraOutfitItem>() : nullptr;
                return marker && state->outgoing && marker->id == state->outgoing;
            });
            if (!marked) {
                logger::info("Preview cleanup capture declined actor={:08X} op={} armor={:08X}: no-instance-matching-outgoing-marker-{:08X}", state->actorId, state->operation, armor->GetFormID(), state->outgoing);
                continue;
            }
            const auto previousCount = state->parts.size();
            for (const auto* slots : {state->biped->objects, state->biped->bufferedObjects}) {
                for (std::uint32_t i = 0; i < RE::BIPED_OBJECTS::kTotal; ++i) {
                    const auto& slot = slots[i]; auto* clone = slot.partClone.get();
                    if (slot.item != armor || !clone || ExcludedNode(clone) || !Below(state->root.get(), clone)) continue;
                    if (std::none_of(state->parts.begin(), state->parts.end(), [&](const auto& part) { return part.clone.get() == clone; }))
                        state->parts.push_back({armor->GetFormID(), slot.partClone, false});
                }
            }
            if (state->parts.size() == previousCount)
                logger::info("Preview cleanup capture declined actor={:08X} op={} armor={:08X}: no-safe-exact-biped-clone-under-original-root", state->actorId, state->operation, armor->GetFormID());
        }
    }
    Inspect(state, actor, "before");
    return state;
}

void Finish(const Snapshot& state, RE::Actor* actor, bool applied) {
    if (!state || !actor || actor->GetFormID() != state->actorId || !Current(*state, actor)) return;
    state->applied = applied;
    {
        std::lock_guard lock(mutex);
        auto it = known.find(state->actorId);
        if (state->generation == generation && it != known.end() && it->second.operation == state->operation) {
            it->second.active = state;
            if (applied && state->incomingForm && state->incomingForm->IsDynamicForm() &&
                actor->GetActorBase() && actor->GetActorBase()->defaultOutfit == state->incomingForm)
                it->second.outfits.insert(state->incomingForm);
        }
    }
    Inspect(state, actor, "after");
    const auto actorId = state->actorId;
    const auto stamp = state->generation, operation = state->operation;
    const std::weak_ptr<State> weak = state;
    // Worker threads hold only a weak control block and numeric tokens. Strong
    // graph/biped references live in known.active and game-thread callbacks.
    std::thread([weak, actorId, stamp, operation] {
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
        if (!CurrentGeneration(actorId, stamp, operation)) return;
        if (auto* tasks = SKSE::GetTaskInterface()) tasks->AddTask([weak] {
            if (auto snapshot = weak.lock()) { auto ref = snapshot->actor.get(); if (auto* actor = ref.get()) Inspect(snapshot, actor, "after-250ms"); }
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(750));
        if (!CurrentGeneration(actorId, stamp, operation)) return;
        if (auto* tasks = SKSE::GetTaskInterface()) tasks->AddTask([weak, actorId, stamp, operation] {
            if (auto snapshot = weak.lock()) { auto ref = snapshot->actor.get(); if (auto* actor = ref.get()) Inspect(snapshot, actor, "after-1000ms"); }
            std::lock_guard lock(mutex);
            auto it = known.find(actorId);
            if (stamp == generation && it != known.end() && it->second.operation == operation) it->second.active.reset();
        });
    }).detach();
}

void Clear() { std::lock_guard lock(mutex); ++generation; known.clear(); }
}
