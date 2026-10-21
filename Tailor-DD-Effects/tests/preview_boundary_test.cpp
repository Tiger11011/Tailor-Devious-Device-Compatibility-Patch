#include "HavokStringAddress.h"
#include "CosmeticVisibility.h"
#include "PreviewIdlePolicy.h"
#include "PreviewRequestOrder.h"
#include "WornArmorContext.h"
#include "PartialPoseContext.h"
#include <cassert>
#include <iostream>
#include <string>
#include <array>

int main(){
    static_assert(sizeof(std::uintptr_t)==8);
    // Actual failed low address in the 0.6.9 crash. The old uint32 complement
    // would discard all high bits; tagged and untagged 64-bit values must agree.
    constexpr std::uintptr_t address=0x2795140c1b0ULL;
    static_assert(HavokStringAddress::Untag(address|1)==address);
    static_assert(HavokStringAddress::Untag(address)==address);
    static_assert(HavokStringAddress::Untag(0)==0);
    static_assert((address & ~std::uint32_t{1})==0x5140c1b0ULL);
    // Exercise actual high-address text copying rather than comparing integers
    // alone. No engine objects, DLLs or game functions are executed.
    alignas(2) char name[]="MT_Idle.HKX";
    auto raw=reinterpret_cast<std::uintptr_t>(name);
    assert(raw>0xffffffffULL && !(raw&1));
    struct Tagged {std::uintptr_t pointer;};
    Tagged tagged{raw|1};assert(std::string(HavokStringAddress::Read(tagged))==name);
    tagged.pointer=raw;assert(std::string(HavokStringAddress::Read(tagged))==name);
    tagged.pointer=0;assert(HavokStringAddress::Read(tagged)==nullptr);
    std::array<Tagged,2> names{{{0},{raw|1}}};
    assert(HavokStringAddress::ReadIndexed(&names,0)==nullptr);
    assert(std::string(HavokStringAddress::ReadIndexed(&names,1))==name);
    assert(HavokStringAddress::ReadIndexed(&names,2)==nullptr);
    assert(HavokStringAddress::ReadIndexed(&names,65535)==nullptr);
    assert((HavokStringAddress::ReadIndexed<std::array<Tagged,2>>(nullptr,0)==nullptr));

    CosmeticVisibility::Table visible{{0xff001234,{0x01000010,{0x02000010}}}};
    assert(CosmeticVisibility::Allows(visible,0xff001234,0x01000010,0x02000010));
    assert(!CosmeticVisibility::Allows(visible,0xff0008b8,0x01000010,0x02000010));
    assert(!CosmeticVisibility::Allows(visible,0xff001234,0x01000011,0x02000010));
    assert(!CosmeticVisibility::Allows(visible,0xff001234,0x01000010,0x03000010));
    assert(!CosmeticVisibility::Allows(visible,0xff001234,0x01000010,0x02000011));
    visible.clear();assert(!CosmeticVisibility::Allows(visible,0xff001234,0x01000010,0x02000010));

    // Synthetic fixture: the initial ordinary outfit waits for DD unlock, Pet runs
    // immediately, then the old ordinary outfit continuation must be rejected.
    PreviewRequestOrder::Requests requests;
    constexpr auto npc=0xff001234u;
    auto ordinaryOutfit=requests.Replace(npc);
    auto pet=requests.Replace(npc);
    assert(requests.Current(npc,pet));assert(!requests.Current(npc,ordinaryOutfit));
    auto other=requests.Replace(npc+1);assert(requests.Current(npc,pet));
    auto edit1=requests.Edit(npc),edit2=requests.Edit(npc);
    assert(!requests.Current(npc,pet));
    assert(requests.SameReplacement(npc,edit1));assert(requests.SameReplacement(npc,edit2));
    auto nextOutfit=requests.Replace(npc);
    assert(!requests.SameReplacement(npc,edit1));assert(requests.Current(npc,nextOutfit));
    requests.Clear();assert(!requests.Current(npc,nextOutfit));assert(!requests.Current(npc+1,other));

    // The actual optimized DD caller left RDX=4 in the crash. It is never an
    // armor pointer here. Obtain IDs exclusively from the outer boundary.
    assert(WornArmorContext::current==nullptr);
    const void* actor=reinterpret_cast<const void*>(0x12340000);
    {
        WornArmorContext::Scope outer({actor,npc,0x01000010,0x02000010});
        auto* current=WornArmorContext::current;
        auto ignoredArg=reinterpret_cast<void*>(4);
        auto observe=[&](void*,const void* actualActor){
            auto* ctx=WornArmorContext::current;
            return ctx && ctx->actor==actualActor ? ctx->armorID : 0;
        };
        assert(observe(ignoredArg,actor)==0x02000010);
        assert(observe(ignoredArg,reinterpret_cast<const void*>(1))==0);
        {
            WornArmorContext::Scope nested({nullptr,0,0,0});
            assert(WornArmorContext::current!=current);
            assert(observe(ignoredArg,actor)==0);
        }
        assert(WornArmorContext::current==current);
        assert(observe(ignoredArg,actor)==0x02000010);
    }
    assert(WornArmorContext::current==nullptr);
    for(auto event:{"IdleHHLeft","IdleHHRight"}){
        assert(PreviewIdlePolicy::Suppress(true,false,event));
        assert(!PreviewIdlePolicy::Suppress(false,false,event));
        assert(!PreviewIdlePolicy::Suppress(true,true,event));
    }
    for(auto event:{"IdleForceDefaultState","IdleStop","IdleHandsBehindBack","DDPetSuit_Struggle","FootLeft",""})
        assert(!PreviewIdlePolicy::Suppress(true,false,event));
    // PickNewIdle is a graph -> actor request, not an incoming graph notify.
    // Preserve the event dispatch/observer path while suppressing only the
    // automatic reselect handler for this active Pet NPC. Exiting the scope
    // restores its normal request; player/other NPC events must pass through.
    struct Actor {bool pet=false,player=false,newIdle=false;unsigned scripts=0,observers=0,explicitIdles=0;};
    auto pick=[](Actor& a){
        if(PreviewIdlePolicy::BlockAutomaticReselection(a.pet,a.player))return true;
        a.newIdle=true;return true;
    };
    auto dispatch=[&](Actor& a){++a.scripts;const bool handled=pick(a);++a.observers;return handled;};
    Actor fitted{true,false};assert(dispatch(fitted));
    assert(!fitted.newIdle && fitted.scripts==1 && fitted.observers==1);
    ++fitted.explicitIdles;assert(fitted.explicitIdles==1); // other handler unchanged
    Actor ordinary{};assert(dispatch(ordinary) && ordinary.newIdle);
    Actor player{true,true};assert(dispatch(player) && player.newIdle);
    fitted.pet=false;assert(dispatch(fitted) && fitted.newIdle);
    assert(fitted.scripts==2 && fitted.observers==2);
    // PAR's outer actor cache call may nest. An inner unrelated actor must
    // not inherit the fitted actor's permission to recheck a cached pose.
    const void* root=reinterpret_cast<const void*>(0x12345000);
    const void* otherRoot=reinterpret_cast<const void*>(0x12346000);
    assert(!PartialPoseContext::Matches(npc,root));
    {
        PartialPoseContext::Scope outer({actor,npc,root});
        assert(PartialPoseContext::Matches(npc,root));
        assert(!PartialPoseContext::Matches(npc,otherRoot));
        assert(!PartialPoseContext::Matches(npc+1,root));
        assert(!PartialPoseContext::Matches(0,root)); // close/target cleared
        {
            PartialPoseContext::Scope nested({nullptr,npc+1,otherRoot});
            assert(!PartialPoseContext::Matches(npc,root));
            assert(!PartialPoseContext::Matches(npc+1,otherRoot));
        }
        assert(PartialPoseContext::Matches(npc,root));
        unsigned applied=0,evaluated=0;
        auto cachedApply=[&](std::uint32_t target,const void* object,bool condition){
            if(PartialPoseContext::Matches(target,object)){
                ++evaluated;if(!condition)return;
            }
            ++applied;
        };
        cachedApply(npc,root,false);assert(applied==0 && evaluated==1);
        cachedApply(npc,root,true);assert(applied==1 && evaluated==2);
        cachedApply(npc+1,root,false);assert(applied==2 && evaluated==2);
        cachedApply(0,root,false);assert(applied==3 && evaluated==2);
    }
    assert(PartialPoseContext::current==nullptr);
    {PartialPoseContext::Scope playerScope({actor,0x14,root});assert(!PartialPoseContext::Matches(0x14,root));}
    std::cout<<"Havok text, last-request-wins, nested worn context/unused RDX and visibility/idle boundaries passed\n";
}
