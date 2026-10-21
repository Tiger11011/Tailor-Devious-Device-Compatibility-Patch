#pragma once
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace CosmeticVisibility {
struct Scope {std::uint32_t base;std::vector<std::uint32_t> armors;};
using Table=std::unordered_map<std::uint32_t,Scope>;
inline bool Allows(const Table& table,std::uint32_t actor,std::uint32_t base,std::uint32_t armor){
    auto it=table.find(actor);
    return actor && base && armor && it!=table.end() && it->second.base==base &&
        std::find(it->second.armors.begin(),it->second.armors.end(),armor)!=it->second.armors.end();
}
}
