#pragma once
#include <cstdint>

// DD NG's optimized CheckNPCArmor caller does not pass its unused armor formal
// parameter. Only the verified InitWornArmor boundary supplies a real armor.
// Keep that information on the synchronous call stack, never in a queued job.
namespace WornArmorContext {
struct Context {
    const void* actor;
    std::uint32_t actorID, baseID, armorID;
    bool npcChecked=false, nativeAllowed=false;
};
inline thread_local Context* current=nullptr;
class Scope {
    Context value;
    Context* previous;
public:
    explicit Scope(Context context):value(context),previous(current) { current=&value; }
    ~Scope() { current=previous; }
    const Context& Get() const { return value; }
    Scope(const Scope&)=delete;
    Scope& operator=(const Scope&)=delete;
};
}
