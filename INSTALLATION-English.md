[中文说明](README.zh-CN.md) | [English](README.md) | [Downloads](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases)

# Tailor Devious Device Compatibility Patch 0.6.14 — Technical reference


**Corresponding source for this binary:** download Tailor-Devious-Device-Compatibility-Patch-0.6.14-Source.zip from [the exact matching release](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.14), available without charge alongside the FOMOD. On Nexus, offer that matching Source.zip in Optional Files or clearly link this exact source download next to the main binary file. The source includes build files and pinned dependency source.


Try supported Devious Devices clothing freely in Tailor. After confirming an outfit, keep genuine DD sounds, animations and other effects, and choose whether normal clothing locks remain active. This is an independent add-on: it does not replace Tailor.dll or include anyone else's translation.

**Tailor author permission:** ColdSun has granted permission to publish this compatibility patch. See the [permission record](AUTHOR-PERMISSIONS.md) and [permission screenshot](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/PERMISSIONS.png). The screenshot is included in Source.zip and the repository, but not in the FOMOD installer.

## Requirements

Install these separately, together with their own requirements:

- [Tailor 3.0.1](https://www.nexusmods.com/skyrimspecialedition/mods/173691), one of the exact supported builds below.
- Skyrim SE/AE, matching [SKSE64](https://skse.silverlock.org/), [Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444), and the Microsoft Visual C++ x64 runtime required by native SKSE plugins. The reported working setup uses Skyrim 1.6.1170; other executables and VR are not validated.
- Devious Devices and all requirements for your DD equipment, with **DD NG v0.4.3 rc2, the exact DLL below**. The [DD NG project page](https://www.loverslab.com/files/file/29779-devious-devices-ng/) is a project reference, not a guarantee that its current download matches this reviewed build. Obtain the matching build from the author's distribution. A similarly named version is not sufficient.
- [PapyrusUtil](https://www.nexusmods.com/skyrimspecialedition/mods/13048) matching your game/SKSE. Its StorageUtil functions are required by the independent helper.
- **Open Animation Replacer:** required for the fitting-preview conditions; locally tested with **3.1.6**, using public Conditions interface **V3 (numeric API value 2)**. Install it and its requirements separately.
- **Optional Partial Animation Replacer adapter:** supports the exact **PAR 0.2.0** DLL supplied with the reviewed DD NG setup, as listed below. PAR is not a separate requirement of this patch. Unknown builds retain their original behavior without this adapter.
- **Optional Mu integration:** [Mu Dynamic NPC Replacer 2.1.4](https://www.nexusmods.com/skyrimspecialedition/mods/150931), exact reviewed DLL below, and RaceMenu as required by Mu. Mu is not required for ordinary use. The appearance adapter is disabled for unreviewed Mu DLLs.
- **Optional Tailor Chinese translation:** [supported CHS translation](https://www.nexusmods.com/skyrimspecialedition/mods/184318), downloaded separately. Chinese add-on labels use Tailor's Chinese font coverage. English users do not need a translation.

| Supported binary | SHA-256 |
| --- | --- |
| Tailor 3.0.1 English, Tailor.dll | `261f415773d550e16809cbdfa39863451ea3eeba247e30b996afabe1e1d47850` |
| Tailor 3.0.1 supported CHS, Tailor.dll | `83dc9c56627d163e00eb9e5f1cceb1af3edb84deaa286ec828c95fb2a0d0df4d` |
| DD NG v0.4.3 rc2, DeviousDevices.dll | `f68147a58cdffa4c4bd9c69844c073a8a274abe30357a76402e6e5b78ffc69b4` |
| Optional PAR 0.2.0, PartialAnimationReplacer.dll | `fa07d8355ee075c4d42a02a7c426524e88d1c027998b0446660c58b768b8030a` |
| Optional Mu 2.1.4, MuDynamicNPCReplacer.dll | `009113664ae10e406914bc6fa8a394290aaa03cfd68367dce154e84543c7da0a` |

These are deliberately narrow native compatibility checks. Unsupported builds are rejected; do not bypass the checks to try a different binary.

## Installation in MO2

1. Close Skyrim. Install Tailor, DD/DD NG, PapyrusUtil, Open Animation Replacer and their requirements. Install the supported Tailor translation separately if desired.
2. Install **Tailor-Devious-Device-Compatibility-Patch-0.6.14-FOMOD.zip** as one mod, choose 中文 or English, and enable the bundled **TailorDDAutoFitting.esp** in the plugin list. The language choice affects only this add-on.
3. Enable only one patch version; disable old independent bridge language mods and temporary startup diagnostics. Keep old matching Generated Data if already used by saves/outfits.
4. Launch normally through MO2/SKSE. Supported winning inventory/render-device links and loose/BSA script inheritance are scanned at data loading. No Python, external generator or initial enter/exit cycle is required.
5. Open Tailor → Settings → DD Settings. The default remains Off.

Left-pane order: required mods (including DD NG and OAR) → Tailor → optional separately obtained Tailor translation → this patch. New users do not need Generated Data; the legacy-data note below applies only to existing saves/outfits that used it. Tailor.dll and other authors' scripts/translations are not replaced.

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

0.6.14 retains automatic fitting setup, the DD removal-token fix, appearance restoration and the default-Off switch. The changes since 0.6.6 improve pose selection and cleanup during fitting, discard superseded outfit requests, and keep the selected Pet preview from being replaced by automatic idle selection. The optional PAR adapter rechecks its own pose conditions only for the current fitting NPC and skips stale overrides after a restraint is removed. Valid cached poses and actors outside that scope follow the original handling.

The bundled OAR JSON selects preview animations already provided by the user's DD installation; no DD animation files are redistributed. Normal DD animations and sounds remain enabled after fitting. Tailor's own cross-save outfit behavior remains unchanged.

In the 2026-10-21 local test, the user reported normal transitions from a restraint pose to ordinary clothing and then to Pet gear. Logs show at least 64 stale PAR overrides skipped, five expected Pet animation snapshots, five automatic idle requests handled, and 767/767 Pet condition matches over about 16 seconds. The old outfit request was discarded, the observed equip/removal callbacks succeeded, and preview scopes cleared on exit. The 64 messages are a logging cap, not a count of all checks. See [VALIDATION-0.6.14.md](VALIDATION-0.6.14.md) for the tested DLL hash and limits; this is not a full equipment, actor, game-version or language compatibility matrix.

## Contents, source and credits

Credit to ersh (ersh1) and Open Animation Replacer contributors for the public Conditions API source. Its original COPYING and EXCEPTIONS texts are retained in the source and installation license notices.

Credit to exiledviper (continued maintenance/refactoring) and meh321 (original version and idea) for [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048). Our independent helper calls its StorageUtil functions. PapyrusUtil is a separately installed runtime requirement; its DLL and implementation scripts are not included.

Included: independent patch DLL and helper PEX, bilingual FOMOD, automatic fitting plugin, fitting-preview OAR condition JSON, bilingual instructions and license notices. The JSON references installed DD assets; no DD animations or PAR configuration assets are included. Excluded: Tailor runtime/assets/fonts, third-party translations, DD/Mu/RaceMenu implementations or assets, personal presets/generated equipment data, saves, logs, and temporary startup diagnostic tools.

Credit to ColdSun / heathbrownkeyworks for Tailor and its API, the DD/DD NG developers, Acro for Mu, expired6978 for RaceMenu, and Llinnn for the separately distributed Tailor CHS translation. Their mods must be obtained independently. This is an unofficial compatibility add-on.

The **Source.zip** and [GitHub repository](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch) contain corresponding source and build instructions. Do not install Source.zip in MO2. The patch is GPL-3.0-or-later; see LICENSE and THIRD_PARTY_NOTICES.txt.

Source delivery: see SOURCE-ACCESS.md and dependency-sources/README.md in the complete Source.zip for the actual 8 resolved dependencies, source SHA-256 values, recorded recipes/configurations and configured-header copies. The complete source also includes the public OAR Conditions API and its notices. Release preparation retains the exact game-tested 0.6.14 DLL and helper PEX without rebuilding.

Full attribution and permission record: **CREDITS.md**, **PREREQUISITE-NOTICES.md**, **RESOURCE-AUDIT.json** (included in the installer/source). Includes SKSE team, meh321/Address Library, DD/NG authors, PapyrusUtil authors, optional integrations, linked libraries and embedded code. Permission to publish this compatibility patch has been granted by Tailor author ColdSun; see [AUTHOR-PERMISSIONS.md](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/AUTHOR-PERMISSIONS.md).
