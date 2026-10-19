[中文说明](README.zh-CN.md) | [English](README.md) | [Downloads](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases)

# Tailor Devious Device Compatibility Patch 0.6.6 — Technical reference / 详细技术说明


**Corresponding source for this binary:** download Tailor-Devious-Device-Compatibility-Patch-0.6.6-credits.2-Source.zip from [the exact matching release](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.6-credits.2), available without charge alongside the FOMOD. On Nexus, offer that matching Source.zip in Optional Files or clearly link this exact source download next to the main binary file. The source includes build files and pinned dependency source.


Try supported Devious Devices clothing freely in Tailor. After confirming an outfit, keep genuine DD sounds, animations and other effects, and choose whether normal clothing locks remain active. This is an independent add-on: it does not replace Tailor.dll or include anyone else's translation.

**Tailor author permission:** ColdSun has granted permission to publish this compatibility patch. See the [permission record](AUTHOR-PERMISSIONS.md) and [permission screenshot](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/PERMISSIONS.png). The screenshot is included in Source.zip and the repository, but not in the FOMOD installer.

## Requirements

Install these separately, together with their own requirements:

- [Tailor 3.0.1](https://www.nexusmods.com/skyrimspecialedition/mods/173691), one of the exact supported builds below.
- Skyrim SE/AE, matching [SKSE64](https://skse.silverlock.org/), [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444), and the Microsoft Visual C++ x64 runtime required by native SKSE plugins. The reported working setup uses Skyrim 1.6.1170; other executables and VR are not validated.
- Devious Devices and all requirements for your DD equipment, with **DD NG v0.4.3 rc2, the exact DLL below**. The [DD NG project page](https://www.loverslab.com/files/file/29779-devious-devices-ng/) is a project reference, not a guarantee that its current download matches this reviewed build. Obtain the matching build from the author's distribution. A similarly named version is not sufficient.
- [PapyrusUtil](https://www.nexusmods.com/skyrimspecialedition/mods/13048) matching your game/SKSE. Its StorageUtil functions are required by the independent helper.
- **Optional Mu integration:** [Mu Dynamic NPC Replacer 2.1.4](https://www.nexusmods.com/skyrimspecialedition/mods/150931), exact reviewed DLL below, and RaceMenu as required by Mu. Mu is not required for ordinary use. The appearance adapter is disabled for unreviewed Mu DLLs.
- **Optional Tailor Chinese translation:** [supported CHS translation](https://www.nexusmods.com/skyrimspecialedition/mods/184318), downloaded separately. Chinese add-on labels use Tailor's Chinese font coverage. English users do not need a translation.

| Supported binary | SHA-256 |
| --- | --- |
| Tailor 3.0.1 English, Tailor.dll | `261f415773d550e16809cbdfa39863451ea3eeba247e30b996afabe1e1d47850` |
| Tailor 3.0.1 supported CHS, Tailor.dll | `83dc9c56627d163e00eb9e5f1cceb1af3edb84deaa286ec828c95fb2a0d0df4d` |
| DD NG v0.4.3 rc2, DeviousDevices.dll | `f68147a58cdffa4c4bd9c69844c073a8a274abe30357a76402e6e5b78ffc69b4` |
| Optional Mu 2.1.4, MuDynamicNPCReplacer.dll | `009113664ae10e406914bc6fa8a394290aaa03cfd68367dce154e84543c7da0a` |

These are deliberately narrow native compatibility checks. Unsupported builds are rejected; do not bypass the checks to try a different binary.

## Installation in MO2

1. Close Skyrim. Install Tailor, DD/DD NG, PapyrusUtil and their requirements. Install the supported Tailor translation separately if desired.
2. Install **Tailor-Devious-Device-Compatibility-Patch-0.6.6-credits.2-FOMOD.zip** as one mod, choose 中文 or English, and enable the bundled **TailorDDAutoFitting.esp** in the plugin list. The language choice affects only this add-on.
3. Enable only one patch version; disable old independent bridge language mods and temporary startup diagnostics. Keep old matching Generated Data if already used by saves/outfits.
4. Launch normally through MO2/SKSE. Supported winning inventory/render-device links and loose/BSA script inheritance are scanned at data loading. No Python, external generator or initial enter/exit cycle is required.
5. Open Tailor → Settings → DD Settings. The default remains Off.

Left-pane order: Tailor → optional separately obtained Tailor translation → this patch → existing legacy Generated Data, if any. New users do not need Generated Data. Tailor.dll and other authors' scripts/translations are not replaced.

## What the switch does

- **While fitting:** supported devices use removable appearance copies. Confirm the intended assignment with Tailor's **Set** workflow and close normally; adding or previewing a piece alone does not confirm an outfit.
- **Off — default:** after confirmation and normal exit, genuine DD items retain their original effects. The patch grants the managed actor/device pair permission to change or remove that clothing through Tailor and the normal inventory.
- **On:** after confirmation and normal exit, genuine DD equipment uses normal DD clothing restrictions. DD's original conflicts, scripts and locking rules still apply.
- To change how an already assigned outfit is handled, change the switch and confirm that outfit again. Turning Off is not a global unlock command for all existing devices.
- This patch only changes clothing restrictions for supported, patch-managed generic devices. It does not disable muffling, sound, animations or other DD effects. It does not forcibly unlock quest devices, block-generic devices, unsupported custom scripts, or items locked independently by other gameplay.

## Generated Data and existing saves

Equipment is recognized from loaded records and verified DD script inheritance, not clothing names. Unsupported/custom devices and quest/block-generic restrictions remain excluded. Installing additional supported clothes requires only a normal game restart; automatic IDs remain stable.

Back up `Documents\My Games\Skyrim Special Edition\SKSE\TailorDDBridge.AutomaticSlots.json` together with saves. This append-only identity ledger is not a log: do not delete it, compact/renumber the full ESP, or reuse IDs of removed clothes. The pool provides 65,536 identities and uses a normal plugin slot.

Existing schema-2 TailorDDCosmetics.esp and its matching TailorDDSafeFitting.json remain supported. Keep both when old saves/outfits use them; do not regenerate or delete them merely to upgrade. New users need only the bundled automatic plugin. Legacy generator sources remain for reference; normal installation no longer runs them.

Tailor's shared outfit assignments across saves are retained. Patch free-wear permissions still use each SKSE co-save. This revision does not clear assignments or force-remove saved DD tokens/locks. If a failed older build left a token in a save, test using a save from before that failure. Back up saves before updating; uninstalling a DLL does not remove devices or undo their locks.

## Changes and validation

0.6.6 adds automatic fitting setup, fixes the helper-created DD removal-token leak, refuses relocking while DD removal is pending, and sends Mu update events only when the geometry set changes. Superseded appearance check chains no longer run. Existing face restoration, DD effects and the default-Off switch remain.

The 2026-10-19 local test loaded 2,442 supported pairs: all 11 removal and 14 equip callbacks succeeded, including the previously failing DD fallback branch; no residual-token refusal or timeout was recorded. Two selected-face restores succeeded. The user reported stable appearance; node-gated notification logic and the loaded binary were checked. See VALIDATION-0.6.6.md for evidence limits. This is not a full game/device/language compatibility matrix.

## Contents, source and credits

Credit to exiledviper (continued maintenance/refactoring) and meh321 (original version and idea) for [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048). Our independent helper calls its StorageUtil functions. PapyrusUtil is a separately installed runtime requirement; its DLL and implementation scripts are not included.

Included: independent patch DLL and helper PEX, bilingual FOMOD, automatic fitting plugin, bilingual instructions and license notices. Excluded: Tailor runtime/assets/fonts, third-party translations, DD/Mu/RaceMenu implementations or assets, personal presets/generated equipment data, saves, logs, and temporary startup diagnostic tools.

Credit to ColdSun / heathbrownkeyworks for Tailor and its API, the DD/DD NG developers, Acro for Mu, expired6978 for RaceMenu, and Llinnn for the separately distributed Tailor CHS translation. Their mods must be obtained independently. This is an unofficial compatibility add-on.

The **Source.zip** and [GitHub repository](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch) contain corresponding source and build instructions. Do not install Source.zip in MO2. The patch is GPL-3.0-or-later; see LICENSE and THIRD_PARTY_NOTICES.txt.

源码交付 / Source delivery: see SOURCE-ACCESS.md and dependency-sources/README.md in the complete Source.zip for the actual 8 resolved dependencies, source SHA-256 values, recorded recipes/configurations and configured-header copies. Release preparation retains the exact game-tested 0.6.6 DLL/PEX without rebuilding.

完整作者与许可清单 / Full attribution and permission record: **CREDITS.md**, **PREREQUISITE-NOTICES.md**, **RESOURCE-AUDIT.json** (included in the installer/source). Includes SKSE team, meh321/Address Library, DD/NG authors, PapyrusUtil authors, optional integrations, linked libraries and embedded code. Permission to publish this compatibility patch has been granted by Tailor author ColdSun; see [AUTHOR-PERMISSIONS.md](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/AUTHOR-PERMISSIONS.md).
