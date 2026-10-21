#pragma once
#include <cstdint>
#include <unordered_map>

// Complete outfit replacements are last-request-wins. Incremental edits share
// an epoch, so adding a second piece does not cancel the first pending add.
// The caller serializes access and separately checks the current world/session.
namespace PreviewRequestOrder {
struct Ticket { std::uint64_t serial, replacement; };
class Requests {
    std::unordered_map<std::uint32_t, Ticket> actors;
public:
    Ticket Replace(std::uint32_t actor) {
        auto& value=actors[actor]; ++value.serial; ++value.replacement; return value;
    }
    Ticket Edit(std::uint32_t actor) {
        auto& value=actors[actor]; ++value.serial; return value;
    }
    bool Current(std::uint32_t actor, Ticket ticket) const {
        auto it=actors.find(actor); return it!=actors.end() && it->second.serial==ticket.serial;
    }
    bool SameReplacement(std::uint32_t actor, Ticket ticket) const {
        auto it=actors.find(actor); return it!=actors.end() && it->second.replacement==ticket.replacement;
    }
    void Clear() { actors.clear(); }
};
}
