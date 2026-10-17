# Building from GitHub

The source preserves its three sibling directories so that the existing xmake and packaging paths remain valid. `Tailor-DD-Compat` here contains only the CommonLib dependency, not Tailor's implementation.

Clone with submodules:

```powershell
git clone --recurse-submodules https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch.git
cd Tailor-Devious-Device-Compatibility-Patch
```

CommonLibSSE-NG is pinned to `8b032fa992750d654d6d38a33731714d8b86be1f`, including its own OpenVR submodule. After downloading GitHub's automatic source ZIP instead of cloning, initialize Git dependencies separately or use the complete companion Source.zip from Releases.

Use Visual Studio 2022 with C++ x64 tools and xmake 3.1.1. From this repository root:

```powershell
xmake -P Tailor-DD-Bridge f -m release -y
xmake -P Tailor-DD-Bridge -j 8 -y
```

xmake obtains the pinned dependencies defined by the project's and CommonLib's xmake files. See BUILD.txt for version and license details. The standalone Source.zip includes dependency source and can also be built with its included instructions.

To test the generator without game files:

```powershell
python Tailor-DD-SafeFitting/tests/test_portable_generator.py
```

FOMOD maintenance files are in `Tailor-DD-Bridge/release-docs/`. The packaging script is `Tailor-DD-Bridge/tests/build_upload_packages.py`. Its pinned expected DLL hash must be deliberately updated when publishing a changed binary. Run it with `--dll` to specify your built DLL. Do not include your own generated ESP, game data or logs in public uploads.
