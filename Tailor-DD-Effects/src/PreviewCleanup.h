#pragma once
#include <memory>

namespace RE { class Actor; class BGSOutfit; }

// GPL-3.0-or-later. Ownership and clone-identity guards adapted from ColdSun's
// Tailor PreviewOutfitCleanup.cpp; see AUTHOR-PERMISSIONS.md and Tailor notices.
namespace PreviewCleanup {
struct State;
using Snapshot = std::shared_ptr<State>;

// Call on the game thread immediately before Tailor SetActorDefaultOutfit.
// Only outgoing dynamic outfits previously observed as successfully applied by
// this hook are eligible. The first observation is diagnostic-only.
Snapshot Capture(RE::Actor* actor, RE::BGSOutfit* incoming);

// Call on the game thread immediately after the original call, with its result.
// Records successful incoming runtime-outfit ownership, inspects immediately,
// then rechecks at approximately 250 and 1000 ms through the SKSE task queue.
// A clone still referenced by ANY biped slot is diagnostic-only; this helper
// never clears biped metadata, changes worn flags, or removes inventory items.
void Finish(const Snapshot& state, RE::Actor* actor, bool applied);

// Required at game load/new game/revert/shutdown before runtime forms can be
// reused. Invalidates delayed work and all remembered runtime-outfit ownership.
void Clear();
}
