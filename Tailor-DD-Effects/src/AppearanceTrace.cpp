#include "PCH.h"
#include "AppearanceTrace.h"
namespace AppearanceTrace {
namespace {
std::atomic_uint snapshots{0},equipmentSnapshots{0};
std::atomic_uint64_t generation{0};
constexpr unsigned snapshotLimit=200;
const char* Texture(const RE::NiPointer<RE::NiSourceTexture>& texture){return texture?texture->name.c_str():"";}
void Biped(RE::Actor* actor,RE::BipedAnim* biped,const char* stage,bool buffered){
    if(!biped)return;
    auto* objects=buffered?biped->bufferedObjects:biped->objects;
    for(unsigned slot=0;slot<RE::BIPED_OBJECTS::kTotal;++slot){
        const auto& object=objects[slot];if(!object.item && !object.addon && !object.partClone)continue;
        logger::info("Equipment biped {} actor={:08X} buffered={} slot={} item={:08X} addon={:08X} clone={} cloneName='{}' skinned={} skinTexture={:08X}",stage,actor->GetFormID(),buffered,slot,object.item?object.item->GetFormID():0,object.addon?object.addon->GetFormID():0,static_cast<const void*>(object.partClone.get()),object.partClone?object.partClone->name.c_str():"",object.skinned,object.skinTexture?object.skinTexture->GetFormID():0);
        auto* armor=object.item?object.item->As<RE::TESObjectARMO>():nullptr;
        auto* file=armor?armor->GetFile(0):nullptr;
        if(buffered || !armor || !file || (file->GetFilename()!="TailorDDAutoFitting.esp" && file->GetFilename()!="TailorDDCosmetics.esp"))continue;
        auto* clone=object.partClone.get();bool attached=false,parentCull=false;unsigned depth=0;
        for(auto* node=clone;node && depth++<64;node=node->parent){parentCull|=node->GetAppCulled();if(node==actor->Get3D()){attached=true;break;}}
        auto* addon=object.addon;const auto sex=actor->GetActorBase()?static_cast<unsigned>(actor->GetActorBase()->GetSex()):0;
        logger::info("Cosmetic model {} actor={:08X} armor={:08X} slot={} mask={:08X} addonMask={:08X} priority={} attached={} culledToActor={} localScale={} worldScale={} alwaysDraw={}",stage,actor->GetFormID(),armor->GetFormID(),slot+30,armor->GetSlotMask().underlying(),addon?addon->GetSlotMask().underlying():0,addon?addon->data.priorities[sex<2?sex:0]:0,attached,parentCull,clone?clone->local.scale:0,clone?clone->world.scale:0,clone && clone->GetFlags().any(RE::NiAVObject::Flag::kAlwaysDraw));
        unsigned shapes=0;
        if(clone)RE::BSVisit::TraverseScenegraphGeometries(clone,[&](RE::BSGeometry* geometry){
            if(shapes++>=8)return RE::BSVisit::BSVisitControl::kStop;
            auto* shader=geometry->lightingShaderProp_cast();auto* raw=shader?shader->material:nullptr;
            auto* material=raw && raw->GetType()==RE::BSShaderMaterial::Type::kLighting?static_cast<RE::BSLightingShaderMaterialBase*>(raw):nullptr;
            logger::info("Cosmetic shape {} actor={:08X} armor={:08X} name='{}' culled={} notVisible={} alwaysDraw={} scale={} alpha={} shader={} diffuse='{}'",stage,actor->GetFormID(),armor->GetFormID(),geometry->name.c_str(),geometry->GetAppCulled(),geometry->GetFlags().any(RE::NiAVObject::Flag::kNotVisible),geometry->GetFlags().any(RE::NiAVObject::Flag::kAlwaysDraw),geometry->world.scale,material?material->materialAlpha:-1,raw?static_cast<unsigned>(raw->GetFeature()):0,material?Texture(material->diffuseTexture):"");
            return RE::BSVisit::BSVisitControl::kContinue;
        });
    }
}
}
void Clear(){++generation;snapshots.store(0);equipmentSnapshots.store(0);}
void SnapshotEquipment(RE::Actor* actor,const char* stage){
    if(!actor || equipmentSnapshots.fetch_add(1)>=snapshotLimit)return;
    auto* npc=actor->GetActorBase();
    const auto& biped=actor->GetBiped();
    logger::info("Equipment snapshot {} actor={:08X} generation={} defaultOutfit={:08X} sleepOutfit={:08X} root={} bipedRoot={}",stage,actor->GetFormID(),generation.load(),npc && npc->defaultOutfit?npc->defaultOutfit->GetFormID():0,npc && npc->sleepOutfit?npc->sleepOutfit->GetFormID():0,static_cast<const void*>(actor->Get3D()),static_cast<const void*>(biped?biped->root:nullptr));
    // The no-init inventory read does not create inventory changes for tracing.
    for(auto& [item,data]:actor->GetInventory([](RE::TESBoundObject& object){return object.IsArmor();},true)){
        auto* entry=data.second.get();auto* armor=item?item->As<RE::TESObjectARMO>():nullptr;
        if(!armor || !entry || !entry->IsWorn())continue;
        logger::info("Equipment worn {} actor={:08X} armor={:08X} name='{}' slots={:08X} count={} wornRight={} wornLeft={}",stage,actor->GetFormID(),armor->GetFormID(),armor->GetName(),armor->GetSlotMask().underlying(),data.first,entry->IsWorn(false),entry->IsWorn(true));
    }
    Biped(actor,biped.get(),stage,false);Biped(actor,biped.get(),stage,true);
}
void Snapshot(RE::Actor* actor,const char* stage){
    if(!actor || snapshots.fetch_add(1)>=snapshotLimit)return;
    auto* npc=actor->GetActorBase();auto* race=actor->GetRace();
    const RE::Color bodyTint=npc?npc->bodyTintColor:RE::Color{};
    logger::info("Appearance snapshot {} actor={:08X} npc={:08X} race={:08X} faceNPC={:08X} root={} generation={} bodyTintRGBA=({},{},{},{})",stage,actor->GetFormID(),npc?npc->GetFormID():0,race?race->GetFormID():0,npc && npc->faceNPC?npc->faceNPC->GetFormID():0,static_cast<const void*>(actor->Get3D()),generation.load(),bodyTint.red,bodyTint.green,bodyTint.blue,bodyTint.alpha);
    if(npc){
        std::uint64_t morphHash=14695981039346656037ULL;
        if(npc->faceData){
            const auto* bytes=reinterpret_cast<const unsigned char*>(npc->faceData);
            for(unsigned i=0;i<sizeof(RE::TESNPC::FaceData);++i){morphHash^=bytes[i];morphHash*=1099511628211ULL;}
        }
        std::string parts;
        if(npc->headParts && npc->numHeadParts>0)for(int i=0;i<npc->numHeadParts;++i)
            if(auto* part=npc->headParts[i])parts+=std::format(" {:08X}",part->GetFormID());
        logger::info("Appearance face data {} actor={:08X} present={} nativeMorphHash={:016X} headParts={}",stage,actor->GetFormID(),npc->faceData!=nullptr,morphHash,parts);
    }
    if(!actor->Get3D())return;
    unsigned emitted=0;
    RE::BSVisit::TraverseScenegraphGeometries(actor->Get3D(),[&](RE::BSGeometry* g){
        if(emitted>=40)return RE::BSVisit::BSVisitControl::kStop;
        auto* shader=g->lightingShaderProp_cast();auto* raw=shader?shader->material:nullptr;
        if(!raw || raw->GetType()!=RE::BSShaderMaterial::Type::kLighting)return RE::BSVisit::BSVisitControl::kContinue;
        auto* material=static_cast<RE::BSLightingShaderMaterialBase*>(raw);
        auto feature=raw->GetFeature();std::string name=g->name.c_str();auto lower=name;
        std::transform(lower.begin(),lower.end(),lower.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
        if(feature!=RE::BSShaderMaterial::Feature::kFaceGen && feature!=RE::BSShaderMaterial::Feature::kFaceGenRGBTint && lower.find("body")==std::string::npos && lower.find("cbbe")==std::string::npos && lower.find("ube")==std::string::npos && lower.find("hand")==std::string::npos && lower.find("feet")==std::string::npos)return RE::BSVisit::BSVisitControl::kContinue;
        ++emitted;
        const char* tint="";RE::NiColor tintColor{};
        if(feature==RE::BSShaderMaterial::Feature::kFaceGen){auto* texture=static_cast<RE::BSLightingShaderMaterialFacegen*>(raw)->tintTexture.get();if(texture)tint=texture->name.c_str();}
        if(feature==RE::BSShaderMaterial::Feature::kFaceGenRGBTint)tintColor=static_cast<RE::BSLightingShaderMaterialFacegenTint*>(raw)->tintColor;
        logger::info("Appearance geometry {} actor={:08X} name='{}' node={} material={} diffuse='{}' normal='{}' shader={} tint='{}' rgbTint=({},{},{}) alpha={} rimSoft='{}' specularBack='{}' specularPower={} specularScale={}",stage,actor->GetFormID(),name,static_cast<const void*>(g),static_cast<const void*>(material),Texture(material->diffuseTexture),Texture(material->normalTexture),static_cast<std::uint32_t>(feature),tint,tintColor.red,tintColor.green,tintColor.blue,material->materialAlpha,Texture(material->rimSoftLightingTexture),Texture(material->specularBackLightingTexture),material->specularPower,material->specularColorScale);
        return RE::BSVisit::BSVisitControl::kContinue;
    });
}
}
