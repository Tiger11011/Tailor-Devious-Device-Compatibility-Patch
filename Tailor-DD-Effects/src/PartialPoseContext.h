#pragma once
#include <cstdint>

// Opaque context across PAR's verified actor -> cached replacer call. Never
// interpret its shared_ptr/map, Replacer layout or third-party allocator data.
namespace PartialPoseContext {
struct Context {const void* actor;std::uint32_t actorID;const void* root;};
inline thread_local const Context* current=nullptr;
class Scope {
public:
    explicit Scope(Context value):value(value),previous(current){current=&this->value;}
    ~Scope(){current=previous;}
    Scope(const Scope&)=delete;
    Scope& operator=(const Scope&)=delete;
private:
    Context value;
    const Context* previous;
};
inline bool Matches(std::uint32_t previewActor,const void* root) noexcept {
    return current && current->actor && previewActor && previewActor!=0x14 &&
        current->actorID==previewActor && current->root && current->root==root;
}
}
