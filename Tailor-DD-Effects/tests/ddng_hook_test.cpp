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
    assert(std::equal(prefix.begin(),prefix.end(),target));
    constexpr std::array<unsigned char,14> hider{0x48,0x83,0xec,0x28,0x0f,0xb6,0x91,0x88,0,0,0,0x83,0xea,1};
    target=mapped+0x78c80;assert(std::equal(hider.begin(),hider.end(),target));trampoline=nullptr;
    assert(MH_CreateHook(target,reinterpret_cast<void*>(&Dummy),&trampoline)==MH_OK);assert(trampoline);
    assert(std::equal(hider.begin(),hider.end(),target));
    assert(MH_EnableHook(target)==MH_OK);assert(!std::equal(hider.begin(),hider.end(),target));
    assert(MH_DisableHook(target)==MH_OK);assert(MH_RemoveHook(target)==MH_OK);
    assert(std::equal(hider.begin(),hider.end(),target));
    // Unlike the original hider-construction test, explicitly check the real
    // optimized callsite. RDX is NOT assigned before CheckNPCArmor. Hook
    // relocation alone cannot validate the source-level parameter ABI.
    constexpr std::array<unsigned char,11> callsite{0x4d,0x8b,0xc4,0x49,0x8b,0xcd,0xe8,0xec,0xfa,0xff,0xff};
    assert(std::equal(callsite.begin(),callsite.end(),mapped+0x79189));
    constexpr std::array<unsigned char,28> worn{0x48,0x89,0x5c,0x24,8,0x4c,0x89,0x44,0x24,0x18,0x55,0x56,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x81,0xec,0xa0,0,0,0};
    target=mapped+0x79000;assert(std::equal(worn.begin(),worn.end(),target));trampoline=nullptr;
    assert(MH_CreateHook(target,reinterpret_cast<void*>(&Dummy),&trampoline)==MH_OK);assert(trampoline);
    assert(std::equal(worn.begin(),worn.end(),target));
    assert(MH_EnableHook(target)==MH_OK);assert(!std::equal(worn.begin(),worn.end(),target));
    assert(MH_DisableHook(target)==MH_OK);assert(MH_RemoveHook(target)==MH_OK);
    assert(std::equal(worn.begin(),worn.end(),target));assert(MH_Uninitialize()==MH_OK);
    VirtualFree(mapped,0,MEM_RELEASE);
    std::cout<<"DD NG filter/hider/worn boundary relocation and actual optimized callsite checked; no DD code executed\n";
}
