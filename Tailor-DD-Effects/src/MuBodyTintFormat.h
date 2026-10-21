#pragma once
#include "MuPresetPath.h"
#include <cstdint>
#include <limits>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>

namespace MuBodyTint {
struct SkinTone {
    std::uint32_t argb;
    std::uint8_t red,green,blue;
    float alpha;
};

// RaceMenu's tintInfo index zero is the skin tone. Retain alpha for the
// engine's SetSkinFromTint conversion rather than assuming the RGB is final.
inline std::optional<SkinTone> ParseSkinTone(std::string_view text){
    auto preset=nlohmann::json::parse(text,nullptr,false);
    if(!preset.is_object())return {};
    auto tints=preset.find("tintInfo");if(tints==preset.end() || !tints->is_array())return {};
    std::optional<SkinTone> result;
    for(const auto& tint:*tints){
        if(!tint.is_object())return {};
        auto index=tint.find("index");
        if(index==tint.end() || !index->is_number_integer())return {};
        if(*index!=0)continue;
        if(result)return {};
        auto color=tint.find("color");
        if(color==tint.end() || !color->is_number_integer())return {};
        if(color->is_number_unsigned()){
            if(color->get<std::uint64_t>()>std::numeric_limits<std::uint32_t>::max())return {};
        }else if(color->get<std::int64_t>()<0 || color->get<std::int64_t>()>std::numeric_limits<std::uint32_t>::max())return {};
        auto texture=tint.find("texture");
        if(texture!=tint.end() && !texture->is_string())return {};
        const auto packed=color->get<std::uint32_t>();
        result=SkinTone{packed,static_cast<std::uint8_t>((packed>>16)&0xff),static_cast<std::uint8_t>((packed>>8)&0xff),static_cast<std::uint8_t>(packed&0xff),static_cast<float>((packed>>24)/255.0)};
    }
    return result;
}

inline std::optional<std::string> SelectedResourcePath(std::string path){
    if(path.find('\0')!=std::string::npos)return {};
    std::replace(path.begin(),path.end(),'/','\\');
    auto lower=path;
    std::transform(lower.begin(),lower.end(),lower.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
    constexpr std::string_view prefix="skse\\plugins\\chargen\\exported\\";
    if(!lower.starts_with(prefix))return {};
    const auto relative=path.substr(prefix.size());
    if(!MuAppearance::ExportedPresetStem(relative))return {};
    return path;
}
}
