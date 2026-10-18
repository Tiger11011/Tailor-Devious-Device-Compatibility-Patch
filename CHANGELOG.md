# Changelog

## 0.6.4-permissions.2 — 2026-10-18

- Refresh bilingual README, permission documentation and log/reporting instructions.
- Include the uploaded permission screenshot in Source.zip only; omit it from the FOMOD.
- Retain the tested DLL/PEX, functional source, language choices and default Off setting unchanged.
- Nexus descriptions are maintained separately by the maintainer; no Nexus description changes in this revision.

## 0.6.4-permissions.1 — 2026-10-18

- Record the maintainer's confirmation of ColdSun's permission to publish this Tailor compatibility patch.
- Add AUTHOR-PERMISSIONS.md to the repository, both installer language choices and corresponding source; synchronize bilingual README/Nexus descriptions and permission records.
- No original author message, grant date or unspecified conditions are invented. Existing licenses and all tested runtime/functional files remain unchanged.

## 0.6.4-sources.4 — 2026-10-18

- Final notice/source packaging revision: all 29 reviewed component/resource credits and notices retained.
- Restrict vendored CommonLib/OpenVR sources to actual build/implementation/header/license files; omit unrelated SDK/Unity/SDL examples, test address databases and debug binaries. Add a per-file source hash inventory.
- Accepted DLL, helper PEX, functional inputs and FOMOD behavior remain byte-identical.

## 0.6.4-sources.3 — 2026-10-18

- Complete the resource audit at 29 components; retain ProggyClean's Tristan Grimmer copyright and MIT license after confirming its existing compressed payload in the accepted DLL.
- Includes the complete prerequisite/embedded-code notices prepared in sources.2. Runtime and functional source remain byte-identical.

## 0.6.4-sources.2 — 2026-10-18

- Correct the omitted PapyrusUtil SE credits: exiledviper (maintenance/refactoring), meh321 (original version/idea).
- Explain the helper's StorageUtil use and separately installed prerequisite; PapyrusUtil implementation is not redistributed.
- Documentation/credit correction only; tested runtime, functional source, settings and language choices remain unchanged.
- Extend the review to 28 direct/runtime/library/resource/reference components; add full CREDITS, prerequisite permission evidence and machine-readable resource audit.
- Retain CommonLib legacy MIT, ImGui/STB, JSON embedded MIT/CC0, FreeType contributed-code notices and DD/NG license references. Record reliance on DD's bug-fix exception without claiming separate author approval.

## 0.6.4-sources.1 — 2026-10-18

- Source/notice packaging revision only; tested DLL, helper PEX, C++/Papyrus/generator sources, settings and FOMOD language choices are unchanged.
- Bundle eight exact upstream dependency source archives with SHA-256 values, retained xmake recipes/patches, sanitized package manifests and five installed configured-header copies.
- Record actual linked FreeType 2.14.1 and zlib 1.3.2; add the missing zlib license notice.
- Add SOURCE-ACCESS.md and a developer-only source retrieval/verification tool. Preserve earlier tags and binary assets.

## 0.6.4-licenses.1 — 2026-10-18

- License-only packaging revision; runtime remains the tested 0.6.4.
- Add ColdSun's copyright attribution and verbatim Tailor GPL, licensing, exception, API MIT and third-party notices from upstream commit a6088e2, with source URLs and hashes.
- Include COPYRIGHT.md in both FOMOD language installations and clarify corresponding-source download directions.
- Keep the accepted DLL, helper PEX, functional source, language choices and default Off byte-for-byte unchanged. Preserve the original v0.6.4 release/tag.

## 0.6.4

- Off retains genuine DD effects after confirmed exit while permitting supported managed clothing changes in Tailor and the normal inventory. On preserves normal DD restrictions.
- Wait for DD script binding, sequence managed equip/removal jobs and recover incomplete equip attempts through DD's normal API.
- Defer native equipment interception until needed and avoid initializing inventories during read-only checks.
- Optional reviewed Mu 2.1.4 adapter: correct preset paths, recheck after delayed DD removal, and recover previously hidden faces when visible.
- Observe face tint/shader changes and update the runtime version banner.
- Local tester confirmed DD switching and the Mu head-covering-to-visible-face appearance fix; full compatibility matrix remains unverified.
- Preserve Tailor shared NPC assignments, independent DLL, separate translation, bilingual FOMOD and default Off.
- Exclude temporary startup diagnostics and personal generated data from public downloads.

## 0.4 Experimental

- Independent SKSE bridge; does not replace Tailor.dll or third-party translations.
- Removable cosmetic copies while fitting supported DD equipment.
- Optional real-DD activation on confirmed exit; default Off.
- English/Chinese FOMOD language selection with a bilingual prompt.
- Centered DD Settings button label and shortened bilingual switch explanation.
- Preserves Tailor's normal shared NPC assignments across saves.
- Source and licenses included. Generated equipment data must be built locally.

Limited local testing; this is an experimental release. A complete runtime matrix and installer GUI verification are pending.
