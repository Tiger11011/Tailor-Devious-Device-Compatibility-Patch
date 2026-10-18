[中文说明](README.zh-CN.md) | [English](README.md) | [Downloads](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases)

# Tailor Devious Device Compatibility Patch

Detailed installation, equipment-data generation and supported versions: [INSTALLATION-English.md](INSTALLATION-English.md)

## Description

This is a compatibility patch mod fixing bugs between “Devious Device” (DD) and "Tailor - An Outfit and Wig Manager". Allowing restraint gear from DD be used with “Tailor”. Please refer to this link for accurate details. [GitHub repository](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch)\

## Installation instructions

Throw it into your mod manager\

## Order (bottom has higher priority)

Tailor\
Tailor translation (optional, obtained separately)\
Tailor Devious Device Compatibility Patch\
Tailor Devious Device Compatibility Patch - Generated Data\

## How to use

Open Tailor in game (Shift + Z by default) → Settings → DD Settings\

## Main features

- Off (default): No DD equipment restrictions apply after exiting Tailor\
- On: Normal DD equipment restrictions remain in effect after exiting (Note: Tailor itself includes a cross-save feature, which will cause this follower across all saves to be subject to DD’s equipment restrictions)\

## Requirements

PapyrusUtil SE\
Tailor\
Devious Device NG\

## AI usage

I Used generate AI for vibe coding, but no AI generated art assets.\

## Feedback and bug report

Please report either on Nexus or [GitHub repository](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch).\
With clear steps of what you did and what unexpected happened. Better with logs so I can fix quicker.\
Include TailorDDBridge.log and Tailor.log where you can find both in:\
`Documents\My Games\Skyrim Special Edition\SKSE\`\
If it's a bug relavent to character appearance (e.g. Skin colour, face sculpt), please also include:\
`Documents\My Games\Skyrim Special Edition\SKSE\skee64.log`\
If it's related to character appearance while using Mu Dynamic NPC Replacer, please also provide:\
`MO2\mods\Mu Dynamic NPC Replacer\SKSE\Plugins\MuDynamicNPCReplacer.log`\

## Shout outs and License

Credit to exiledviper (continued maintenance/refactoring) and meh321 (original version and idea) for [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048). Our independent helper calls its StorageUtil functions. PapyrusUtil is a separately installed runtime requirement; its DLL and implementation scripts are not included.\

Included: independent patch DLL and helper PEX, bilingual FOMOD, generator tools, bilingual instructions and license notices. Excluded: Tailor runtime/assets/fonts, third-party translations, DD/Mu/RaceMenu implementations or assets, personal presets/generated equipment data, saves, logs, and temporary startup diagnostic tools.\

Credit to ColdSun / heathbrownkeyworks for Tailor and its API, the DD/DD NG developers, Acro for Mu, expired6978 for RaceMenu, and Llinnn for the separately distributed Tailor CHS translation. Their mods must be obtained independently. This is an unofficial compatibility add-on.\

The [Source.zip](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.4-permissions.3) and [GitHub repository](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch) contain corresponding source and build instructions. Do not install Source.zip in MO2. The patch is GPL-3.0-or-later; see LICENSE and THIRD_PARTY_NOTICES.txt.\
Source delivery: see SOURCE-ACCESS.md and dependency-sources/README.md in the complete Source.zip for the actual 8 resolved dependencies, source SHA-256 values, recorded recipes/configurations and configured-header copies. This packaging revision does not rebuild or alter the tested DLL/PEX.\

Full attribution and permission record: CREDITS.md, PREREQUISITE-NOTICES.md, RESOURCE-AUDIT.json (included in the installer/source). Includes SKSE team, meh321/Address Library, DD/NG authors, PapyrusUtil authors, optional integrations, linked libraries and embedded code. Permission to publish this compatibility patch has been granted by Tailor author ColdSun; see [AUTHOR-PERMISSIONS.md](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/AUTHOR-PERMISSIONS.md).
