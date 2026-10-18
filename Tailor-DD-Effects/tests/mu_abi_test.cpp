// Verify the explicit MSVC member-function sret adapter used with Mu's PDB
// signature, including ownership/destruction of the returned shared_ptr.
#include <memory>
#include <cstring>
#include <cassert>
#include <iostream>
struct Npc{int value=42;};
struct Manager{
    std::shared_ptr<int> data=std::make_shared<int>(17);
    __declspec(noinline) const std::shared_ptr<void> Current(Npc* npc)const{
        assert(npc && npc->value==42);return data;
    }
};
using Adapter=void*(*)(void*,std::shared_ptr<void>*,Npc*);
int main(){
    Manager manager;Npc npc;auto member=&Manager::Current;
    static_assert(sizeof(member)==sizeof(Adapter));Adapter adapter;std::memcpy(&adapter,&member,sizeof(adapter));
    {std::shared_ptr<void> result;auto* returned=adapter(&manager,&result,&npc);assert(returned==&result);assert(result.get()==manager.data.get());assert(manager.data.use_count()==2);}
    assert(manager.data.use_count()==1);
    std::cout<<"Mu PDB member/shared_ptr return ABI and lifetime probe passed; no Mu code executed\n";
}
