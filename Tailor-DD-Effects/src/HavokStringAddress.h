#pragma once
#include <cstdint>
#include <cstring>

namespace HavokStringAddress {
// Havok tags bit zero; the rest is a pointer, not a 32-bit string identifier.
constexpr std::uintptr_t Untag(std::uintptr_t address) noexcept {
    return address & ~std::uintptr_t{1};
}
template<class StringPointer>
const char* Read(const StringPointer& value) noexcept {
    static_assert(sizeof(StringPointer)==sizeof(std::uintptr_t));
    std::uintptr_t address=0;
    std::memcpy(&address,&value,sizeof(address));
    return reinterpret_cast<const char*>(Untag(address));
}
template<class StringArray>
const char* ReadIndexed(const StringArray* values,std::uint32_t index) noexcept {
    // hkbClipGenerator has a 16-bit binding index. Refuse an absent or
    // inconsistent table instead of following an unchecked animation index.
    if(!values)return nullptr;
    const auto count=static_cast<std::uint64_t>(values->size());
    if(count>65536 || index>=count)return nullptr;
    return Read((*values)[index]);
}
}
