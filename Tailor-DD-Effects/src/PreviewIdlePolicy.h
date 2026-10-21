#pragma once
#include <string_view>
namespace PreviewIdlePolicy {
constexpr bool BlockAutomaticReselection(bool petPreview,bool player) noexcept {
    // This policy is used only by PickNewIdleHandler. Explicit animation
    // requests and every other graph-event handler keep their normal route.
    return petPreview && !player;
}
constexpr bool Suppress(bool petPreview,bool player,std::string_view event) noexcept {
    // Exact ENAM events from Skyrim.esm IDLE 13A9D / DC8E8, corresponding
    // to the two competing HH clips captured in the 0.6.10 runtime trace.
    return petPreview && !player && (event=="IdleHHLeft" || event=="IdleHHRight");
}
}
