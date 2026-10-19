#include "PCH.h"
#include "EquipmentBridge.h"
#include "EquipmentProfiles.h"
#include "TailorInterface1.h"
#include "DevicePolicy.h"
#include "FreeWear.h"
#include "AppearanceTrace.h"
#include "MuAppearance.h"
#include "AutoFitting.h"
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <intrin.h>

namespace Equipment {
std::atomic_bool activateOnExit{false};
namespace {
    // Const input layouts, checked against the source and each supported binary's
    // disassembly. Copies belong to this DLL; Tailor only reads them during the call.
    struct ArmorItem { RE::FormID id; std::string plugin, name; };
    struct Outfit { int id; std::string name; std::vector<ArmorItem> items; int sex; };
    static_assert(sizeof(std::string) == 32 && sizeof(ArmorItem) == 72);
    static_assert(offsetof(Outfit, items) == 0x28 && sizeof(Outfit) == 72);
    struct Pair { RE::TESObjectARMO* inv; RE::TESObjectARMO* ren; RE::TESObjectARMO* cos; ArmorItem original, copy; };
    struct Pending { RE::ActorHandle actor; Outfit outfit; bool actual; };
    std::vector<Pair> pairs;
    std::unordered_map<RE::FormID, std::size_t> inventories, renders, cosmetics;
    std::unordered_set<RE::FormID> excluded;
    std::unordered_map<RE::FormID, Outfit> lastApplied;
    std::unordered_map<RE::FormID, Pending> pending;
    std::unordered_set<RE::FormID> confirmedActors;
    struct Mode { int outfit; std::uint64_t pieces; bool free=false; };
    std::unordered_map<RE::FormID, Mode> actualModes;
    std::recursive_mutex stateMutex;
    std::uint64_t generation = 0;
    bool ready = false;
    std::uintptr_t tailorBase = 0, tailorEnd = 0;
    TailorAPI::ITailorInterface1* api = nullptr;
    bool (*isTailorOpen)() = nullptr;
    using ItemFn = void (*)(void*, RE::Actor*, const ArmorItem&);
    using ItemsFn = void (*)(void*, RE::Actor*, const std::vector<ArmorItem>&);
    using ApplyFn = bool (*)(void*, RE::Actor*, const Outfit&, bool);
    using PlayerFn = bool (*)(void*, RE::Actor*, const Outfit&);
    using ConfirmFn = bool (*)(void*, int);
    using CloseFn = void (*)(void*, std::uint8_t);
    using DefaultFn = bool (*)(void*, RE::Actor*, RE::BGSOutfit*, bool);
    ItemFn addItem = nullptr, removeItem = nullptr;
    ItemsFn loadItems = nullptr;
    ApplyFn applyOutfit = nullptr;
    PlayerFn applyPlayer = nullptr;
    ConfirmFn confirm = nullptr;
    CloseFn closeUI = nullptr;
    DefaultFn setDefault = nullptr;
    using ArmorVector = std::vector<RE::TESObjectARMO*>;
    using ResolveFn = ArmorVector* (*)(ArmorVector*, const std::vector<ArmorItem>&);
    ResolveFn resolvePlayer = nullptr;
    thread_local bool resolvingPlayer = false, playerActual = false;
    thread_local RE::Actor* playerActor = nullptr;
    void* manager = nullptr;
    // MSVC's member-function ABI puts this in RCX and ObjectRefHandle's return
    // storage in RDX. Verified at Tailor's actual virtual RemoveItem call sites.
    using RemoveFn = void* (*)(RE::Actor*, void*, RE::TESBoundObject*, std::int32_t,
        RE::ITEM_REMOVE_REASON, RE::ExtraDataList*, RE::TESObjectREFR*, const RE::NiPoint3*, const RE::NiPoint3*);
    RemoveFn previousRemove = nullptr;
    static_assert(sizeof(RE::ObjectRefHandle) == 4);

    bool IsOpen() {
        return !isTailorOpen || isTailorOpen();
    }
    bool Worn(RE::Actor* actor, RE::TESObjectARMO* armor) {
        auto* inventory = actor ? actor->GetInventoryChanges(true) : nullptr;
        if (!inventory || !inventory->entryList) return false;
        for (auto* entry : *inventory->entryList) {
            if (!entry || entry->object != armor || !entry->extraLists) continue;
            for (auto* extra : *entry->extraLists) if (extra && extra->GetWorn()) return true;
        }
        return false;
    }
    bool Protected(RE::TESBoundObject* object) {
        auto* armor = object ? object->As<RE::TESObjectARMO>() : nullptr;
        if (!armor) return false;
        return inventories.contains(armor->GetFormID()) || renders.contains(armor->GetFormID()) ||
            armor->HasKeywordString("zad_InventoryDevice") || armor->HasKeywordString("zad_Lockable");
    }
    void Preserve(RE::Actor* actor) {
        auto* inventory = actor ? actor->GetInventoryChanges(true) : nullptr;
        if (!inventory || !inventory->entryList) return;
        for (auto* entry : *inventory->entryList) {
            if (!entry || !Protected(entry->object) || !entry->extraLists) continue;
            for (auto* extra : *entry->extraLists) if (extra) extra->RemoveByType(RE::ExtraDataType::kOutfitItem);
        }
    }
    void* Remove(RE::Actor* actor, void* result, RE::TESBoundObject* object, std::int32_t count,
        RE::ITEM_REMOVE_REASON reason, RE::ExtraDataList* extra, RE::TESObjectREFR* to,
        const RE::NiPoint3* drop, const RE::NiPoint3* rotate) {
        if (FreeWear::DeferRemoval(previousRemove, actor, object, count, reason, extra, to, drop, rotate)) {
            std::memset(result, 0, sizeof(RE::ObjectRefHandle)); return result;
        }
        const auto caller = reinterpret_cast<std::uintptr_t>(_ReturnAddress());
        if (caller >= tailorBase && caller < tailorEnd && Protected(object)) {
            std::memset(result, 0, sizeof(RE::ObjectRefHandle));
            logger::info("Preserved genuine DD inventory {:08X} from Tailor copy cleanup", object->GetFormID());
            return result;
        }
        return previousRemove(actor, result, object, count, reason, extra, to, drop, rotate);
    }
    std::uint64_t Fingerprint(const std::vector<ArmorItem>& items) {
        std::uint64_t value = 14695981039346656037ULL;
        for (const auto& item : items) {
            const ArmorItem* source = &item;
            auto* armor = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(item.id, item.plugin);
            if (armor) if (auto it = cosmetics.find(armor->GetFormID()); it != cosmetics.end()) source = &pairs[it->second].original;
            for (unsigned char c : source->plugin) { value ^= static_cast<unsigned char>(std::tolower(c)); value *= 1099511628211ULL; }
            value ^= 0; value *= 1099511628211ULL;
            for (int i = 0; i < 4; ++i) { value ^= (source->id >> (i * 8)) & 0xff; value *= 1099511628211ULL; }
        }
        return value;
    }
    RE::TESObjectARMO* MapArmor(RE::TESObjectARMO* armor, RE::Actor* actor, bool actualAllowed) {
        if (!armor) return nullptr;
        auto id = armor->GetFormID();
        if (excluded.contains(id)) return nullptr;
        std::optional<std::size_t> index;
        bool inventoryOrCopy = true;
        if (auto it = inventories.find(id); it != inventories.end()) index = it->second;
        else if (auto it = cosmetics.find(id); it != cosmetics.end()) index = it->second;
        else if (auto it = renders.find(id); it != renders.end()) { index = it->second; inventoryOrCopy = false; }
        if (!index) {
            if (armor->HasKeywordString("zad_InventoryDevice") || armor->HasKeywordString("zad_Lockable")) {
                logger::warn("Refused unreviewed DD item {:08X} in Tailor equipment path", id);
                return nullptr;
            }
            return armor;
        }
        auto& pair = pairs[*index];
        if (inventoryOrCopy && actualAllowed && !IsOpen()) {
            if (Worn(actor, pair.ren)) return nullptr; // DD already owns this device.
            return pair.inv;
        }
        return pair.cos;
    }
    std::optional<ArmorItem> Map(const ArmorItem& item, RE::Actor* actor, bool actualAllowed) {
        auto* armor = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(item.id, item.plugin);
        if (!armor) return item;
        auto* mapped = MapArmor(armor, actor, actualAllowed);
        if (!mapped) return std::nullopt;
        if (mapped == armor) return item;
        if (auto it = cosmetics.find(mapped->GetFormID()); it != cosmetics.end()) return pairs[it->second].copy;
        if (auto it = inventories.find(mapped->GetFormID()); it != inventories.end()) return pairs[it->second].original;
        return std::nullopt;
    }
    bool AllowReal(const std::vector<ArmorItem>& items, RE::Actor* actor, int outfit) {
        bool actualAllowed = false;
        {
            std::lock_guard lock(stateMutex);
            auto it = actor ? actualModes.find(actor->GetFormID()) : actualModes.end();
            if (it != actualModes.end()) actualAllowed = Policy::UseReal(true, IsOpen(), outfit, it->second.outfit, Fingerprint(items), it->second.pieces);
        }
        return actualAllowed;
    }
    std::vector<ArmorItem> MapAll(const std::vector<ArmorItem>& items, RE::Actor* actor, int outfit) {
        std::vector<ArmorItem> mapped;
        mapped.reserve(items.size());
        const bool actualAllowed = AllowReal(items, actor, outfit);
        for (const auto& item : items) if (auto result = Map(item, actor, actualAllowed)) mapped.push_back(std::move(*result));
        return mapped;
    }
    void Add(void* self, RE::Actor* actor, const ArmorItem& item) {
        if (!actor) return;
        auto handle=actor->GetHandle();auto stamp=generation;
        FreeWear::AfterUnrestrained(actor,[self,handle,item,stamp]{auto ref=handle.get();auto* a=ref.get();if(a&&stamp==generation&&IsOpen())if(auto mapped=Map(item,a,0))addItem(self,a,*mapped);});
    }
    void RemovePreview(void* self, RE::Actor* actor, const ArmorItem& item) {
        if (auto mapped = Map(item, actor, 0)) removeItem(self, actor, *mapped);
    }
    void Load(void* self, RE::Actor* actor, const std::vector<ArmorItem>& items) {
        if (!actor) return;
        auto handle=actor->GetHandle();auto stamp=generation;
        FreeWear::AfterUnrestrained(actor,[self,handle,items,stamp]{auto ref=handle.get();auto* a=ref.get();if(!a||stamp!=generation||!IsOpen())return;auto mapped=MapAll(items,a,0);if(!items.empty()&&mapped.empty())return;loadItems(self,a,mapped);});
    }
    void Remember(RE::Actor* actor, const Outfit& outfit) {
        if (!actor) return;
        std::lock_guard lock(stateMutex);
        // Do not overwrite an original snapshot with the mapped copy passed from
        // ApplyCustomOutfit to ApplyPlayerOutfit.
        auto found = lastApplied.find(actor->GetFormID());
        if (found == lastApplied.end() || found->second.id != outfit.id ||
            std::none_of(outfit.items.begin(), outfit.items.end(), [](const ArmorItem& item) { return item.plugin == "TailorDDCosmetics.esp" || item.plugin == AutoFitting::Plugin; }))
            lastApplied[actor->GetFormID()] = outfit;
    }
    bool Apply(void* self, RE::Actor* actor, const Outfit& outfit, bool automatic) {
        Remember(actor, outfit);
        if (actor && actor->IsPlayerRef()) return applyOutfit(self, actor, outfit, automatic);
        if (actor && IsOpen()) {
            auto handle=actor->GetHandle();auto stamp=generation;
            FreeWear::AfterUnrestrained(actor,[self,handle,outfit,automatic,stamp]{auto ref=handle.get();auto* a=ref.get();if(!a||stamp!=generation||!IsOpen())return;auto mapped=outfit;mapped.items=MapAll(outfit.items,a,outfit.id);if(!outfit.items.empty()&&mapped.items.empty())return;applyOutfit(self,a,mapped,automatic);});
            return true;
        }
        auto mapped = outfit; mapped.items = MapAll(outfit.items, actor, outfit.id);
        if (!outfit.items.empty() && mapped.items.empty()) return false;
        const bool applied=applyOutfit(self,actor,mapped,automatic);
        bool free=false;
        {std::lock_guard lock(stateMutex);auto it=actor?actualModes.find(actor->GetFormID()):actualModes.end();free=it!=actualModes.end() && it->second.free;}
        if(applied && free && AllowReal(outfit.items,actor,outfit.id)){
            std::vector<RE::TESObjectARMO*> devices;
            for(const auto& item:outfit.items){auto* armor=RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(item.id,item.plugin);if(armor){if(auto it=cosmetics.find(armor->GetFormID());it!=cosmetics.end())armor=pairs[it->second].inv;devices.push_back(armor);}}
            FreeWear::EquipManaged(actor,devices);
        }
        return applied;
    }
    bool ApplyPlayer(void* self, RE::Actor* actor, const Outfit& outfit) {
        Remember(actor, outfit);
        if(actor && IsOpen()){
            auto handle=actor->GetHandle();auto stamp=generation;
            FreeWear::AfterUnrestrained(actor,[self,handle,outfit,stamp]{
                auto ref=handle.get();auto* a=ref.get();if(!a||stamp!=generation||!IsOpen())return;
                struct Restore {bool resolving=resolvingPlayer,actual=playerActual;RE::Actor* actor=playerActor;~Restore(){resolvingPlayer=resolving;playerActual=actual;playerActor=actor;}} restore;
                resolvingPlayer=true;playerActor=a;playerActual=false;applyPlayer(self,a,outfit);
            });
            return true;
        }
        struct Scope {
            bool resolving = resolvingPlayer, actual = playerActual;
            RE::Actor* actor = playerActor;
            ~Scope() { resolvingPlayer = resolving; playerActual = actual; playerActor = actor; }
        } scope;
        resolvingPlayer = true; playerActor = actor; playerActual = AllowReal(outfit.items, actor, outfit.id);
        // Keep the original CustomOutfit intact. Tailor records its original piece
        // identities for save/reconcile; only the temporary resolved armor vector changes.
        return applyPlayer(self, actor, outfit);
    }
    ArmorVector* ResolvePlayer(ArmorVector* output, const std::vector<ArmorItem>& items) {
        auto* result = resolvePlayer(output, items);
        if (resolvingPlayer && result) {
            auto write = result->begin();
            for (auto* armor : *result) if (auto* mapped = MapArmor(armor, playerActor, playerActual)) *write++ = mapped;
            result->erase(write, result->end()); // pointer elements only; never allocates across DLLs
        }
        return result;
    }
    bool Default(void* self, RE::Actor* actor, RE::BGSOutfit* outfit, bool update) {
        MuAppearance::Before(actor);
        AppearanceTrace::Snapshot(actor,"before Tailor default outfit");
        bool free=false;
        {std::lock_guard lock(stateMutex);auto it=actor?actualModes.find(actor->GetFormID()):actualModes.end();free=it!=actualModes.end() && it->second.free;}
        if(free)FreeWear::StripOutfit(actor,outfit);
        Preserve(actor);auto result=setDefault(self,actor,outfit,update);AppearanceTrace::Snapshot(actor,"after Tailor default outfit");MuAppearance::After(actor);return result;
    }
    bool Confirm(void* self, int situation) {
        manager = self;
        auto* bytes = static_cast<std::byte*>(self);
        // These offsets are the same in both disassembled 3.0.1 builds.
        const bool cycling = *reinterpret_cast<const bool*>(bytes + 0x1c0);
        auto handle = *reinterpret_cast<const RE::ActorHandle*>(bytes);
        auto ref = handle.get(); auto* actor = ref.get();
        int selected = 0;
        if (cycling && actor && *reinterpret_cast<const int*>(bytes + 0x1b8) == situation) {
            const auto& ids = *reinterpret_cast<const std::vector<int>*>(bytes + 0x1a0);
            const int index = *reinterpret_cast<const int*>(bytes + 0x19c);
            if (index >= 0 && static_cast<std::size_t>(index) < ids.size()) selected = ids[index];
        }
        std::optional<Outfit> snapshot;
        {
            std::lock_guard lock(stateMutex);
            auto it = actor ? lastApplied.find(actor->GetFormID()) : lastApplied.end();
            if (it != lastApplied.end() && it->second.id == selected) snapshot = it->second;
        }
        const bool ok = confirm(self, situation);
        if(ok && actor && IsOpen())confirmedActors.insert(actor->GetFormID());
        if (ok && snapshot && IsOpen()) {
            std::lock_guard lock(stateMutex);
            actualModes.erase(actor->GetFormID());
            const bool hasDevice = std::any_of(snapshot->items.begin(), snapshot->items.end(), [](const ArmorItem& item) {
                auto* form = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(item.id, item.plugin);
                return form && (inventories.contains(form->GetFormID()) || cosmetics.contains(form->GetFormID()));
            });
            if (hasDevice) {
                pending[actor->GetFormID()] = {handle, std::move(*snapshot), activateOnExit.load()};
                logger::info("Confirmed {:08X} outfit {}, real DD on exit={}", actor->GetFormID(), selected, activateOnExit.load());
            } else pending.erase(actor->GetFormID());
        }
        return ok;
    }
    void Name(const char* text, void* context) { *static_cast<std::string*>(context) = text ? text : ""; }
    void Close(void* self, std::uint8_t reason) {
        const bool wasOpen = IsOpen();
        closeUI(self, reason);
        std::vector<RE::FormID> confirmed(confirmedActors.begin(),confirmedActors.end());confirmedActors.clear();
        // Focus loss/target switch must also restore temporarily removed devices.
        // Loading, new game and shutdown discard the old world instead.
        if(wasOpen && reason!=4 && reason!=5 && reason!=6)
            FreeWear::FinishPreview(reason==0?confirmed:std::vector<RE::FormID>{});
        std::unordered_map<RE::FormID, Pending> requests;
        std::uint64_t stamp;
        {
            std::lock_guard lock(stateMutex);
            stamp = ++generation;
            requests = std::move(pending); pending.clear(); lastApplied.clear();
        }
        if (!wasOpen || reason != 0) return;
        for (const auto& [id, request] : requests) {
            SKSE::GetTaskInterface()->AddTask([request, stamp] {
                {
                    std::lock_guard lock(stateMutex);
                    if (!Policy::Handoff(true, true, IsOpen(), true, stamp, generation)) return;
                }
                auto ref = request.actor.get(); auto* actor = ref.get();
                if (!actor || actor->IsDead() || !actor->Is3DLoaded() || !manager || !api) return;
                FreeWear::AfterReleased(actor,[request,stamp]{
                auto ref=request.actor.get();auto* actor=ref.get();if(!actor||stamp!=generation||IsOpen())return;
                std::string active;
                if (!api->GetOutfit(actor, Name, &active)) return;
                if (active != request.outfit.name) {
                    logger::info("No DD handoff for {:08X}: confirmed outfit isn't the currently worn Tailor outfit", actor->GetFormID());
                    return;
                }
                const bool actual = request.actual && activateOnExit.load();
                std::vector<RE::TESObjectARMO*> devices;
                for(const auto& item:request.outfit.items){auto* armor=RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESObjectARMO>(item.id,item.plugin);if(armor && inventories.contains(armor->GetFormID()))devices.push_back(armor);else if(armor)if(auto found=cosmetics.find(armor->GetFormID());found!=cosmetics.end())devices.push_back(pairs[found->second].inv);}
                std::vector<RE::TESObjectARMO*> newlyGranted;
                if(!actual && !FreeWear::Grant(actor,devices,&newlyGranted)){logger::warn("Free wear: confirmation could not grant scoped permission; quest/custom or pre-existing locked device was left alone");return;}
                {
                    std::lock_guard lock(stateMutex);
                    actualModes[actor->GetFormID()] = {request.outfit.id, Fingerprint(request.outfit.items), !actual};
                }
                // A handoff is an explicit re-dress, only after Set + normal exit.
                // Original DD inventory scripts retain responsibility for actual locking.
                const bool applied = actor->IsPlayerRef() ? ApplyPlayer(manager, actor, request.outfit) : Apply(manager, actor, request.outfit, false);
                if (!applied) { FreeWear::Revoke(actor,newlyGranted);std::lock_guard lock(stateMutex);actualModes.erase(actor->GetFormID()); }
                else if(actual)FreeWear::Revoke(actor,devices);
                logger::info("Exit handoff {:08X}: real={}, Tailor accepted={}; DD script completion is asynchronous", actor->GetFormID(), actual, applied);
                });
            });
        }
    }
    void ReadLegacyPairs() {
        std::ifstream file("Data/SKSE/Plugins/TailorDDSafeFitting.json");
        nlohmann::json manifest; file >> manifest;
        if (manifest.at("schema") != 2 || manifest.at("targetPlugin") != "TailorDDCosmetics.esp") throw std::runtime_error("unsupported manifest; regenerate data with the 0.4 tool");
        auto digest = [](const std::filesystem::path& path) {
            std::ifstream input(path, std::ios::binary);
            if (!input) throw std::runtime_error("evidence input is unavailable: " + path.string());
            BCRYPT_HASH_HANDLE hash = nullptr;
            if (BCryptCreateHash(BCRYPT_SHA256_ALG_HANDLE, &hash, nullptr, 0, nullptr, 0, 0) < 0) throw std::runtime_error("SHA256 initialization failed");
            std::array<unsigned char, 64 * 1024> buffer{};
            bool ok = true;
            while (input) {
                input.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
                if (input.gcount() && BCryptHashData(hash, buffer.data(), static_cast<ULONG>(input.gcount()), 0) < 0) { ok = false; break; }
            }
            std::array<unsigned char, 32> result{};
            if (input.bad() || !ok || BCryptFinishHash(hash, result.data(), static_cast<ULONG>(result.size()), 0) < 0) { BCryptDestroyHash(hash); throw std::runtime_error("SHA256 input read failed"); }
            BCryptDestroyHash(hash);
            constexpr char hex[] = "0123456789abcdef";
            std::string output; for (auto byte : result) { output += hex[byte >> 4]; output += hex[byte & 15]; }
            return output;
        };
        if (digest("Data/TailorDDCosmetics.esp") != manifest.at("targetSha256").get<std::string>()) throw std::runtime_error("generated plugin differs from its manifest");
        for (const auto* key : {"pluginProofs", "scriptProofs"}) {
            for (const auto& proof : manifest.at(key)) {
                auto name = std::filesystem::u8path(proof.at("file").get<std::string>());
                if (name.empty() || name.filename() != name) throw std::runtime_error("invalid evidence input file name");
                const auto path = std::filesystem::path(std::string_view(key) == "scriptProofs" ? "Data/Scripts" : "Data") / name;
                if (digest(path) != proof.at("sha256").get<std::string>()) throw std::runtime_error("enabled source files changed; regenerate cosmetic data: " + name.string());
            }
        }
        auto* data = RE::TESDataHandler::GetSingleton();
        for (const auto& row : manifest.at("mappings")) {
            auto sourcePlugin = row.at("sourcePlugin").get<std::string>();
            auto* inv = data->LookupForm<RE::TESObjectARMO>(row.at("sourceId").get<RE::FormID>(), sourcePlugin);
            auto* ren = data->LookupForm<RE::TESObjectARMO>(row.at("renderId").get<RE::FormID>(), row.at("renderPlugin").get<std::string>());
            auto* cos = data->LookupForm<RE::TESObjectARMO>(row.at("targetId").get<RE::FormID>(), "TailorDDCosmetics.esp");
            if (!inv || !ren || !cos || cos == inv || cos == ren || cos->formEnchanting || cos->templateArmor || cos->numKeywords)
                throw std::runtime_error("mapping forms missing or cosmetic record changed; regenerate data");
            auto index = pairs.size();
            pairs.push_back({inv, ren, cos, {row.at("sourceId"), sourcePlugin, row.at("name")}, {row.at("targetId"), "TailorDDCosmetics.esp", row.at("name")}});
            inventories.emplace(inv->GetFormID(), index); renders.try_emplace(ren->GetFormID(), index); cosmetics.emplace(cos->GetFormID(), index);
        }
        for (const auto& row : manifest.value("excluded", nlohmann::json::array())) {
            if (auto* inv = data->LookupForm<RE::TESObjectARMO>(row.at("sourceId").get<RE::FormID>(), row.at("sourcePlugin").get<std::string>())) excluded.insert(inv->GetFormID());
            if (auto* ren = data->LookupForm<RE::TESObjectARMO>(row.at("renderId").get<RE::FormID>(), row.at("renderPlugin").get<std::string>())) excluded.insert(ren->GetFormID());
        }
        if (pairs.empty()) throw std::runtime_error("no supported device pairs");
    }
    void ReadPairs() {
        auto* data=RE::TESDataHandler::GetSingleton();
        if(!data->LookupLoadedModByName(AutoFitting::Plugin)) {
            // Existing installations can retain their tested generated map.
            // The new FOMOD always supplies the automatic fitting plugin.
            ReadLegacyPairs();return;
        }
        auto found=AutoFitting::Build();
        for(const auto& row:found.rows) {
            auto* source=row.inventory->GetFile(0);auto* cosmetic=row.cosmetic->GetFile(0);
            if(!source || !cosmetic)throw std::runtime_error("automatic pair identity unavailable");
            auto index=pairs.size();
            pairs.push_back({row.inventory,row.rendered,row.cosmetic,
                {row.inventory->GetLocalFormID(),std::string(source->GetFilename()),row.inventory->GetName()},
                {row.cosmetic->GetLocalFormID(),std::string(cosmetic->GetFilename()),row.inventory->GetName()}});
            inventories.emplace(row.inventory->GetFormID(),index);
            renders.try_emplace(row.rendered->GetFormID(),index);
            cosmetics.emplace(row.cosmetic->GetFormID(),index);
            cosmetics.emplace(row.automatic->GetFormID(),index);
        }
        for(auto id:found.excluded)excluded.insert(id);
        if(pairs.empty())throw std::runtime_error("no supported DD pairs discovered automatically; see scan diagnostics");
    }
    void Save(SKSE::SerializationInterface* stream) {
        FreeWear::Save(stream);
        std::lock_guard lock(stateMutex);
        if (!stream->OpenRecord(0x4D4F4445, 3)) return;
        auto count = static_cast<std::uint32_t>(actualModes.size()); stream->WriteRecordData(count);
        for (auto [actor, mode] : actualModes) { stream->WriteRecordData(actor); stream->WriteRecordData(mode.outfit); stream->WriteRecordData(mode.pieces); stream->WriteRecordData(mode.free); }
    }
    void LoadSave(SKSE::SerializationInterface* stream) {
        Clear(); std::uint32_t type, version, length;
        while (stream->GetNextRecordInfo(type, version, length)) {
            if(type==FreeWear::Record){FreeWear::Load(stream,version,length);continue;}
            if (type != 0x4D4F4445 || (version != 2 && version != 3) || length < 4) continue;
            std::uint32_t count = 0;
            if (!stream->ReadRecordData(count) || count > 10000 || length != 4 + count * (version==3?17:16)) continue;
            for (unsigned i = 0; i < count; ++i) {
                RE::FormID oldID, newID; Mode mode;
                if (!stream->ReadRecordData(oldID) || !stream->ReadRecordData(mode.outfit) || !stream->ReadRecordData(mode.pieces)) break;
                if(version==3 && !stream->ReadRecordData(mode.free))break;
                if (mode.outfit > 0 && stream->ResolveFormID(oldID, newID)) actualModes[newID] = mode;
            }
        }
    }
}
bool Ready() { return ready; }
std::size_t PairCount() { return pairs.size(); }
void Clear() { std::lock_guard lock(stateMutex); ++generation; actualModes.clear(); pending.clear(); lastApplied.clear(); confirmedActors.clear(); FreeWear::Clear(); MuAppearance::Clear(); }
void RegisterSerialization() {
    auto* stream = SKSE::GetSerializationInterface(); stream->SetUniqueID(0x54444442);
    stream->SetSaveCallback(Save); stream->SetLoadCallback(LoadSave);
    stream->SetRevertCallback([](SKSE::SerializationInterface*) { Clear(); });
}
bool Install(std::uintptr_t base, std::size_t profile) {
    try {
        ReadPairs();
        {std::vector<FreeWear::Pair> rows;for(const auto& p:pairs)rows.push_back({p.inv,p.ren});FreeWear::SetPairs(std::move(rows));}
        const auto request = reinterpret_cast<TailorAPI::ITailorInterface1* (*)(std::uint32_t)>(GetProcAddress(reinterpret_cast<HMODULE>(base), "RequestTailorInterface"));
        api = request ? request(1) : nullptr;
        isTailorOpen = reinterpret_cast<bool (*)()>(GetProcAddress(reinterpret_cast<HMODULE>(base), "IsTailorOpen"));
        if (!api || !isTailorOpen || api->GetApiVersion() != 1.0f) throw std::runtime_error("Tailor API 1.0 unavailable");
        auto& p = equipmentProfiles[profile];
        const std::array<void*, 9> hooks{reinterpret_cast<void*>(&Add), reinterpret_cast<void*>(&RemovePreview), reinterpret_cast<void*>(&Load), reinterpret_cast<void*>(&Apply), reinterpret_cast<void*>(&ApplyPlayer), reinterpret_cast<void*>(&Confirm), reinterpret_cast<void*>(&Close), reinterpret_cast<void*>(&Default), reinterpret_cast<void*>(&ResolvePlayer)};
        const std::array<void**, 9> originals{reinterpret_cast<void**>(&addItem), reinterpret_cast<void**>(&removeItem), reinterpret_cast<void**>(&loadItems), reinterpret_cast<void**>(&applyOutfit), reinterpret_cast<void**>(&applyPlayer), reinterpret_cast<void**>(&confirm), reinterpret_cast<void**>(&closeUI), reinterpret_cast<void**>(&setDefault), reinterpret_cast<void**>(&resolvePlayer)};
        for (const auto& point : p.points) {
            if (std::memcmp(reinterpret_cast<void*>(base + point.rva), point.prologue.data(), point.prologue.size())) throw std::runtime_error("equipment entry point changed by another plugin");
        }
        std::vector<void*> installed;
        for (std::size_t i = 0; i < hooks.size(); ++i) {
            auto* address = reinterpret_cast<void*>(base + p.points[i].rva);
            auto result = MH_CreateHook(address, hooks[i], originals[i]);
            if (result != MH_OK) {
                for (auto* previous : installed) MH_RemoveHook(previous);
                throw std::runtime_error(MH_StatusToString(result));
            }
            installed.push_back(address);
        }
        // Enable all equipment hooks as a transaction. UI can still explain a failure.
        for (auto* address : installed) MH_QueueEnableHook(address);
        auto result = MH_ApplyQueued();
        if (result != MH_OK) {
            for (auto* address : installed) { MH_DisableHook(address); MH_RemoveHook(address); }
            throw std::runtime_error(MH_StatusToString(result));
        }
        tailorBase = base; tailorEnd = base + p.imageSize;
        REL::Relocation<std::uintptr_t> playerVTable{RE::VTABLE_PlayerCharacter[0]};
        previousRemove = reinterpret_cast<RemoveFn>(playerVTable.write_vfunc(0x56, &Remove));
        if(!FreeWear::Install()){
            playerVTable.write_vfunc(0x56,previousRemove);
            for(auto* address:installed){MH_DisableHook(address);MH_RemoveHook(address);}
            logger::error("Effect-preserving free wear unavailable; equipment hooks rolled back");return false;
        }
        ready = true;
        MuAppearance::Init();
        logger::info("Equipment bridge enabled: {} pairs; original Tailor DLL and translation files unchanged", pairs.size());
        return true;
    } catch (const std::exception& e) {
        logger::error("Equipment bridge disabled: {}", e.what()); return false;
    }
}
}
