#pragma once
#include <RE/Skyrim.h>
#include <string>
#include <vector>
namespace AutoFitting {
inline constexpr const char* Plugin="TailorDDAutoFitting.esp";
struct Row { RE::TESObjectARMO* inventory; RE::TESObjectARMO* rendered; RE::TESObjectARMO* cosmetic; RE::TESObjectARMO* automatic; };
struct Result { std::vector<Row> rows; std::vector<RE::FormID> excluded; };
// Runs once at kDataLoaded. The shipped generic plugin contains empty slots;
// appearance comes only from the currently loaded user's own winning records.
Result Build();
}
