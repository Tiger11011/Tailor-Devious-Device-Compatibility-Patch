# Source access / 对应源码

Version **0.6.14**. Release preparation retains the game-tested DLL and helper PEX without rebuilding. The tested patch DLL SHA-256 is:

`e34a3e1cbf44508b268b5f058baa4741fedc2cb1ed6bd1ab1ad9989a6230d4f2`

Download **Tailor-Devious-Device-Compatibility-Patch-0.6.14-Source.zip**, free of charge alongside the FOMOD, from [the exact matching release](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.14).

The complete source includes:

- Patch, helper and legacy generator source; fitting-preview OAR JSON; build and packaging files.
- CommonLibSSE-NG, OpenVR and MinHook source, and eight resolved dependency source archives with recorded recipes, configuration differences, configured headers and SHA-256 values.
- The public Open Animation Replacer Conditions API source, provenance and license notices. OAR itself remains a separately installed prerequisite.
- Attribution, third-party license texts and the ColdSun permission record/screenshot. The permission screenshot is included in Source.zip and the repository, not the FOMOD.

See `BUILD.md` and `dependency-sources/README.md` inside Source.zip. Upstream components retain their individual licenses. General-purpose build tools and separately required game/mod implementations are obtained independently. GitHub's automatic repository source download is not a substitute for the complete Source.zip, because it does not supply all vendored/submodule and dependency archive contents.

**Source.zip is not a player installer: do not install it in MO2.** On Nexus, provide this matching Source.zip in Optional Files or clearly link the exact release beside the binary download. The installer does not bundle Tailor runtime/assets/fonts, third-party translations, DD/Mu/RaceMenu implementations/assets, personal presets, generated player equipment data, saves or logs. OAR JSON only references separately installed DD animations.

CommonLib/OpenVR source scope and omitted non-build SDK examples/test data are documented in `dependency-sources/VENDORED-SOURCE-SCOPE.md`. Source.zip carries a per-file `VENDORED-SOURCE-SHA256.json`.

## 中文

版本为 **0.6.14**。发布整理不重新编译，保留游戏实测的 DLL 和辅助 PEX；实测 DLL 的 SHA-256 见上文。

[完全对应的发布页](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.14)与 FOMOD 同时免费提供 **Tailor-Devious-Device-Compatibility-Patch-0.6.14-Source.zip**。

完整源码包包含补丁、辅助脚本和旧生成器源码、试衣预览 OAR JSON、构建及打包文件；CommonLibSSE-NG、OpenVR、MinHook；实际解析的 8 项依赖原始源码、配方、配置差异、已配置头文件及校验值；公开 OAR Conditions API 源码、来源和许可声明。OAR 本体仍需另行安装。ColdSun 授权记录与截图、第三方作者及许可说明均保留，截图仅收入源码包和仓库，不收入 FOMOD。

构建方法及依赖清单见包内 `BUILD.md` 和 `dependency-sources/README.md`。各依赖保留各自许可证，通用构建工具及游戏/模组本体需另行获取。GitHub 自动生成的源码下载不能替代完整 Source.zip，因为它不能提供全部内含/子模块源码及依赖附件。

**Source.zip 不要安装到 MO2，普通玩家只需 FOMOD。** 上传 Nexus 时，在 Optional Files 提供这份匹配源码，或在主文件下载说明旁明确链接该固定发布页。不包含 Tailor 本体/字体、他人汉化、DD/Mu/RaceMenu 实现或资源、个人预设、玩家生成装备数据、存档或日志；OAR JSON 只引用另行安装的 DD 动画。

CommonLib/OpenVR 源码范围及未收入的非构建示例/测试数据见 `dependency-sources/VENDORED-SOURCE-SCOPE.md`，逐文件校验表为 `VENDORED-SOURCE-SHA256.json`。
