#include "PCH.h"
#include "AutoFitting.h"
#include "AutoFittingFormat.h"
#include <format>
#include <map>
#include <set>
#include <unordered_map>

namespace AutoFitting {
namespace {
using Bytes=std::vector<std::uint8_t>;
// Duplicate readers keep scanning separate from the engine's original TESFile
// cursor. The engine reader handles compression and winning record overrides.
class Readers {
    std::map<RE::TESFile*,RE::TESFile*> files;
public:
    ~Readers(){for(auto [_,f]:files)if(f)f->CloseTES(true);}
    std::pair<RE::TESFile*,Bytes> Metadata(RE::TESObjectARMO* armor) {
        auto* owner=armor->GetFile(-1);
        if(!owner)throw std::runtime_error("armor has no winning plugin");
        auto [it,inserted]=files.try_emplace(owner,nullptr);
        if(inserted) {
            it->second=owner->Duplicate();
            if(!it->second || !it->second->OpenTES(RE::NiFile::OpenMode::kReadOnly,false))throw std::runtime_error("cannot open winning plugin metadata");
        }
        auto* file=it->second;
        if(!file || !file->SeekForm(armor))throw std::runtime_error("winning armor record unavailable");
        if(!file->SeekNextSubrecordType(0x44414d56))return {file,{}};
        auto size=file->GetCurrentSubRecordSize();
        if(size<6 || size>2*1024*1024)throw std::runtime_error("invalid VMAD length");
        Bytes bytes(size);
        if(!file->ReadData(bytes.data(),size))throw std::runtime_error("cannot read VMAD");
        // Resolve property references in the same winning file/master context.
        return {file,std::move(bytes)};
    }
};
class Scripts {
    std::map<std::string,Format::Parent> cache;
public:
    bool IsDD(std::string name) {
        name=Format::Lower(name);bool found=false;std::set<std::string> seen;
        while(name!="objectreference" && name!="form" && name!="quest" && name!="activemagiceffect" && !name.empty()) {
            if(!seen.insert(name).second || seen.size()>64)throw std::runtime_error("cyclic/excessive PEX ancestry");
            auto it=cache.find(name);
            if(it==cache.end()) {
                if(name.find_first_of("/\\:")!=std::string::npos)throw std::runtime_error("invalid PEX class name");
                RE::BSResourceNiBinaryStream stream("Scripts/"+name+".pex");
                if(!stream.good() || !stream.stream)throw std::runtime_error("winning PEX unavailable: "+name);
                auto size=stream.stream->totalSize;
                if(size<16 || size>16*1024*1024)throw std::runtime_error("invalid PEX length");
                Bytes bytes(size);if(!stream.read(bytes.data(),size))throw std::runtime_error("cannot read winning PEX");
                auto metadata=Format::PEX(bytes);
                if(metadata.name!=name)throw std::runtime_error("PEX class/file identity differs");
                it=cache.emplace(name,std::move(metadata)).first;
            }
            found|=name=="zadequipscript";
            name=it->second.parent;
        }
        return found;
    }
};
std::string Key(RE::TESObjectARMO* armor) {
    auto* owner=armor->GetFile(0);
    if(!owner)throw std::runtime_error("device has no source identity");
    return Format::Lower(std::string(owner->GetFilename()))+"|"+std::format("{:06x}",armor->GetLocalFormID());
}
class Slots {
    std::filesystem::path path;
    std::map<std::string,std::uint32_t> entries;
    bool dirty=false;
public:
    Slots() {
        auto directory=SKSE::log::log_directory();if(!directory)throw std::runtime_error("SKSE user directory unavailable");
        path=*directory/"TailorDDBridge.AutomaticSlots.json";
        if(!std::filesystem::exists(path)) {
            // Do not silently reuse slot IDs if the live ledger was removed but
            // the backup demonstrates that assignments have existed before.
            if(std::filesystem::exists(path.wstring()+L".bak"))throw std::runtime_error("automatic slot ledger missing; restore its .bak before using saved outfits");
            return;
        }
        std::ifstream file(path);nlohmann::json record;file>>record;
        if(record.at("schema")!=1 || record.at("plugin")!=Plugin)throw std::runtime_error("unsupported automatic slot ledger");
        entries=record.at("slots").get<decltype(entries)>();
        std::set<std::uint32_t> used;
        for(const auto& [key,id]:entries)if(key.empty() || id<Format::FirstSlot || id>=Format::EndSlot || !used.insert(id).second)throw std::runtime_error("invalid automatic slot ledger; no IDs were reassigned");
    }
    std::uint32_t Get(RE::TESObjectARMO* armor) {
        auto before=entries.size();auto id=Format::Allocate(entries,Key(armor));dirty|=before!=entries.size();return id;
    }
    void Commit() {
        if(!dirty)return;
        auto temporary=path.wstring()+L".tmp";
        {std::ofstream file(temporary,std::ios::binary|std::ios::trunc);
         file<<nlohmann::json{{"schema",1},{"plugin",Plugin},{"slots",entries}}.dump(2)<<'\n';
         file.flush();if(!file)throw std::runtime_error("cannot save automatic slot ledger");}
        if(std::filesystem::exists(path) && !CopyFileW(path.c_str(),(path.wstring()+L".bak").c_str(),FALSE))throw std::runtime_error("cannot back up automatic slot ledger");
        if(!MoveFileExW(temporary.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))throw std::runtime_error("cannot commit automatic slot ledger");
        logger::info("Automatic fitting slot ledger: {} ({} permanent identities)",path.string(),entries.size());
    }
};
// Copy appearance-bearing components, not TESForm/VMAD/keywords/enchantment or
// template ownership. CopyComponent uses the engine's model/alternate-texture
// copy operation rather than memcpy-ing arrays or sharing mutable ownership.
void Appearance(RE::TESObjectARMO* dst,RE::TESObjectARMO* src,RE::TESObjectARMO* inventory) {
    if(!dst || dst==src || dst==inventory || dst->numKeywords || dst->formEnchanting || dst->templateArmor)throw std::runtime_error("automatic fitting slot is not an empty safe armor");
    auto id=dst->GetFormID();auto* file=dst->GetFile(0);
    dst->boundData=src->boundData;
    dst->fullName=inventory->fullName;
    dst->race=src->race;dst->value=src->value;dst->weight=src->weight;
    static_cast<RE::TESBipedModelForm*>(dst)->CopyComponent(static_cast<RE::TESBipedModelForm*>(src));
    dst->equipSlot=src->equipSlot;dst->bipedModelData=src->bipedModelData;
    dst->blockBashImpactDataSet=src->blockBashImpactDataSet;dst->altBlockMaterialType=src->altBlockMaterialType;
    dst->armorRating=src->armorRating;dst->armorAddons=src->armorAddons;
    // Same shield flag selection as the existing offline generator.
    dst->formFlags=(dst->formFlags&~std::uint32_t(0x64))|(src->formFlags&0x40);
    if(dst->GetFormID()!=id || dst->GetFile(0)!=file)throw std::runtime_error("automatic fitting slot identity changed");
}
std::unordered_map<RE::FormID,RE::TESObjectARMO*> Legacy() {
    std::unordered_map<RE::FormID,RE::TESObjectARMO*> result;
    auto* data=RE::TESDataHandler::GetSingleton();
    if(!data->LookupLoadedModByName("TailorDDCosmetics.esp"))return result;
    std::ifstream file("Data/SKSE/Plugins/TailorDDSafeFitting.json");
    if(!file){logger::warn("Legacy cosmetic ESP is loaded without its mapping; original automatic slots remain available");return result;}
    nlohmann::json manifest;file>>manifest;
    if(manifest.at("schema")!=2 || manifest.at("targetPlugin")!="TailorDDCosmetics.esp")throw std::runtime_error("unsupported legacy fitting map");
    for(const auto& row:manifest.at("mappings")) {
        auto* inv=data->LookupForm<RE::TESObjectARMO>(row.at("sourceId").get<RE::FormID>(),row.at("sourcePlugin").get<std::string>());
        auto* cos=data->LookupForm<RE::TESObjectARMO>(row.at("targetId").get<RE::FormID>(),"TailorDDCosmetics.esp");
        if(inv && cos && cos!=inv && !cos->numKeywords && !cos->formEnchanting && !cos->templateArmor) {
            if(!result.emplace(inv->GetFormID(),cos).second)throw std::runtime_error("duplicate legacy device identity");
        }
    }
    return result;
}
}
Result Build() {
    auto* data=RE::TESDataHandler::GetSingleton();
    if(!data || !data->LookupLoadedModByName(Plugin))throw std::runtime_error("enable the bundled TailorDDAutoFitting.esp");
    Readers readers;Scripts scripts;Slots slots;auto legacy=Legacy();Result result;
    struct Candidate {RE::TESObjectARMO* inventory;RE::TESObjectARMO* rendered;};
    std::map<std::string,Candidate> candidates;
    std::size_t inspected=0;
    for(auto* armor:data->GetFormArray<RE::TESObjectARMO>()) {
        if(!armor || armor->IsDeleted())continue;
        auto* origin=armor->GetFile(0);
        if(!origin || origin->GetFilename()==Plugin || origin->GetFilename()=="TailorDDCosmetics.esp")continue;
        // An unnamed scripted ARMO is often a rendered/custom form. Leave it
        // alone; the final map still rejects DD-keyword items not proven safe.
        if(!armor->GetName() || !*armor->GetName())continue;
        ++inspected;
        bool deviceCandidate=false;
        try {
            auto [owner,bytes]=readers.Metadata(armor);
            if(bytes.empty())continue;
            auto metadata=Format::VMAD(bytes);
            for(const auto& script:metadata) {
                auto inventory=script.objects.find("deviceinventory"),render=script.objects.find("devicerendered");
                if(inventory==script.objects.end() || render==script.objects.end())continue;
                if(owner->GetRuntimeFormID(inventory->second)!=armor->GetFormID())continue;
                deviceCandidate=true;
                auto* rendered=RE::TESForm::LookupByID<RE::TESObjectARMO>(owner->GetRuntimeFormID(render->second));
                bool renderScripts=false;
                if(rendered){auto [unused,body]=readers.Metadata(rendered);renderScripts=!body.empty() && !Format::VMAD(body).empty();}
                if(!scripts.IsDD(script.name) || !rendered || rendered->IsDeleted() || renderScripts || rendered->templateArmor || rendered->armorAddons.empty()) {
                    result.excluded.push_back(armor->GetFormID());
                    if(rendered)result.excluded.push_back(rendered->GetFormID());
                    break;
                }
                if(!candidates.emplace(Key(armor),Candidate{armor,rendered}).second)throw std::runtime_error("ambiguous automatic device pair");
                break;
            }
        }catch(const std::exception& e) {
            if(deviceCandidate || armor->HasKeywordString("zad_InventoryDevice") || armor->HasKeywordString("zad_Lockable"))result.excluded.push_back(armor->GetFormID());
            logger::warn("Automatic fitting skipped {:08X} '{}': {}",armor->GetFormID(),armor->GetName(),e.what());
        }
    }
    for(const auto& [_,candidate]:candidates) {
        auto* inv=candidate.inventory;auto* ren=candidate.rendered;
        auto* cos=data->LookupForm<RE::TESObjectARMO>(slots.Get(inv),Plugin);
        if(!cos)throw std::runtime_error("automatic fitting slot missing from bundled plugin");
        {auto [unused,body]=readers.Metadata(cos);if(!body.empty() && !Format::VMAD(body).empty())throw std::runtime_error("automatic fitting slot has scripts");}
        Appearance(cos,ren,inv);
        auto* automatic=cos;
        // Preserve existing assigned cosmetic identities for upgrades. New
        // devices still receive automatic slots with no manual regeneration.
        if(auto old=legacy.find(inv->GetFormID());old!=legacy.end()) {
            {auto [unused,body]=readers.Metadata(old->second);if(!body.empty() && !Format::VMAD(body).empty())throw std::runtime_error("legacy fitting copy has scripts");}
            Appearance(old->second,ren,inv);cos=old->second;
        }
        result.rows.push_back({inv,ren,cos,automatic});
    }
    slots.Commit();
    logger::info("Automatic fitting: {} named armors inspected, {} supported DD pairs, {} excluded forms; no generator or game restart required",inspected,result.rows.size(),result.excluded.size());
    return result;
}
}
