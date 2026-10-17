#include "DevicePolicy.h"
#include <cassert>
#include <iostream>
int main() {
    using namespace Equipment::Policy;
    assert(UseReal(true, false, 7, 7, 100, 100));
    assert(!UseReal(true, true, 7, 7, 100, 100));
    assert(!UseReal(false, false, 7, 7, 100, 100));
    assert(!UseReal(true, false, 0, 0, 100, 100));
    assert(!UseReal(true, false, 8, 7, 100, 100));
    assert(!UseReal(true, false, 7, 7, 101, 100)); // edited/replaced library entry
    assert(Handoff(true, true, false, true, 5, 5));
    assert(!Handoff(false, true, false, true, 5, 5)); // no Set
    assert(!Handoff(true, false, false, true, 5, 5)); // focus/lifecycle close
    assert(!Handoff(true, true, true, true, 5, 5)); // reopened
    assert(!Handoff(true, true, false, false, 5, 5)); // invalid/unloaded actor
    assert(!Handoff(true, true, false, true, 5, 6)); // load or a later session
    std::cout << "Device selection and confirmed-exit policy passed\n";
}
