#include "MuPresetPath.h"
#include <cassert>
#include <iostream>
int main(){
    using MuAppearance::ExportedPresetStem;
    assert(ExportedPresetStem("Chargen\\Presets\\Example With Spaces.jslot")=="Chargen\\Presets\\Example With Spaces");
    assert(ExportedPresetStem("Chargen/Presets/Example.JSLOT")=="Chargen\\Presets\\Example");
    assert(ExportedPresetStem("Example.jslot")=="Example");
    for(auto path:{"", "../Example.jslot", "C:\\Example.jslot", "\\Example.jslot", "Example.nif", ".jslot", "folder\\.jslot"})assert(!ExportedPresetStem(path));
    const auto stem=*ExportedPresetStem("Chargen\\Presets\\Example With Spaces.jslot");
    assert("SKSE\\Plugins\\CharGen\\Exported\\"+stem+".jslot"=="SKSE\\Plugins\\CharGen\\Exported\\Chargen\\Presets\\Example With Spaces.jslot");
    assert("Textures\\CharGen\\Exported\\"+stem+".dds"=="Textures\\CharGen\\Exported\\Chargen\\Presets\\Example With Spaces.dds");
    std::cout<<"Mu selected export path and tint regression checks passed\n";
}
