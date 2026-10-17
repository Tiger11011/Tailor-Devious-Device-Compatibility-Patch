#pragma once
#include <cstdint>
namespace Equipment::Policy {
constexpr bool UseReal(bool hasActor, bool previewOpen, int requestedOutfit, int approvedOutfit,
    std::uint64_t requestedPieces, std::uint64_t approvedPieces) {
    return hasActor && !previewOpen && requestedOutfit > 0 && requestedOutfit == approvedOutfit && requestedPieces == approvedPieces;
}
constexpr bool Handoff(bool confirmed, bool userClose, bool previewOpen, bool actorReady,
    std::uint64_t requestedGeneration, std::uint64_t currentGeneration) {
    return confirmed && userClose && !previewOpen && actorReady && requestedGeneration == currentGeneration;
}
}
