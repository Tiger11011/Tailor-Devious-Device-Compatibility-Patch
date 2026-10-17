// Exercises creation, activation and removal of hooks on mapped copies of the exact
// supported entry points. Never loads Tailor as a DLL or executes its code.
#include <Windows.h>
#include <MinHook.h>
#include "NativeProfiles.h"
#include "EquipmentProfiles.h"
#include <cassert>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>
#include <algorithm>

bool Dummy() { return false; }
int wmain(int argc, wchar_t** argv) {
    if (argc != 3) return 2;
    assert(MH_Initialize() == MH_OK);
    for (int i = 0; i < 2; ++i) {
        std::ifstream file(std::filesystem::path(argv[i + 1]), std::ios::binary);
        assert(file);
        const std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(bytes.data());
        const auto* pe = reinterpret_cast<const IMAGE_NT_HEADERS64*>(bytes.data() + dos->e_lfanew);
        const auto& profile = profiles[i];
        assert(pe->OptionalHeader.SizeOfImage == profile.imageSize);
        auto* mapped = static_cast<unsigned char*>(VirtualAlloc(nullptr, profile.imageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE));
        assert(mapped);
        std::copy_n(bytes.data(), pe->OptionalHeader.SizeOfHeaders, mapped);
        const auto* section = IMAGE_FIRST_SECTION(pe);
        for (unsigned j = 0; j < pe->FileHeader.NumberOfSections; ++j) {
            assert(section[j].PointerToRawData + section[j].SizeOfRawData <= bytes.size());
            assert(section[j].VirtualAddress + section[j].SizeOfRawData <= profile.imageSize);
            std::copy_n(bytes.data() + section[j].PointerToRawData, section[j].SizeOfRawData, mapped + section[j].VirtualAddress);
        }
        std::vector<EquipmentPoint> points{equipmentProfiles[i].points.begin(), equipmentProfiles[i].points.end()};
        points.push_back({profile.settingsRva, profile.prologue});
        for (const auto& point : points) {
        auto* target = mapped + point.rva;
        assert(std::equal(point.prologue.begin(), point.prologue.end(), target));
        void* trampoline = nullptr;
        auto result = MH_CreateHook(target, reinterpret_cast<void*>(&Dummy), &trampoline);
        if (result != MH_OK) { std::cerr << MH_StatusToString(result) << '\n'; return 3; }
        assert(trampoline);
        assert(MH_EnableHook(target) == MH_OK);
        assert(!std::equal(point.prologue.begin(), point.prologue.end(), target));
        assert(MH_DisableHook(target) == MH_OK);
        assert(MH_RemoveHook(target) == MH_OK);
        assert(std::equal(point.prologue.begin(), point.prologue.end(), target));
        }
        VirtualFree(mapped, 0, MEM_RELEASE);
        std::cout << profile.name << ": all ten hook constructions and rollbacks passed; Tailor code was not executed\n";
    }
    assert(MH_Uninitialize() == MH_OK);
}
