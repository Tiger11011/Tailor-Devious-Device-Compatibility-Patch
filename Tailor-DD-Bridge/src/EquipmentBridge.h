#pragma once
#include <atomic>
namespace Equipment {
    extern std::atomic_bool activateOnExit;
    bool Install(std::uintptr_t base, std::size_t profile);
    bool Ready();
    std::size_t PairCount();
    void RegisterSerialization();
    void Clear();
}
