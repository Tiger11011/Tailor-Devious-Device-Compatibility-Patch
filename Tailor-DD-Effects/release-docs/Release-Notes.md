# Tailor Devious Device Compatibility Patch 0.6.4

**Permission-record package revision: 0.6.4-permissions.1 (2026-10-18).** Comprehensive attribution/notice correction: PapyrusUtil, all direct runtime and optional integrations, inherited/embedded library notices, fixed-text installer artwork and development references. CREDITS.md, PREREQUISITE-NOTICES.md and RESOURCE-AUDIT.json are included. Existing source archives and earlier notices are retained. The DLL, helper PEX and functional source are unchanged from the locally accepted 0.6.4 build. The original release is retained.

Retains ColdSun's copyright notice and verbatim upstream license files pinned to a6088e2. Adds verified source archives for ImGui 1.92.6, FreeType 2.14.1, zlib 1.3.2, spdlog 1.16.0, JSON 3.12.0, DirectXTK feb2024, DirectXMath feb2024 and rapidcsv 8.92; records build recipes/configurations and five installed header differences. Adds the previously missing zlib license notice. No runtime or functional source was changed. See COPYRIGHT.md and licenses/LICENSE-SOURCES.json. The installer includes neither Tailor.dll nor any third-party translation.

For this exact binary, download **Tailor-Devious-Device-Compatibility-Patch-0.6.4-permissions.1-Source.zip** from this release alongside the FOMOD. Both are available without charge; source includes build files, eight verified upstream dependency archives, CommonLib/OpenVR/MinHook, package manifests, recipes and configured headers. See dependency-sources/README.md. The audit checks retained build evidence and source availability, not a fresh or bit-identical rebuild. When distributing the binary on Nexus, offer this matching Source.zip in Optional Files or link this exact GitHub release clearly next to the binary download.

Local testing confirmed DD clothing switching and Mu NPC appearance recovery when changing from fully covering DD headgear to face-visible equipment.

- **FOMOD.zip**: main MO2 installer; choose 中文 or English. Default Off.
- **Source.zip**: corresponding source and dependencies; not an MO2 installation.
- **SHA256SUMS.txt**: archive checksums.

Off retains genuine DD effects after Set and normal exit while permitting supported managed clothing changes through Tailor and the normal inventory. On preserves normal DD clothing locks. Previews remain removable appearance copies.

Fixes include DD script binding/order, incomplete equip handling, Mu/RaceMenu preset paths, and missed restoration after delayed removal or initially hidden heads. Tailor's original shared NPC assignments remain unchanged.

Requires the exact reviewed Tailor 3.0.1 and DD NG v0.4.3 rc2 builds, matching game/SKSE/Address Library, DD dependencies, PapyrusUtil and profile-specific Generated Data. Mu/RaceMenu are optional for the reviewed Mu integration. Check README hashes; this is not universal runtime or DD NG support.

Tailor, third-party translations, DD/Mu/RaceMenu implementations/assets, personal presets/generated data and startup diagnostic tools are not included. Existing matching schema-2 Generated Data can be retained; do not renumber it merely to update the patch.

**中文：**已获本地实测成功反馈。关闭时保留真实 DD 效果并允许受支持装备自由穿脱；开启保留原穿脱规则。FOMOD 自选补丁语言，Tailor 汉化请自行下载。更新保留匹配的 Generated Data，禁用旧独立中英文测试补丁，避免重复加载及残留临时诊断工具。

Offline checks passed. A complete device/player/NPC/runtime/language compatibility matrix is not claimed. See bilingual installation instructions.

Source distribution cleanup: remove unrelated upstream CommonLib unit-test databases, OpenVR/SDL/Unity samples, SDK/debug binaries and sample artwork. Retain required headers/implementation/build files and licenses at the recorded commits, with per-file hashes. See dependency-sources/VENDORED-SOURCE-SCOPE.md. This changes no runtime file.

## Tailor author permission update

The maintainer confirms receipt of ColdSun's permission to publish this compatibility patch. Recorded 2026-10-18; this is not an asserted original grant date. AUTHOR-PERMISSIONS.md is included in both FOMOD language installations and Source.zip. Original correspondence/unspecified conditions are not reproduced or invented. Existing component licenses remain unchanged. Runtime DLL/PEX, all functional inputs and FOMOD settings/choices are unchanged.
