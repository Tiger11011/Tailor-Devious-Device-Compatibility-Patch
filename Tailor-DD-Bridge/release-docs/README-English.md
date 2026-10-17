# Tailor Devious Device Compatibility Patch 0.4 — Experimental

An independent SKSE plugin for trying on supported Devious Devices equipment in Tailor. Open Tailor, open **Settings**, then **DD Settings**. Supported devices use removable cosmetic copies while fitting. Ordinary clothing follows Tailor's normal workflow. This download includes the latest button-label centering adjustment.

## Requirements — install separately

1. **Tailor 3.0.1**, including its own requirements: https://www.nexusmods.com/skyrimspecialedition/mods/173691
2. **SKSE64** matching your Skyrim executable: https://skse.silverlock.org/
3. **Address Library for SKSE Plugins**, matching your game version: https://www.nexusmods.com/skyrimspecialedition/mods/32444
4. Your DD equipment mods, their Devious Devices framework/scripts and all their normal requirements. Those mods must already be correctly installed and enabled in the MO2 profile you generate from.
5. **Mod Organizer 2** and **Python 3.10 or later** for the supplied data generator. Python is needed during generation, not while playing. The generator uses the Python standard library; no pip packages, SSEEdit or .NET are needed for it.
6. The **Microsoft Visual C++ x64 runtime** required by native SKSE plugins must be installed.

This experimental build accepts only these exact Tailor.dll SHA-256 hashes:

| Tailor build | SHA-256 |
| --- | --- |
| English 3.0.1 | `261f415773d550e16809cbdfa39863451ea3eeba247e30b996afabe1e1d47850` |
| Supported separately obtained CHS 3.0.1 | `83dc9c56627d163e00eb9e5f1cceb1af3edb84deaa286ec828c95fb2a0d0df4d` |

A matching version label alone is insufficient. Other Tailor builds/translations and Skyrim VR are not supported by this release. SE/AE users must satisfy Tailor's own runtime requirements; this package is not a claim that every Skyrim executable has been tested. Unknown Tailor DLLs are rejected rather than hooked.

## Choose the language in FOMOD

- **English**: this bridge's new labels and explanations are in English.
- **中文**: only this bridge's new labels and explanations are in Chinese. Obtain and install the supported Tailor CHS translation separately for its Chinese font coverage: https://www.nexusmods.com/skyrimspecialedition/mods/184318

Download the single FOMOD archive. Its left panel asks "你用什么语言？" and "What language do you use?"; choose **中文** or **English** on the right. The shared DLL and tools are installed once; the choice changes only this bridge's JSON language. The package does not translate Tailor or contain the third-party translation.

If you already have matching Generated Data from this build, keep it enabled: changing the installer format or UI language does not require regeneration. The installer defaults real-DD activation to Off; back up your bridge JSON if preserving that preference matters.

## MO2 installation

1. Close Skyrim. Install Tailor and its requirements. If desired, install the supported Tailor translation so it wins Tailor's own file conflicts. Disable older bridge experiments that replace Tailor.dll, and disable the withdrawn 0.5 save-isolation experiment.
2. Install **Tailor-Devious-Device-Compatibility-Patch-0.4-FOMOD.zip** as its own MO2 mod. Choose **中文** or **English** in the installer, then enable the installed mod. Disable previously separate English/Chinese bridge mods; keep only this installation active. It contains `SKSE/Plugins/TailorDDBridge.dll` and does not overwrite `Tailor.dll`.
3. Select the MO2 profile you will play and enable the DD/outfit mods and plugins you want scanned. Close MO2 to ensure its profile files are saved. In MO2, **Open in Explorer** on the bridge mod folder first if you need its path; use Windows Terminal in that folder for the next command. Run the generator from Windows, not through MO2's virtual filesystem.
4. Generate your own equipment data (replace the example paths):

   ```powershell
   py -3 Tools/build_cosmetics.py --mo2 "D:/YourMO2" --profile "Default" --output "D:/TailorDDGenerated" --zip
   ```

   If `py` is unavailable, use `python` or the full path to your Python 3.10+ executable. The quoted MO2 path must contain `ModOrganizer.ini`; `--profile` is the actual MO2 profile name. If omitted, the currently selected profile is used. Put the output in a dedicated folder **outside** the game Data folder and MO2 mods/overwrite folders. Custom MO2 layouts can use `--game-data`, `--mods-dir`, `--profiles-dir`, and `--overwrite-dir`; see `--help`.

5. Reopen MO2. Install the generated `D:/TailorDDGenerated.zip` as a second mod, for example **Tailor Devious Device Compatibility Patch - Generated Data**. Enable it and `TailorDDCosmetics.esp`. Place the ESP **after all its listed masters**. Keep generated data below other copies of its ESP/manifest in MO2's left pane so this matching pair wins. The bridge language mod and generated-data mod normally have no overlapping runtime files.
6. Launch Skyrim using SKSE through MO2. Open Tailor with its configured hotkey (default **Shift+Z**) or power, then **Settings → DD Settings**. If the bridge reports unavailable, read `TailorDDBridge.log` under the game's usual SKSE logs directory.

**Do not upload or use someone else's generated ESP/manifest as universal data.** Its masters and mappings depend on that player's enabled mods. The FOMOD download intentionally contains no ESP. Never generate a fresh mapping for an existing playthrough: keep the output folder and manifest so FormIDs remain stable.

### Typical enabled mods

```text
Tailor (original)
Tailor CHS translation (optional; installed separately)
Tailor Devious Device Compatibility Patch (choose language in FOMOD)
Tailor Devious Device Compatibility Patch - Generated Data
```

The original/translation conflict order remains as required by those mods. This bridge does not replace either DLL. Only the generated ESP needs an entry in the right-pane plugin list; the bridge itself is an SKSE DLL.

## Using DD Settings

- While fitting: supported devices use removable cosmetic copies.
- **Activate real DD on confirmed exit — Off (default):** keep cosmetic copies after confirming with **Set** and closing normally.
- **On:** after **Set** and a normal exit, attempt to equip the real DD items for the confirmed outfit currently worn. DD's own restrictions, conflicts, prompts and asynchronous scripts determine whether equipping/locking succeeds.
- Turning the switch Off before exit cancels pending activation. If you turn it On after pressing Set, confirm the outfit again. An outfit assigned to an inactive situation is not immediately forced onto the actor.
- Adding pieces to an outfit or previewing it is not a substitute for confirming a wardrobe assignment with Set.
- Already locked real devices are not automatically unlocked. This is not an emergency unlock tool. The switch handles fitting/equipping; it does not manage muffling, animations or other DD effects. Cosmetic copies do not reproduce all original scripted effects.

**Tailor's normal shared NPC assignments remain unchanged.** NPC outfit choices can carry across saves as designed by Tailor. This release does not add save-by-save NPC assignment isolation, clear legacy assignments or migrate old assignments.

## Compatibility and updates

Detection uses the winning loose PEX script inheritance and validated DD inventory/render-device links, not garment names. Third-party garments using that verified protocol can be included. Standalone scripts that merely call DD, BSA-only script ancestry, scripted rendered devices and other excluded/unknown devices are not blanket-supported. Review the local generation report; do not assume every DD-labelled garment is covered.

Regenerate data when enabled source plugins, script winners or the active mod profile change. Use the **same output folder**, then replace the generated MO2 mod with the new matching ESP/manifest. Existing IDs are preserved and removed IDs retired. Runtime hash checks reject modified inputs; do not bypass them. Keep separate generated outputs for different mod setups.

Back up your bridge JSON before reinstalling if you want to preserve switch preferences; package defaults set real-DD activation Off. Reinstall the FOMOD to change language, keeping only one bridge installation enabled.

Use a backup/test save first. Existing saves may reference the generated ESP. For removal, return to a save from before using the bridge/generated data; disabling the DLL alone does not remove genuine DD equipment or undo DD locks. No save-cleaning or forced-unlock feature is included.

## Status, contents and credits

Experimental release. Local NPC fitting tests have been reported working, including free switching of supported DD copies; this is not a complete player/NPC, language and runtime compatibility matrix. The button centering change compiles but still needs in-game visual confirmation. Report the game/SKSE/Tailor versions, DD mod, player or NPC, switch state, exact Set/exit steps and relevant Tailor/bridge logs when reporting a problem.

Included: our independent DLL, default configuration, the two Python generation tools, bilingual instructions and license notices. Excluded: Tailor.dll, Tailor ESP/assets/fonts, third-party translation files, DD scripts/assets, personal generated data, personal configurations, saves and logs.

Credit: ColdSun / heathbrownkeyworks for Tailor and its API; Llinnn for the separately distributed CHS translation (not redistributed here). The companion **Source** archive provides corresponding bridge/generator source and build inputs. It is for developers, not an MO2 install. See `LICENSE` and `THIRD_PARTY_NOTICES.txt` for GPL and dependency notices. This is an unofficial compatibility add-on.
