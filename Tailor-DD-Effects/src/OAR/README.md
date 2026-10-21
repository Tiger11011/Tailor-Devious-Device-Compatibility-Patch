# Open Animation Replacer Conditions API provenance

The five `OpenAnimationReplacer*` API files in this directory are retained,
unmodified copies of ersh1/OpenAnimationReplacer's public Conditions API support
files, reviewed against the official primary sources on 2026-10-19. Their exact
SHA-256 values are recorded in `source-hashes.json`. The source snapshot was
already present locally at `_scratch/OpenAnimationReplacer-source/src/API`.
No commit identity is claimed for that local snapshot. The local full project
reports version 3.2.1; API negotiation is required at runtime and failure disables
only this patch's preview poses.

Upstream author: ersh (ersh1) and Open Animation Replacer contributors.
The original GPL version 3 license and upstream exceptions are retained verbatim
as `COPYING` and `EXCEPTIONS`. They are also included in the installation package
as `licenses/OpenAnimationReplacer-COPYING.txt` and
`licenses/OpenAnimationReplacer-EXCEPTIONS.txt`. No separate permissive license
for these API sources is asserted.

Primary source URLs:

- https://github.com/ersh1/OpenAnimationReplacer
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/src/API/OpenAnimationReplacerAPI-Conditions.h
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/src/API/OpenAnimationReplacerAPI-Conditions.cpp
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/src/API/OpenAnimationReplacer-ConditionTypes.h
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/src/API/OpenAnimationReplacer-ConditionTypes.cpp
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/src/API/OpenAnimationReplacer-SharedTypes.h
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/COPYING
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/EXCEPTIONS
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer-ExamplePlugin/main/src/main.cpp
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer-ExamplePlugin/main/src/Conditions.h
- https://raw.githubusercontent.com/ersh1/OpenAnimationReplacer/main/src/Parsing.cpp

`../OARConditionsAPI.cpp` compiles the two unmodified upstream implementation
files using this project's existing `src/*.cpp` build rule. The patch's own
conditions and numeric snapshot storage are in `../PreviewAnimations.cpp`.

## Integration and scope

Call `PreviewAnimations::Init()` during SKSE `kPostLoad`, as required by the
upstream API. Do not defer registration until `kDataLoaded`. This implementation
requests the documented Conditions V3 interface and does not call binary offsets
or PAR/OAR internals. A missing/rejected API or condition-name collision keeps all
preview conditions false and logs a diagnostic.

The installed OAR 3.1.6.0 DLL was inspected without execution: its exported
`RequestPluginAPI_Conditions` accepts enum values 1 and 2 (V2 and V3) and returns
the interface singleton. The raw DLL inspection and PDB are private validation
inputs, not redistributed source dependencies. The installed PDB also contains the
`GetComment`, `SetComment`, and `GetEssentialImpl` condition methods used by the
retained API headers. This verifies static interface availability; it does not
replace an in-game condition-registration/pose test.

Call `Set(actorID, renderedSources)` on the main thread only while that actor has
an active fitting session. Pass original DD rendered armor records corresponding
to cosmetic copies that are actually worn; do not pass selected-but-rejected or
delayed items. It reads eight exact DD keyword predicates to produce seven bits,
then retains only numeric actor IDs and bits. Animation-thread queries read only
that numeric map under a shared mutex. `Clear` and `ClearAll` revoke the snapshot
for session, actor, menu, and world transitions.

The package's own JSON-only OAR submods reuse installed DD NG `DD to OAR`
100001..100006 and 100300 folders through `overrideAnimationsFolder`. The patch
contains no DD HKX copies. Its predicates require `!IsChild` and the patch's own
session conditions. This does not grant DD keywords/scripts to cosmetic forms or
invoke DD control, perk, locking, or animation-evaluation functions.

The first implementation version used by the conditions/configs is 0.6.7.0;
the bridge plugin declaration must report that version or later.
The tested NPC's runtime pose transitions and sustained Pet preview were
corroborated by the 2026-10-21 user test and logs; see VALIDATION-0.6.14.md.
This does not establish a complete actor/device compatibility matrix.
PAR-only constraint variants without one of the seven classic OAR
classifications remain outside this OAR mapping.

## PAR interface finding

The reviewed public PAR Papyrus API exposes global `SetEnabled`, `Reload`, and
`Dump`; it has no supported per-actor temporary application API. The OAR
condition layer described above does not use those global controls or PAR
internals. Version 0.6.14 separately adds our narrowly scoped native PAR adapter
in `../PartialPoseCompatibility.cpp`: it requires the exact reviewed PAR 0.2.0
binary and calls its original condition evaluator for the active fitting NPC
before a cached bone override is applied. No PAR implementation or container
layout is copied. See CREDITS.md and the runtime installation instructions.

- https://raw.githubusercontent.com/IHateMyKite/PartialAnimationReplacer/main/src/Papyrus.h
- https://raw.githubusercontent.com/IHateMyKite/PartialAnimationReplacer/main/src/ReplacerManager.h
- https://raw.githubusercontent.com/IHateMyKite/PartialAnimationReplacer/main/src/XSEPlugin.cpp
