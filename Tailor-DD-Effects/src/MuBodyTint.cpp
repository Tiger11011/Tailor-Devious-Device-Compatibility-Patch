#include "PCH.h"
#include "MuBodyTint.h"
#include "MuBodyTintFormat.h"

namespace MuBodyTint {
bool Restore(RE::Actor* actor,const std::string& resourcePath){
    if(!actor || actor->IsPlayerRef() || !actor->Is3DLoaded())return false;
    auto* npc=actor->GetActorBase();if(!npc)return false;
    const auto path=SelectedResourcePath(resourcePath);if(!path)return false;
    try{
        RE::BSResourceNiBinaryStream stream(*path);
        if(!stream.good() || !stream.stream){logger::warn("Mu body tint: selected preset resource unavailable '{}'",*path);return false;}
        const auto size=stream.stream->totalSize;
        if(size==0 || size>16*1024*1024){logger::warn("Mu body tint: selected preset length invalid '{}'",*path);return false;}
        std::string text(size,'\0');
        if(!stream.read(text.data(),size)){logger::warn("Mu body tint: selected preset read incomplete '{}'",*path);return false;}
        const auto tone=ParseSkinTone(text);
        if(!tone){logger::warn("Mu body tint: selected preset has no unambiguous skin tone '{}'",*path);return false;}
        const auto before=npc->bodyTintColor;
        RE::TintMask tint{};
        tint.color.red=tone->red;tint.color.green=tone->green;tint.color.blue=tone->blue;
        tint.alpha=tone->alpha;tint.type=RE::TintMask::Type::kSkinTone;
        RE::NiColorA result{};
        // Match RaceMenu ApplyPresetData's skin-tone conversion only:
        // https://github.com/expired6978/SKSE64Plugins/blob/master/skee64/PresetInterface.cpp#L179-L201
        npc->SetSkinFromTint(&result,&tint,true);
        actor->UpdateSkinColor();
        const auto after=npc->bodyTintColor;
        logger::info("Mu body tint: restored {:08X} from selected preset '{}' ARGB={:08X} alpha={}; bodyTintRGBA=({},{},{},{}) -> ({},{},{},{})",actor->GetFormID(),*path,tone->argb,tone->alpha,before.red,before.green,before.blue,before.alpha,after.red,after.green,after.blue,after.alpha);
        return true;
    }catch(const std::exception& error){logger::warn("Mu body tint: selected preset restore skipped: {}",error.what());return false;}
}
}
