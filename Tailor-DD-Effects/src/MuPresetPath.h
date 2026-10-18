#pragma once
#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include <string_view>

namespace MuAppearance {
// Mu stores an exported preset path with its extension. RaceMenu's
// LoadCharacterEx adds Exported/ and .jslot (and derives a .dds tint path).
inline std::optional<std::string> ExportedPresetStem(std::string path) {
    std::replace(path.begin(),path.end(),'/','\\');
    if(path.empty() || path.front()=='\\' || path.find(':')!=std::string::npos)return {};
    for(std::size_t start=0;start<path.size();) {
        auto end=path.find('\\',start);
        auto part=std::string_view(path).substr(start,end==std::string::npos?path.size()-start:end-start);
        if(part.empty() || part=="." || part=="..")return {};
        if(end==std::string::npos)break;
        start=end+1;
    }
    if(path.size()<=6)return {};
    auto extension=path.substr(path.size()-6);
    std::transform(extension.begin(),extension.end(),extension.begin(),[](unsigned char c){return static_cast<char>(std::tolower(c));});
    if(extension!=".jslot")return {};
    path.resize(path.size()-6);
    if(path.empty() || path.back()=='\\')return {};
    return path;
}
}
