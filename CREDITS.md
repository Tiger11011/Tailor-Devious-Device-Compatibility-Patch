# Credits and resource provenance / 鸣谢与资源来源

Checked 2026-10-18 for functional version 0.6.4. This record distinguishes
separately installed prerequisites from code compiled into the patch and files
actually distributed. Credit is not a claim of endorsement, permission to
repackage a prerequisite, or a replacement for its license.

## External game and mod prerequisites

| Project and creator | Actual use in this project | Distribution / licensing boundary |
| --- | --- | --- |
| [Tailor](https://github.com/heathbrownkeyworks/Tailor), **ColdSun / heathbrownkeyworks** | `RequestTailorInterface`, `IsTailorOpen`, settings/outfit runtime hooks; Tailor-derived interface declarations/comments | Tailor.dll/assets are not bundled. Tailor GPL-3.0-or-later, exceptions, LICENSING.md and API MIT notices are retained; MIT does not cover all Tailor internals. |
| **Devious Devices team**: **Kimy**, **Colycon**; **Zarantha** for the SE version; **Min** and **Zadil** as founders, plus the contributors credited in DD's own README | DD `zadlibs.LockDevice/UnlockDevice`, quest/keywords and device inventory/render-record relationships | No original DD scripts, ESP/ESM, models, textures, animations or sounds are bundled. DD 5.2 has custom permissions, including an explicit patch-publication restriction; see PREREQUISITE-NOTICES.md. |
| [Devious Devices NG](https://github.com/IHateMyKite/DeviousDevicesNG), **ponzipyramid** and **IHateMyKite/DeviousDevicesNG contributors** | Supported DeviousDevices.dll hooks and `zadNativeFunctions.GetInventoryObjectPropertyObject`; exact runtime fingerprints | Original DLL/scripts are not bundled. The reviewed NG plugin source LICENSE is Apache-2.0, Copyright 2022 ponzipyramid. That does not relicense all DD scripts/assets or settle DD 5.2's patch-publication terms. |
| [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048), **exiledviper** (continued maintenance/refactoring), **meh321** (original version and idea) | Helper calls `StorageUtil.GetIntValue` to observe DD-owned removal tokens (0.6.6 no longer writes or clears them) | Separately installed runtime prerequisite. No PapyrusUtil DLL or implementation scripts are bundled; source has minimal compile-only declarations. No blanket redistribution license is inferred. |
| [SKSE64](https://skse.silverlock.org/), **Ian Patterson, Stephen Abel, Paul Connelly, Brendan Borthwick** (ianpatt, behippo, scruggsywuggsy the ferret, purple lunchbox) and contributors | Native plugin loader/interfaces, Papyrus registration/calls, tasks, serialization and NiNode update events | Runtime installed separately. CommonLib provides compiled interface implementation in our build; original SKSE loader/runtime is not bundled. |
| [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444), **meh321** | CommonLib `REL` game-runtime address resolution | Database .bin files are obtained separately, not bundled. Exact matching runtime database is required; no database redistribution is performed. |
| [Mu Dynamic NPC Replacer / MDNR](https://www.nexusmods.com/skyrimspecialedition/mods/150931), **Acro / Acro748** | Optional, hash-guarded current-preset lookup and appearance-update interoperability | Optional reviewed Mu 2.1.4 only. No Mu implementation, configuration, preset or asset is bundled; no general asset-use permission is claimed. |
| [RaceMenu](https://www.nexusmods.com/skyrimspecialedition/mods/19080), **Expired / expired6978**, with **PurpleLunchbox** credited on its project page | Optional Mu restoration uses `CharGen.LoadCharacterEx`; existing exported preset/tint resources are read from the user's installation | No RaceMenu implementation, exported presets or face textures are bundled. Its page prohibits redistribution of original files. Only our minimal compile-only CharGen declaration is in source. |
| [Tailor CHS translation](https://www.nexusmods.com/skyrimspecialedition/mods/184318), **Llinnn** | Optional, separately installed supported translated Tailor runtime | No translator's text, DLL or font is repackaged. New patch labels are our own bilingual text. Compatibility fingerprints are short validation data, not a translated runtime. |
| **Bethesda Game Studios / Bethesda Softworks**, Skyrim SE/AE and Creation Kit | Game runtime and Papyrus compiler/native API; source contains minimal compile-only game signatures and flag declarations | Game, compiler, original script implementations and game assets are not redistributed. Their terms remain separate. |

For DD's current distribution and script API, follow its own official release
and documentation: https://github.com/DeviousDevices/Docs/wiki . Obtain DD NG
from its author's distribution; current source is a reference, not proof that
an arbitrary current/nightly build matches the supported rc2 DLL.

## Libraries compiled or incorporated into the patch

| Component | Attribution | License / included notice |
| --- | --- | --- |
| CommonLibSSE-NG, commit `8b032fa992750d654d6d38a33731714d8b86be1f` | **Ryan-rsm-McKenzie**, **CharmedBaryon**, **alandtse** and CommonLib contributors | GPL-3.0-or-later with retained exceptions; `CommonLibSSE-NG-GPL.txt`, `CommonLibSSE-NG-EXCEPTIONS.md`; historical Ryan MIT attribution retained separately in `CommonLibSSE-NG-legacy-MIT.txt`, not a relicense of the current whole library. |
| Dear ImGui 1.92.6 | **Omar Cornut** and contributors | MIT: `Dear-ImGui-MIT.txt`. |
| ImGui embedded STB code | **Sean Barrett** and contributors | Original MIT/public-domain alternatives preserved in `Dear-ImGui-embedded-STB.txt`; original sources remain in the pinned archive. |
| ProggyClean embedded default font | **Tristan Grimmer** | MIT / Copyright (c) 2004, 2005; `ProggyClean-MIT.txt`. The complete 9583-byte compressed font payload is present in the accepted DLL through ImGui. |
| FreeType 2.14.1 | **David Turner, Robert Wilhelm, Werner Lemberg** and contributors | FreeType License selected; `FreeType-FTL.txt`, plus `FreeType-third-party-notices.txt` for BDF/PCF and HarfBuzz-derived portions. Portions of this software are copyright © 1996–2025 The FreeType Project. All rights reserved. |
| zlib 1.3.2 | **Jean-loup Gailly, Mark Adler** and contributors | zlib license: `zlib-LICENSE.txt`. |
| MinHook 1.3.4 and HDE | **Tsuda Kageyu**; HDE **Vyacheslav Patkov** | BSD-2-Clause; `MinHook-BSD.txt` retains both HDE32 and HDE64 notices. |
| nlohmann/json 3.12.0 | **Niels Lohmann** and contributors | MIT: `nlohmann-json-MIT.txt`. |
| JSON embedded UTF-8 decoder / Grisu2 / Hedley | **Björn Hoehrmann**, **Florian Loitsch**, **Evan Nemerson** | Decoder/Grisu2 MIT attributions and full terms in `nlohmann-json-embedded-MIT.txt`; original license summary in `nlohmann-json-third-party.md`; Hedley CC0-1.0 text is included in `Hedley-CC0-1.0.txt`. |
| spdlog 1.16.0 | **Gabi Melman** and contributors | MIT: `spdlog-MIT.txt`; retained configuration uses `std::format`, not the optional bundled fmt implementation. |
| DirectXTK feb2024 and DirectXMath feb2024 | **Microsoft Corporation** and contributors | MIT: `DirectXTK-MIT.txt`, `DirectXMath-MIT.txt`. |
| rapidcsv 8.92 | **Kristofer Berggren** and contributors | BSD-3-Clause: `rapidcsv-BSD.txt`. |
| OpenVR, commit `60eb187801956ad277f1cae6680e3a410ee0873b` | **Valve Corporation** and contributors | Valve BSD license: `OpenVR-BSD.txt`; CommonLib dependency source is retained. This is not a claim that this patch is tested in Skyrim VR. |

License filenames above refer to `licenses/` in the installed patch, or
`Tailor-DD-Effects/licenses/` plus the historical shared license directory in
source. Existing shared notices are installed by the FOMOD together with the
new notices. Exact dependency sources, package configurations and recipe
patches are described in `dependency-sources/README.md`.

## Installer, generated data and development resources

- **Installer artwork/text:** the two-line language prompt is generated by our
  `Make-LanguagePrompt.ps1` script using Windows System.Drawing. **Microsoft
  YaHei** and **Segoe UI** render fixed sentences into a PNG; no font files,
  glyph atlas, Tailor artwork or translation screenshot is bundled. Microsoft's
  [font FAQ](https://learn.microsoft.com/en-us/typography/fonts/font-faq) permits
  graphics containing fixed words/phrases under its stated conditions; that
  does not permit redistributing Windows fonts or converting them into a font.
- **In-game fonts:** our settings hook uses Tailor's existing font objects; it
  does not embed/copy Tailor's fonts. Their supply and terms remain with Tailor.
- **Generated equipment data:** our generator reads the user's enabled winning
  plugin records and PEX inheritance, then creates local appearance records.
  It retains appearance fields, item names and references into the original
  clothing mods. Those providers' models/textures/animations/sounds are not
  copied into this release. Their authors retain their rights; the generated
  plugin is personal profile data, not a grant to upload their records/assets.
  No particular third-party clothing/body/preset pack is a new universal
  prerequisite of this patch.
- **Mod Organizer 2:** credit **Tannin and the Mod Organizer development team**;
  the generator reads MO2's saved profile/configuration formats. MO2 binaries
  or implementation are not bundled. Project: https://github.com/ModOrganizer2/modorganizer .
- **Python:** credit the **Python Software Foundation** and contributors for
  the standard-library tools used by the generator/packager. No Python
  interpreter is bundled. Windows APIs/SDK, Visual Studio/MSVC, PowerShell and
  .NET System.Drawing are Microsoft development/runtime resources, not copied
  game assets. xmake is by **Ruki Wang** and contributors; xmake-repo build
  recipes retain their Apache-2.0 notice in `dependency-sources/recipes/LICENSE.md`.
  CMake, Meson, Ninja and pkgconf are general-purpose build tools, not linked
  game plugins; see `dependency-sources/BUILD-TOOLS.json` for evidence limits.
- **Analysis reference:** **Orvid / Champollion contributors**,
  https://github.com/Orvid/Champollion . The generator's independent PEX metadata
  parser identifies its format cross-check against Champollion's FileReader.
  No Champollion implementation/binary is distributed or linked. Ghidra, by
  the **National Security Agency and contributors**, was a development analysis
  tool; no Ghidra code or decompiled third-party implementation is packaged.

## 中文说明

PapyrusUtil、SKSE、Address Library、DD/DD NG 是按实际调用或构建接口核对的前置；
Mu/RaceMenu、Tailor 汉化属于相应功能的可选集成。上表记录了作者、用途和许可边界。
所有随包编译或内嵌的库均保留各自许可，不能统一改称本补丁的 GPL。

DD、Mu、Tailor 本身可能要求其他模组，例如动画/身体/骨架、SexLab/Aroused、
SkyUI、OAR、Papyrus Tweaks NG、MCM Helper 等；以各自所安装发行版的原始前置
清单为准。这些不是本补丁新增的直接调用，也没有被打包进补丁。不应把本地 DD
5.2 README 中的旧 LE 链接直接当成当前 SE/NG 安装指南。

**鸣谢已补充不等于已经获得额外书面许可。**维护者确认本项目为纯兼容 bug 修复，
据 DD 5.2 原条款的 bug 修复例外处理；详细依据和解释边界见
`PREREQUISITE-NOTICES.md`，Nexus 平台规则仍独立适用。

Tailor author permission / Tailor 作者许可（记录于 2026-10-18）：维护者确认已取得 ColdSun 对发布本兼容补丁的许可。ColdSun has granted permission to publish this compatibility patch, as confirmed by the maintainer. See AUTHOR-PERMISSIONS.md. Original correspondence is not included; other component rights and notices remain unchanged.
