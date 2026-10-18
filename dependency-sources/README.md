# Corresponding dependency source — 0.6.4-sources.4

This is a source/notice packaging revision. No C++, Papyrus, generator, game
configuration or tested binary was changed or recompiled.

The **matching release Source.zip** contains `dependency-sources/archives/`:
the eight original upstream source archives listed in `DEPENDENCIES.json`.
They retain upstream notices and licenses. Their hashes match the retained
xmake-repo recipes, rather than a download of the latest version.
Git clones intentionally do not store these archives in Git history. From a
clone, run `python dependency-sources/fetch_sources.py --download` to retrieve
the same archives. Without `--download`, the command only verifies local files.
The matching release is:
https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.4-sources.4

| Component | Actual resolved version | Use |
| --- | --- | --- |
| Dear ImGui | 1.92.6 | Static; Win32, DX11, FreeType enabled |
| FreeType | 2.14.1 | Static; zlib enabled; bzip2/PNG/WOFF2/HarfBuzz disabled |
| zlib | 1.3.2 | Static, FreeType dependency |
| spdlog | 1.16.0 | Static; wchar and std::format enabled |
| nlohmann/json | 3.12.0 | Header-only; xmake patch retained |
| DirectXTK | 24.2.0 / feb2024 | Static |
| DirectXMath | 2024.02 / feb2024 | Headers |
| rapidcsv | 8.92 | Header-only |

Already included elsewhere in Source.zip: CommonLibSSE-NG
`8b032fa992750d654d6d38a33731714d8b86be1f`, its OpenVR submodule
`60eb187801956ad277f1cae6680e3a410ee0873b`, and MinHook 1.3.4.

## Evidence and configuration differences

The audit used the retained accepted-build link inputs and installed xmake
package manifests, not just version labels in the project file.
`accepted-link-inputs.lua` records the linked libraries with private workspace
paths removed. The eight `*-build-manifest.lua` files preserve resolved package
configuration, dependency versions and the xmake-repo revision. Installed
library SHA-256 values and header comparisons are in `DEPENDENCIES.json`.
FreeType and DirectXTK were installed as xmake prebuilt artifacts; this audit
matches their recorded versions and installed headers to source, and does not
claim to have rebuilt those artifacts or reproduced their bytes.

Five installed headers differ from the original upstream archives. Exact
copies are retained in `installed-overrides/`:

- ImGui `imconfig.h`: xmake-configured compile-time options, including the
  exact FreeType/SVG defines present in the retained installed header.
- FreeType `ftconfig.h`, `ftoption.h`: retained installed configuration;
  `ftconfig.h` differs only in line endings, while `ftoption.h` enables
  `FT_CONFIG_OPTION_SYSTEM_ZLIB`.
- JSON `from_json.hpp`, `to_json.hpp`: the xmake `fix-ndk.patch` adjustments.

The retained recipe directories contain the actual build scripts and patches,
including JSON's patch. Preserve the archive's top-level directory when
extracting it. JSON overrides map from `include/nlohmann/...` to the same path
in its source tree. ImGui's `include/imconfig.h` maps to source-root
`imconfig.h`. FreeType's `include/freetype2/freetype/config/...` maps to
`include/freetype/config/...`. The overrides are exact installed reference
files, not new changes to the tested patch. Use each component's original
license for these copies; this repository does not relicense them all as GPL.
`configured-header-differences.patch` shows the substantive differences after
normalizing line endings; it therefore has no change for `ftconfig.h`.

## Building with the recorded dependencies

See `../BUILD.md` for the DLL and independent Papyrus helper commands. The
existing functional xmake.lua is deliberately unchanged. Its transitive
FreeType/zlib resolution is not a complete lockfile, so do not rely on a future
default package-repository checkout resolving the same versions.

For the recorded recipe set, clone https://github.com/xmake-io/xmake-repo and
check out commit `bd29d77f242e247589457638272d9936a24a3fe0`. Retained copies of
the relevant library/tool recipes are under `recipes/`. Use an isolated xmake
environment and this fixed repository revision; do not silently update the
repository before configuring. Ensure all eight resolved versions and configs
match this manifest before compiling. To build dependencies locally instead
of consuming prebuilt package artifacts, use xmake's package `--build` option
(consult the installed xmake help for syntax), or build their verified source
archives using the retained recipes/configuration. Upstream source archives
include their CMake/Meson and other original build inputs. The exact installed
overrides are supplied for comparison and manual builds.

The accepted link record identifies MSVC 14.44.35207 (VS 2022), Windows x64,
Release. The retained MSVC detection cache identifies Windows SDK 10.0.22621.0.
Library manifests use dynamic release MSVC runtime (MD). `BUILD-TOOLS.json`
records separately installed tool versions and their evidence limits.
The retained xmake utility reports `3.1.1+HEAD.3ba37a0d4`.
General-purpose tools (compiler, Windows SDK, xmake, CMake, Meson, Python,
Ninja, pkgconf) and Skyrim's PapyrusCompiler are obtained separately; they
are not game content or missing library implementations. Source access is
complete for the enumerated libraries without downloading binary packages.
This revision verifies source availability, hashes, recorded configurations
and unchanged runtime files. It does **not** claim a clean-room or bit-identical
rebuild; build outputs can differ across toolchains and environments.

## 中文

这是源码和说明修订，不修改或重新编译已通过测试的功能。完整 Source.zip
已附带上表 8 项依赖的原始源码压缩包、SHA-256、实际构建配置、构建配方和
配置过的头文件。Git 仓库本身不存放这些压缩包；从 Git 克隆后，可运行
`python dependency-sources/fetch_sources.py --download` 获取并校验同一批文件。
直接下载本修订 Source.zip 则无需再下载依赖源码。

默认构建可能随将来的包仓库变化而选择新的传递依赖，因此请按清单核对版本、
固定配方仓库提交，并保留原有配置差异。此处不声称重新编译会得到字节相同的
DLL，也不需要用户为这次文档修订重复测试游戏。Source.zip 不要安装到 MO2。

在 Nexus 主文件旁提供本修订 Source.zip 的免费下载，或明确链接上述同一
GitHub 发布页。保留与二进制对应的源码获取方式；不要只链接会变化的 main。

CommonLib/OpenVR source scope and omitted non-build SDK examples/test data: dependency-sources/VENDORED-SOURCE-SCOPE.md. Source.zip carries a per-file VENDORED-SOURCE-SHA256.json.
