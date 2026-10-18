[中文说明](README.zh-CN.md) | [English](README.md) | [Downloads](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases)

# Tailor Devious Device Compatibility Patch 0.6.4

**Source/notice package revision: 0.6.4-permissions.1, 2026-10-18.** Runtime behavior, DLL, helper PEX and functional source remain the tested 0.6.4. This revision completes dependency source delivery, records actual resolved build versions/configurations, and adds the zlib notice. Earlier ColdSun attribution and upstream notices are retained. See COPYRIGHT.md and licenses/LICENSE-SOURCES.json in the installer.

**Corresponding source for this binary:** download Tailor-Devious-Device-Compatibility-Patch-0.6.4-permissions.1-Source.zip from [the exact matching release](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.4-permissions.1), available without charge alongside the FOMOD. On Nexus, offer that matching Source.zip in Optional Files or clearly link this exact source download next to the main binary file. The source includes build files and pinned dependency source.


Try supported Devious Devices clothing freely in Tailor. After confirming an outfit, keep genuine DD sounds, animations and other effects, and choose whether normal clothing locks remain active. This is an independent add-on: it does not replace Tailor.dll or include anyone else's translation.

**Tailor author permission:** ColdSun has granted permission to publish this compatibility patch, as confirmed by the maintainer on 2026-10-18. See AUTHOR-PERMISSIONS.md; original correspondence is not included.

## Requirements

Install these separately, together with their own requirements:

- [Tailor 3.0.1](https://www.nexusmods.com/skyrimspecialedition/mods/173691), one of the exact supported builds below.
- Skyrim SE/AE, matching [SKSE64](https://skse.silverlock.org/), [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444), and the Microsoft Visual C++ x64 runtime required by native SKSE plugins. The reported working setup uses Skyrim 1.6.1170; other executables and VR are not validated.
- Devious Devices and all requirements for your DD equipment, with **DD NG v0.4.3 rc2, the exact DLL below**. The [DD NG project page](https://www.loverslab.com/files/file/29779-devious-devices-ng/) is a project reference, not a guarantee that its current download matches this reviewed build. Obtain the matching build from the author's distribution. A similarly named version is not sufficient.
- [PapyrusUtil](https://www.nexusmods.com/skyrimspecialedition/mods/13048) matching your game/SKSE. Its StorageUtil functions are required by the independent helper.
- Mod Organizer 2 and Python 3.10+ to generate equipment data for your own enabled profile. Python is only needed for generation; no pip packages are required.
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

1. Close Skyrim. Install the prerequisites and your equipment mods. Install Tailor's translation separately if desired, following its own conflict instructions.
2. Install **Tailor-Devious-Device-Compatibility-Patch-0.6.4-permissions.1-FOMOD.zip** as its own mod. Choose **中文** or **English**. The left image asks “你用什么语言？” and “What language do you use?”. This choice changes only the add-on language; it does not install a Tailor translation.
3. Disable older separate bridge language mods and any older experiment replacing Tailor.dll. Enable just one current patch installation. When upgrading the private test setup, disable the old Chinese/English bridge mods so their temporary startup diagnostics are not loaded alongside this release. **Do not disable or regenerate existing matching Generated Data merely to update the DLL.**
4. For a new installation, select your intended MO2 profile, enable its clothing mods/plugins and close MO2 so its lists are saved. Run the supplied generator from Windows, outside MO2's virtual filesystem, with your actual paths:

   ```powershell
   py -3 Tools/build_cosmetics.py --mo2 "D:/YourMO2" --profile "Default" --output "D:/TailorDDGenerated" --zip
   ```

5. Install the generated ZIP as a separate mod, enable it, and enable **TailorDDCosmetics.esp**. The output folder contains the generated ESP, matching JSON manifest and report; keep them together. Check the report for included and excluded devices.
6. Start through MO2/SKSE. Open Tailor with your configured hotkey (commonly Shift+Z), then **Settings → DD Settings**. The default is **Off**.

Recommended left-pane order:

```text
Tailor
Tailor translation (optional, obtained separately)
Tailor Devious Device Compatibility Patch 0.6.4
Tailor Devious Device Compatibility Patch - Generated Data
```

The patch installs its own `TailorDDBridge.dll` and `TailorDDFreeWear.pex`. Neither should be overwritten by an older patch. It does not overwrite Tailor.dll, Mu, DD scripts or translation files. The generated ESP is the patch-related entry needed in the right-pane plugin list.

## What the switch does

- **While fitting:** supported devices use removable appearance copies. Confirm the intended assignment with Tailor's **Set** workflow and close normally; adding or previewing a piece alone does not confirm an outfit.
- **Off — default:** after confirmation and normal exit, genuine DD items retain their original effects. The patch grants the managed actor/device pair permission to change or remove that clothing through Tailor and the normal inventory.
- **On:** after confirmation and normal exit, genuine DD equipment uses normal DD clothing restrictions. DD's original conflicts, scripts and locking rules still apply.
- To change how an already assigned outfit is handled, change the switch and confirm that outfit again. Turning Off is not a global unlock command for all existing devices.
- This patch only changes clothing restrictions for supported, patch-managed generic devices. It does not disable muffling, sound, animations or other DD effects. It does not forcibly unlock quest devices, block-generic devices, unsupported custom scripts, or items locked independently by other gameplay.

## Generated Data and existing saves

The generator recognizes verified winning loose-script DD inheritance and validated inventory/render-device links. It does not classify clothing by names such as “catsuit”. Third-party devices can qualify; a mod merely calling some DD functions is not automatically supported. BSA-only/unknown ancestry and excluded scripted devices require separate review.

Generate data again when the enabled source plugins, script winners or profile change. Reuse the **same output folder and manifest** to preserve assigned FormIDs, then update the generated MO2 mod. Keep different outputs for different mod setups. Do not distribute your personal generated ESP or manifest to other users.

Existing matching schema-2 Generated Data can be retained when updating from 0.4 or private 0.6.x builds. Cosmetic copies already assigned by 0.4 do not gain genuine DD effects automatically: open Tailor, set the switch, confirm that outfit again and exit normally. Other NPC assignments do not need to be cleared.

Tailor's original shared NPC outfit assignments across saves are preserved. This release does not isolate assignments by save or clear old assignments. The patch's free-wear permissions use its SKSE co-save; keep each save and co-save together. This does not change Tailor's own assignment storage.

Use a backup save for updates. Uninstalling the DLL does not remove real DD devices or undo their locks. Prefer returning to a save from before installing the patch/generated plugin when removing it. No save-cleaning or forced-unlock utility is included.

## Changes and validation

0.6.4 preserves genuine DD effects in free-wear mode, fixes incomplete-equip/removal handling, corrects the Mu/RaceMenu preset-path extension, and checks appearance again after asynchronous DD removal. A face initially hidden by headgear can be restored when visible again; tint/shader changes are now observed as well as model-node changes.

The local tester reported successful DD clothing switching and successful recovery of the Mu NPC appearance when changing from fully covering DD headgear to face-visible equipment. Offline native interface, operation-policy and preset-path checks passed. This is **not** a full player/NPC, device, language, runtime or mod-list compatibility matrix.

If reporting an issue, include game/SKSE/Tailor/DD NG versions, player or NPC, switch state, the exact confirm/exit sequence, equipment names and `Documents/My Games/Skyrim Special Edition/SKSE/TailorDDBridge.log`. For Mu appearance issues, also include the relevant RaceMenu/Mu logs. Review logs for personal data before posting.

## Contents, source and credits

Credit to exiledviper (continued maintenance/refactoring) and meh321 (original version and idea) for [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048). Our independent helper calls its StorageUtil functions. PapyrusUtil is a separately installed runtime requirement; its DLL and implementation scripts are not included.

Included: independent patch DLL and helper PEX, bilingual FOMOD, generator tools, bilingual instructions and license notices. Excluded: Tailor runtime/assets/fonts, third-party translations, DD/Mu/RaceMenu implementations or assets, personal presets/generated equipment data, saves, logs, and temporary startup diagnostic tools.

Credit to ColdSun / heathbrownkeyworks for Tailor and its API, the DD/DD NG developers, Acro for Mu, expired6978 for RaceMenu, and Llinnn for the separately distributed Tailor CHS translation. Their mods must be obtained independently. This is an unofficial compatibility add-on.

The **Source.zip** and [GitHub repository](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch) contain corresponding source and build instructions. Do not install Source.zip in MO2. The patch is GPL-3.0-or-later; see LICENSE and THIRD_PARTY_NOTICES.txt.

源码交付 / Source delivery: see SOURCE-ACCESS.md and dependency-sources/README.md in the complete Source.zip for the actual 8 resolved dependencies, source SHA-256 values, recorded recipes/configurations and configured-header copies. This packaging revision does not rebuild or alter the tested DLL/PEX.

完整作者与许可清单 / Full attribution and permission record: **CREDITS.md**, **PREREQUISITE-NOTICES.md**, **RESOURCE-AUDIT.json** (included in the installer/source). Includes SKSE team, meh321/Address Library, DD/NG authors, PapyrusUtil authors, optional integrations, linked libraries and embedded code; credits do not imply endorsement.
