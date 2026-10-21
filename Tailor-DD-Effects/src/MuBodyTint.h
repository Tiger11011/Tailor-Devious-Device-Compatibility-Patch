#pragma once
#include <string>
namespace RE {class Actor;}
namespace MuBodyTint {
// Main-thread only. The caller must resolve and verify the actor's current Mu
// preset/race and scope the request to an actor managed by this bridge.
bool Restore(RE::Actor* actor,const std::string& resourcePath);
}
