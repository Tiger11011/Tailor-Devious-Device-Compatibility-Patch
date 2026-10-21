#pragma once
#include <RE/Skyrim.h>

namespace PartialPoseCompatibility {
// The existing Tailor preview is single-target. Track ordinary outfits too:
// their removal of a real device must invalidate a stale PAR pose as well.
void SetPreview(RE::FormID actorID);
void Clear(RE::FormID actorID);
void ClearAll();
}
