// Map the reviewed DD NG PE as plain bytes and test MinHook relocation/rollback.
// Never load or execute any DD NG code. Assertions are intentionally enabled.
#include <Windows.h>
#include <MinHook.h>
#include <array>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <vector>
#include <iostream>
#include <algorithm>
bool Dummy(){return false;}
int wmain(int argc,wchar_t** argv){
    assert(argc==2);
    std::ifstream f(std::filesystem::path(argv[1]),std::ios::binary);assert(f);
    std::vector<unsigned char> bytes{std::istreambuf_iterator<char>(f),{}};
    const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(bytes.data());
    assert(dos->e_magic==IMAGE_DOS_SIGNATURE);
    const auto* pe=reinterpret_cast<const IMAGE_NT_HEADERS64*>(bytes.data()+dos->e_lfanew);
    assert(pe->Signature==IMAGE_NT_SIGNATURE);
    auto size=pe->OptionalHeader.SizeOfImage;
    auto* mapped=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(mapped);
    std::copy_n(bytes.data(),pe->OptionalHeader.SizeOfHeaders,mapped);
    const auto* sections=IMAGE_FIRST_SECTION(pe);
    for(unsigned j=0;j<pe->FileHeader.NumberOfSections;++j){
        const auto& s=sections[j];assert(s.PointerToRawData+s.SizeOfRawData<=bytes.size());assert(s.VirtualAddress+s.SizeOfRawData<=size);
        std::copy_n(bytes.data()+s.PointerToRawData,s.SizeOfRawData,mapped+s.VirtualAddress);
    }
    constexpr std::array<unsigned char,32> prefix{72,137,92,36,32,85,86,87,65,84,65,85,65,86,65,87,72,141,108,36,217,72,129,236,240,0,0,0,72,139,5,45};
    auto* target=mapped+0x8ce70;assert(std::equal(prefix.begin(),prefix.end(),target));
    assert(MH_Initialize()==MH_OK);void* trampoline=nullptr;
    assert(MH_CreateHook(target,reinterpret_cast<void*>(&Dummy),&trampoline)==MH_OK);assert(trampoline);
    assert(std::equal(prefix.begin(),prefix.end(),target)); // prepared hook is inert
    assert(MH_EnableHook(target)==MH_OK);assert(!std::equal(prefix.begin(),prefix.end(),target));
    assert(MH_DisableHook(target)==MH_OK);assert(MH_RemoveHook(target)==MH_OK);
    assert(std::equal(prefix.begin(),prefix.end(),target));assert(MH_Uninitialize()==MH_OK);
    VirtualFree(mapped,0,MEM_RELEASE);
    std::cout<<"DD NG reviewed filter: relocation, activation, restoration passed; no DD code executed\n";
}
