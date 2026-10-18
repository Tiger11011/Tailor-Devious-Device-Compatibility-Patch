#include "OperationPolicy.h"
#include <cassert>
#include <iostream>
int main(){
    using namespace FreeWear::Policy;
    // E1 -> R2 -> E3: no mutual wait, even when E3 starts before R2 completes.
    assert(WaitForEarlier(2,1,false));
    assert(!WaitForEarlier(1,2,false));
    assert(WaitForEarlier(3,2,false));
    assert(!WaitForEarlier(2,3,false));
    assert(!WaitForEarlier(3,2,true));
    assert(!WaitForEarlier(3,3,false));
    assert(!WaitForEarlier(3,0,false));
    // The observed inventory-only failure can continue normal clothing changes;
    // a live rendered DD or an unfinished equip must still follow DD removal.
    assert(ContinuePastFailedEquip(false,true,false));
    assert(!ContinuePastFailedEquip(true,true,false));
    assert(!ContinuePastFailedEquip(false,false,false));
    assert(!ContinuePastFailedEquip(false,true,true));
    std::cout<<"Ordered DD equip/removal waits and inventory-only failure recovery policy passed\n";
}
