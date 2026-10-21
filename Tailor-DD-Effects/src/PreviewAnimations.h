#pragma once

#include <RE/Skyrim.h>
#include <vector>

// Cosmetic fitting poses with a scoped Pet automatic-idle guard. No DD
// keywords, scripts, controls, perks or behavior/animation assets are changed.
namespace PreviewAnimations {
    // Call only during SKSE kPostLoad, before OAR initializes its conditions.
    void Init();
    [[nodiscard]] bool Ready();

    // Main thread only. Sources must correspond to cosmetic armors actually worn
    // by this actor in the active fitting session. Source pointers are not retained.
    // An empty or unclassified list removes this actor's override.
    void Set(RE::FormID actorID, const std::vector<RE::TESObjectARMO*>& renderedSources);
    void Clear(RE::FormID actorID);
    void ClearAll();
    // Bounded read-only active-clip trace for the Pet preview fade investigation.
    void Trace(RE::Actor* actor);
}
