# Building 0.6.14

The current implementation is **Tailor-DD-Effects/**. Tailor-DD-Bridge retains older 0.4 source and the shared MinHook dependency. Tailor-DD-Compat here contains only CommonLibSSE-NG, not Tailor's implementation or reverse-engineering inputs.

```powershell
git clone --recurse-submodules https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch.git
cd Tailor-Devious-Device-Compatibility-Patch
xmake f -P Tailor-DD-Effects -m release -y
xmake build -P Tailor-DD-Effects -j 8 -y TailorDDBridge
```

Use Visual Studio 2022 C++ x64 tools and xmake 3.1.1 or compatible xmake >=3.0.1. The build uses C++23, CommonLibSSE-NG `8b032fa992750d654d6d38a33731714d8b86be1f`, MinHook 1.3.4, ImGui 1.92.6 and JSON 3.12.0. CommonLib pins its other dependencies; OpenVR is `60eb187801956ad277f1cae6680e3a410ee0873b`. Keep the sibling directories. The matching 0.6.14 Source.zip includes eight upstream dependency archives plus recorded configurations and recipe patches; see dependency-sources/README.md. Git clones can retrieve/verify them with `python dependency-sources/fetch_sources.py --download`. The automatic-fitting code uses the same pinned dependency versions; future transitive dependency resolution must be checked against DEPENDENCIES.json; GitHub's automatic source archive omits submodules.

## Independent Papyrus helper

Compile **only** Tailor-DD-Effects/Papyrus/TailorDDFreeWear.psc using Skyrim's PapyrusCompiler:

```powershell
New-Item -ItemType Directory -Force Tailor-DD-Effects/Papyrus/output
& "PATH/TO/PapyrusCompiler.exe" "Tailor-DD-Effects/Papyrus/TailorDDFreeWear.psc" -f="Tailor-DD-Effects/Papyrus/compile-only/TESV_Papyrus_Flags.flg" -i="Tailor-DD-Effects/Papyrus;Tailor-DD-Effects/Papyrus/compile-only" -o="Tailor-DD-Effects/Papyrus/output"
```

The compile-only files are minimal declarations, not DD/RaceMenu/PapyrusUtil implementations. **Never install PEX files compiled from these declarations.** Distribute only TailorDDFreeWear.pex. The helper, unchanged since 0.6.6, fixes removal-token ownership and guards relocking; compile the current source, not an older helper. PEX header metadata may vary between compiler environments.

## Checks

```powershell
python Tailor-DD-Effects/tests/removal_token_regression.py
python Tailor-DD-SafeFitting/tests/test_portable_generator.py
```

From a Visual Studio x64 Native Tools terminal:

```powershell
cl /nologo /std:c++latest /EHsc /MD /I Tailor-DD-Effects/src Tailor-DD-Effects/tests/mu_preset_path_test.cpp /Fe:mu_preset_path_test.exe
./mu_preset_path_test.exe
cl /nologo /std:c++latest /EHsc /MD Tailor-DD-Effects/tests/mu_abi_test.cpp /Fe:mu_abi_test.exe
./mu_abi_test.exe
cl /nologo /std:c++latest /EHsc /MD /I Tailor-DD-Effects/src Tailor-DD-Effects/tests/operation_policy_test.cpp /Fe:operation_policy_test.exe
./operation_policy_test.exe
```

The native DD hook test needs a separately obtained exact supported DeviousDevices.dll, not distributed here. Offline tests do not execute the game.

## Automatic fitting pool

```powershell
python Tailor-DD-Effects/tests/build_auto_fitting_pool.py --help
```

The packaging script calls the pool builder directly. The pool contains 65,536 empty armor records; appearances are populated in memory from the user's loaded equipment. No user clothing assets are bundled.

## Packaging

```powershell
python Tailor-DD-Effects/tests/package_release.py --dll PATH/TO/TailorDDBridge.dll --pex PATH/TO/TailorDDFreeWear.pex --dependency-source-dir dependency-sources/archives --reference-fomod PATH/TO/verified-0.6.14-local-test-FOMOD.zip
```

The script accepts the deliberately pinned tested DLL and helper hashes; review new binaries before changing the pins. Recompilation need not produce byte-identical output. Packaging checks both FOMOD language mappings and excluded files, then creates corresponding source and checksums. It never packages personal equipment data, presets, game files, logs, credentials, backups or startup diagnostics.

FOMOD maintenance inputs are in Tailor-DD-Effects/release-docs. See runtime instructions for exact supported prerequisite builds.

CommonLib/OpenVR source scope and omitted non-build SDK examples/test data: dependency-sources/VENDORED-SOURCE-SCOPE.md. Source.zip carries a per-file VENDORED-SOURCE-SHA256.json.

## Preview adapters in 0.6.14

The Open Animation Replacer public Conditions API support files and original
licenses are retained in `Tailor-DD-Effects/src/OAR`. Their per-file hashes and
provenance are included. The OAR DLL and DD animations are obtained separately.
The PAR adapter calls the separately installed exact supported PAR DLL; it does
not bundle PAR code or data. See the runtime instructions for supported builds.

`preview_boundary_test.cpp` covers request ordering, full-width Havok strings,
DD visibility and scoped PAR context policies. `par_hook_test.cpp` requires a
separately obtained supported PAR DLL and MinHook; it tests a private memory
copy without executing PAR or game code. The DD hook test similarly requires
the supported DD NG DLL. Native tests do not replace gameplay verification.

Release packaging requires the frozen local-test FOMOD as a reference and
compares every DLL/PEX/ESP, OAR configuration and language/default JSON byte
for byte. That reference is a maintainer validation input, not a prerequisite
for players or for building the source. No release-time recompilation occurs.
The original diagnostic startup string remains in the tested source/binary;
see `VALIDATION-0.6.14.md`.
