// Inspect and temporarily patch a private memory copy. Never load or execute
// PAR, its conditions, game code or the constructed trampolines.
#include <Windows.h>
#include <MinHook.h>
#include <array>
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
void Dummy(){}
int wmain(int argc,wchar_t** argv){
    assert(argc==2);
    std::ifstream f(std::filesystem::path(argv[1]),std::ios::binary);assert(f);
    std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(f),{}};
    const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(bytes.data());assert(dos->e_magic==IMAGE_DOS_SIGNATURE);
    const auto* pe=reinterpret_cast<const IMAGE_NT_HEADERS64*>(bytes.data()+dos->e_lfanew);assert(pe->Signature==IMAGE_NT_SIGNATURE);
    const auto size=pe->OptionalHeader.SizeOfImage;
    auto* mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(mapped);
    std::copy_n(bytes.data(),pe->OptionalHeader.SizeOfHeaders,mapped);
    auto* sections=IMAGE_FIRST_SECTION(pe);
    for(unsigned i=0;i<pe->FileHeader.NumberOfSections;++i){
        const auto& s=sections[i];assert(s.PointerToRawData+s.SizeOfRawData<=bytes.size());assert(s.VirtualAddress+s.SizeOfRawData<=size);
        std::copy_n(bytes.data()+s.PointerToRawData,s.SizeOfRawData,mapped+s.VirtualAddress);
    }
    constexpr std::array<unsigned char,16> actor{0x40,0x56,0x48,0x83,0xec,0x20,0x4c,0x8b,0x11,0x48,0xb8,0x25,0x23,0x22,0x84,0xe4};
    constexpr std::array<unsigned char,15> apply{0x48,0x8b,0xc4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20};
    constexpr std::array<unsigned char,13> eval{0x48,0x83,0xec,0x28,0x48,0x8b,0x49,0x40,0x48,0x85,0xc9,0x74,0x13};
    assert(std::equal(actor.begin(),actor.end(),mapped+0x86cb0));
    assert(std::equal(apply.begin(),apply.end(),mapped+0x82920));
    assert(std::equal(eval.begin(),eval.end(),mapped+0x828f0));
    // Actual optimized cached-vector caller supplies RCX=Replacer*, RDX=root.
    constexpr std::array<unsigned char,7> arguments{0x48,0x8b,0x0b,0x48,0x8b,0xd6,0xe8};
    assert(std::equal(arguments.begin(),arguments.end(),mapped+0x86d65));
    assert(0x86d6b+5+*reinterpret_cast<const int*>(mapped+0x86d6c)==0x82920);
    assert(MH_Initialize()==MH_OK);
    std::array<void*,2> targets{mapped+0x86cb0,mapped+0x82920};
    std::array<std::array<unsigned char,32>,2> originals{};
    for(unsigned i=0;i<targets.size();++i){
        std::copy_n(static_cast<unsigned char*>(targets[i]),32,originals[i].begin());
        void* trampoline=nullptr;
        assert(MH_CreateHook(targets[i],reinterpret_cast<void*>(&Dummy),&trampoline)==MH_OK && trampoline);
        assert(std::equal(originals[i].begin(),originals[i].end(),static_cast<unsigned char*>(targets[i])));
        assert(MH_QueueEnableHook(targets[i])==MH_OK);
    }
    assert(MH_ApplyQueued()==MH_OK);
    for(unsigned i=0;i<targets.size();++i){
        assert(!std::equal(originals[i].begin(),originals[i].end(),static_cast<unsigned char*>(targets[i])));
        assert(MH_DisableHook(targets[i])==MH_OK);assert(MH_RemoveHook(targets[i])==MH_OK);
        assert(std::equal(originals[i].begin(),originals[i].end(),static_cast<unsigned char*>(targets[i])));
    }
    assert(MH_Uninitialize()==MH_OK);VirtualFree(mapped,0,MEM_RELEASE);
    std::cout<<"PAR actor/pose entry construction, atomic activation/rollback and real cached-loop arguments passed; no PAR/game code executed\n";
}
