#include "PCH.h"
#include "AppearanceTrace.h"
namespace AppearanceTrace {
void Snapshot(RE::Actor* actor,const char* stage){
    if(!actor)return;
    static std::atomic_uint count{0};if(count.fetch_add(1)>200)return;
    auto* npc=actor->GetActorBase();auto* race=actor->GetRace();
    logger::info("Appearance snapshot {} actor={:08X} npc={:08X} race={:08X} faceNPC={:08X} root={}",stage,actor->GetFormID(),npc?npc->GetFormID():0,race?race->GetFormID():0,npc && npc->faceNPC?npc->faceNPC->GetFormID():0,static_cast<const void*>(actor->Get3D()));
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
        const char* tint="";
        if(feature==RE::BSShaderMaterial::Feature::kFaceGen){auto* texture=static_cast<RE::BSLightingShaderMaterialFacegen*>(raw)->tintTexture.get();if(texture)tint=texture->name.c_str();}
        logger::info("Appearance geometry {} actor={:08X} name='{}' node={} material={} diffuse='{}' normal='{}' shader={} tint='{}'",stage,actor->GetFormID(),name,static_cast<const void*>(g),static_cast<const void*>(material),material->diffuseTexture?material->diffuseTexture->name.c_str():"",material->normalTexture?material->normalTexture->name.c_str():"",static_cast<std::uint32_t>(feature),tint);
        return RE::BSVisit::BSVisitControl::kContinue;
    });
}
}
