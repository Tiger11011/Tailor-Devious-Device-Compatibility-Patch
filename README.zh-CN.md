[中文说明](README.zh-CN.md) | [English](README.md) | [Downloads](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases)

# Tailor Devious Device Compatibility Patch

详细安装、装备自动识别和受支持版本：[INSTALLATION.zh-CN.md](INSTALLATION.zh-CN.md)

## 简介

这是一个兼容性补丁模组，用于修复“Devious Device”（DD）与“Tailor - 服装与假发管理器”之间的bug。让DD的装备能在Tailor试衣间里使用，详细信息请以此处为准 [Github发布页](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch)\


## 安装说明

安装 FOMOD，选择语言，并启用 **TailorDDAutoFitting.esp**，然后通过 SKSE 正常启动即可。无需外部生成器、Python 或先进入再退出一次游戏；新增受支持服装会在下次启动时自动识别。\


## 顺序 （下面覆盖上面）

Tailor\
Tailor translation（可选，需用户自行获取）\
Tailor Devious Device Compatibility Patch\


## 怎么用

游戏内打开Tailor (默认 Shift + Z ) → 设置 → DD 设置\


## 主要功能

- 关闭（默认）：退出 Tailor 后，补丁管理的受支持 DD 衣服可自由穿脱，动画、音效等原有效果保留\
- 开启：退出后保留 DD 正常穿脱限制（注意Tailor本身带有跨存档保留功能，会导致所有存档的该随从都被DD拘束装备限制）\


0.6.14 改善拘束装备、普通衣服与 Pet 装备之间切换时的姿势预览：试衣中跳过已失效的上身姿势覆盖，并保护已选择的 Pet 预览不被自动待机替换。2026-10-21 本地实测通过了用户报告的切换流程，详见[验证及限度](VALIDATION-0.6.14.md)。\

## 前置

PapyrusUtil SE\
Tailor\
Devious Device NG\
Open Animation Replacer（本地实测 3.1.6；公开 Conditions 接口 V3，API 数值为 2）\

可选的 Partial Animation Replacer 0.2.0 适配支持 DD NG 随附的已核查版本，本补丁不要求另行安装 PAR。精确支持范围见[安装说明](INSTALLATION.zh-CN.md)。


## 反馈和bug报告

请在Nexus或 [Github发布页](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch)进行反馈\
请详细说明操作的步骤以及遇到了什么意外情况。最好附上日志log，这样我才能更快地解决bug\
反馈时别忘了附带TailorDDBridge.log和Tailor.log，你可以在这里找到：\
`Documents\My Games\Skyrim Special Edition\SKSE\`\
如果该问题与角色外观（例如肤色、捏脸）相关，请额外提供以下log：\
`Documents\My Games\Skyrim Special Edition\SKSE\skee64.log`\
如果你碰到角色外观相关的问题同时使用Mu Dynamic NPC Replacer，请提供：\
`MO2\mods\Mu Dynamic NPC Replacer\SKSE\Plugins\MuDynamicNPCReplacer.log`\



## AI使用相关

我用了生成式AI进行氛围编程，但没有美术资源是AI生成的\


## 声明许可

**许可：**[GPL-3.0-or-later](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/LICENSE)。第三方组件保留各自的许可证，详见 `LICENSE`、`COPYRIGHT.md` 和 `THIRD_PARTY_NOTICES.txt`。

同时感谢随包代码库的作者：**CommonLibSSE-NG** 的 **Ryan-rsm-McKenzie、CharmedBaryon、alandtse 及贡献者**；**MinHook** 的 **Tsuda Kageyu**，以及 HDE 作者 **Vyacheslav Patkov**；**Dear ImGui** 的 **Omar Cornut 及贡献者**。对应版权与许可声明保留在 `CREDITS.md`、`THIRD_PARTY_NOTICES.txt` 和 `licenses/` 中。\

感谢 **ersh（ersh1）及 Open Animation Replacer 贡献者**提供公开 Conditions API 源码；原始 `COPYING` 和 `EXCEPTIONS` 文件保留在源码及安装包许可声明中。\

感谢 [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048) 的 exiledviper（持续维护与重构）及 meh321（原始版本与构想）。补丁自有辅助脚本调用其 StorageUtil 函数；PapyrusUtil 是需单独安装的运行时前置，不随包分发其 DLL 或实现脚本。\

包含：独立 DLL、自有辅助 PEX、双语 FOMOD、自动试衣插件、试衣预览 OAR 条件 JSON、双语说明及许可证。JSON 引用另行安装的 DD 动画，不随包分发 DD 动画或 PAR 配置资源。不包含：Tailor 本体/字体、他人汉化、DD/Mu/RaceMenu 实现及资源、个人预设/生成装备数据、存档、日志或临时崩溃诊断工具。\

感谢 ColdSun / heathbrownkeyworks 的 Tailor 和 API、DD/DD NG 开发者、Mu 作者 Acro、RaceMenu 作者 expired6978，以及独立发布 Tailor 汉化的 Llinnn。相关作品均需自行下载。本补丁为非官方兼容扩展。\

对应源码及构建说明见 [Source.zip](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.14) 和 [GitHub](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch)。Source.zip 不是 MO2 安装包。补丁使用 GPL-3.0-or-later，详见 LICENSE 与 THIRD_PARTY_NOTICES.txt。\
源码交付：请参阅完整 Source.zip 文件中的 SOURCE-ACCESS.md 和 dependency-sources/README.md，其中包含实际已解决的 8 个依赖项、源码的 SHA-256 值、记录的构建配方/配置以及已配置头文件的副本。0.6.14 发布包沿用本轮游戏实测的 DLL 和辅助 PEX，发布整理不重新编译。完整源码还包含 Open Animation Replacer 公开 Conditions API 及其许可声明。授权截图仅收入 Source.zip 和仓库。\

完整作者与许可清单：CREDITS.md、PREREQUISITE-NOTICES.md、RESOURCE-AUDIT.json（包含在安装程序/源代码中）。其中包括 SKSE 团队、meh321/Address Library、DD/NG 作者、PapyrusUtil 作者、可选集成、关联库及嵌入式代码。已获得 Tailor 作者 ColdSun 发布本兼容补丁的许可 [AUTHOR-PERMISSIONS.md](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/AUTHOR-PERMISSIONS.md)

**数据与反馈补充：**请备份 SKSE 日志旁的 `TailorDDBridge.AutomaticSlots.json`，它是装备身份记录，不是可清理日志；不要压缩或重新编号随包 ESP。重启游戏会覆盖 TailorDDBridge.log，请先复制日志。MO2 下 Mu 日志也可能位于 `Overwrite\SKSE\Plugins\` 或指定输出模组；使用 Mu Dynamic NormalMap 时，法线相关问题请一并提供其模组目录或输出目录中的 `MuDynamicNormalMap.log`。
