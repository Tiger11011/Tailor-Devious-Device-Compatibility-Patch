#include "AutoFittingFormat.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
using namespace AutoFitting::Format;
int main(int argc,char** argv) {
    if(argc==3) {
        try {
            std::ifstream f(argv[2],std::ios::binary);
            if(!f)throw std::runtime_error("test input missing");
            std::vector<std::uint8_t> bytes{std::istreambuf_iterator<char>(f),{}};
            if(std::string(argv[1])=="pex") {auto p=PEX(bytes);std::cout<<p.name<<'\t'<<p.parent<<'\n';}
            else for(const auto& s:VMAD(bytes)) {
                std::cout<<s.name;
                for(const auto& [key,id]:s.objects)std::cout<<'\t'<<key<<'='<<id;
                std::cout<<'\n';
            }
            return 0;
        }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 2;}
    }
    std::map<std::string,std::uint32_t> ledger;
    auto a=Allocate(ledger,"a.esp|000800"),b=Allocate(ledger,"b.esp|000800");
    auto saved=ledger;
    assert(a==FirstSlot && b==FirstSlot+1);
    assert(Allocate(ledger,"a.esp|000800")==a);
    // A newly added outfit that sorts before both old entries cannot move them.
    assert(Allocate(ledger,"0-new.esp|000900")==FirstSlot+2);
    for(const auto& [key,id]:saved)assert(ledger.at(key)==id);
    // An inactive source stays in the ledger rather than donating its slot.
    assert(Allocate(ledger,"c.esp|000800")==FirstSlot+3);
    assert(ledger.at("b.esp|000800")==b);
    bool rejected=false;try {VMAD({});}catch(...){rejected=true;}assert(rejected);
    rejected=false;try {PEX({});}catch(...){rejected=true;}assert(rejected);
    ledger={{"full",EndSlot-1}};
    rejected=false;try {Allocate(ledger,"overflow");}catch(...){rejected=true;}assert(rejected);
    std::cout<<"Stable append-only slots and malformed metadata checks passed\n";
}
