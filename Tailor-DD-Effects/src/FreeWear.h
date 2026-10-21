#pragma once
#include <functional>
namespace FreeWear {
    struct Pair { RE::TESObjectARMO* inventory; RE::TESObjectARMO* rendered; };
    bool Install();
    void SetPairs(std::vector<Pair> pairs);
    bool RegisterPapyrus(RE::BSScript::IVirtualMachine* vm);
    bool Grant(RE::Actor* actor,const std::vector<RE::TESObjectARMO*>& devices,std::vector<RE::TESObjectARMO*>* newlyGranted=nullptr);
    std::size_t GrantSupported(RE::Actor* actor,const std::vector<RE::TESObjectARMO*>& devices,std::vector<RE::TESObjectARMO*>* newlyGranted=nullptr);
    bool IsGranted(RE::Actor* actor,RE::TESObjectARMO* device);
    void SetCosmeticVisibility(RE::Actor* actor,const std::vector<RE::TESObjectARMO*>& copies);
    void Revoke(RE::Actor* actor,const std::vector<RE::TESObjectARMO*>& devices);
    void Clear();
    std::vector<RE::FormID> ManagedActors();
    void Save(SKSE::SerializationInterface* stream);
    void Load(SKSE::SerializationInterface* stream,std::uint32_t version,std::uint32_t length);
    void AfterUnrestrained(RE::Actor* actor,std::function<void()> action);
    void AfterReleased(RE::Actor* actor,std::function<void()> action);
    void FinishPreview(const std::vector<RE::FormID>& confirmedActors);
    void StripOutfit(RE::Actor* actor,RE::BGSOutfit* outfit);
    void EquipManaged(RE::Actor* actor,const std::vector<RE::TESObjectARMO*>& devices);
    using RemoveFn = void* (*)(RE::Actor*,void*,RE::TESBoundObject*,std::int32_t,RE::ITEM_REMOVE_REASON,RE::ExtraDataList*,RE::TESObjectREFR*,const RE::NiPoint3*,const RE::NiPoint3*);
    bool DeferRemoval(RemoveFn original,RE::Actor* actor,RE::TESBoundObject* item,std::int32_t count,RE::ITEM_REMOVE_REASON reason,RE::ExtraDataList* extra,RE::TESObjectREFR* to,const RE::NiPoint3* drop,const RE::NiPoint3* rotate);
    inline constexpr std::uint32_t Record=0x46524545;
}
