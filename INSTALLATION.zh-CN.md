[中文说明](README.zh-CN.md) | [English](README.md) | [Downloads](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases)

# Tailor Devious Device Compatibility Patch 0.6.14 — 详细技术说明


**本二进制对应源码：**在[完全对应的发布页](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.14)免费下载 Tailor-Devious-Device-Compatibility-Patch-0.6.14-Source.zip，与 FOMOD 同页提供。上传 Nexus 时，将这份源码放到 Optional Files，或在主文件下载说明旁明确链接该对应源码。源码包含构建文件及固定版本依赖源码。


在 Tailor 中自由试穿受支持的 DD 装备。确认退出后保留真实 DD 的音效、动画等效果，由开关决定是否保留正常穿脱限制。独立补丁，不替换 Tailor.dll，不包含任何人的 Tailor 汉化。

**Tailor 作者许可：**已获得 ColdSun 对发布本兼容补丁的许可，详见[许可记录](AUTHOR-PERMISSIONS.md)及[许可截图](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/PERMISSIONS.png)。截图包含在源码包和仓库中，FOMOD 安装包不含截图。

## 前置要求

以下模组和各自的前置均需自行安装：

- [Tailor 3.0.1](https://www.nexusmods.com/skyrimspecialedition/mods/173691)，仅支持下表已核查的英文或汉化构建。
- Skyrim SE/AE、与游戏匹配的 [SKSE64](https://skse.silverlock.org/)、[Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444) 及原生插件需要的 Microsoft Visual C++ x64 运行库。本次实测使用 Skyrim 1.6.1170；其他游戏版本及 VR 未验证。
- Devious Devices、服装本身的全部依赖，以及 **DD NG v0.4.3 rc2，且 DLL 哈希与下表一致**。[DD NG 项目页面](https://www.loverslab.com/files/file/29779-devious-devices-ng/) 仅供查找项目，并不保证页面当前下载就是受支持构建。请从作者渠道获取匹配版本，不能仅凭版本名称判断兼容。
- 与游戏/SKSE 匹配的 [PapyrusUtil](https://www.nexusmods.com/skyrimspecialedition/mods/13048)，补丁辅助脚本需要其 StorageUtil 函数。
- **Open Animation Replacer：**试衣预览条件需要此模组，本地实测 **3.1.6**，使用公开 Conditions 接口 **V3（API 数值为 2）**。请另行安装及满足其前置。
- **可选 Partial Animation Replacer 适配：**仅支持已核查 DD NG 环境随附的 **PAR 0.2.0** DLL，哈希见下表。本补丁不要求另行安装 PAR；未知版本不启用此适配，保留原处理。
- **可选 Mu 外观适配：**[Mu Dynamic NPC Replacer 2.1.4](https://www.nexusmods.com/skyrimspecialedition/mods/150931)，DLL 需符合下表，并安装 Mu 要求的 RaceMenu。普通使用不强制要求 Mu；未知 Mu DLL 不启用外观适配。
- **可选 Tailor 汉化：**自行下载[受支持的汉化](https://www.nexusmods.com/skyrimspecialedition/mods/184318)。补丁中文依赖 Tailor 汉化提供的中文字库；英文玩家无需安装汉化。

| 受支持的文件 | SHA-256 |
| --- | --- |
| Tailor 3.0.1 英文 Tailor.dll | `261f415773d550e16809cbdfa39863451ea3eeba247e30b996afabe1e1d47850` |
| Tailor 3.0.1 受支持汉化 Tailor.dll | `83dc9c56627d163e00eb9e5f1cceb1af3edb84deaa286ec828c95fb2a0d0df4d` |
| DD NG v0.4.3 rc2 DeviousDevices.dll | `f68147a58cdffa4c4bd9c69844c073a8a274abe30357a76402e6e5b78ffc69b4` |
| 可选 PAR 0.2.0 PartialAnimationReplacer.dll | `fa07d8355ee075c4d42a02a7c426524e88d1c027998b0446660c58b768b8030a` |
| 可选 Mu 2.1.4 MuDynamicNPCReplacer.dll | `009113664ae10e406914bc6fa8a394290aaa03cfd68367dce154e84543c7da0a` |

本版原生兼容范围有意限定于已核查的构建。未知构建会拒绝启用对应功能，请勿绕过哈希检查。

## MO2 安装

1. 关闭 Skyrim，安装 Tailor、DD/DD NG、PapyrusUtil、Open Animation Replacer 及各自前置。Tailor 汉化由用户另行获取。
2. 安装 **Tailor-Devious-Device-Compatibility-Patch-0.6.14-FOMOD.zip**，选择中文或 English，并在右侧启用随包的 **TailorDDAutoFitting.esp**。语言选择只影响本补丁。
3. 只启用一个补丁版本，禁用旧的独立语言补丁和临时启动诊断工具。已有存档/套装使用旧 Generated Data 的用户保留原数据。
4. 通过 MO2/SKSE 正常启动。加载时自动识别当前胜出的装备配对和松散/BSA 脚本继承；无需 Python、外部生成器或先进入再退出一次游戏。
5. 打开 Tailor → 设置 → DD 设置；开关默认关闭。

左侧顺序：所需前置（含 DD NG 和 OAR）→ Tailor → 可选汉化 → 本补丁。新用户不需要 Generated Data；下方旧数据说明仅适用于已使用该数据的存档/套装。不替换 Tailor.dll、他人汉化或 DD/Mu 原文件。

## 开关行为

- **试衣过程中：**受支持的 DD 使用可自由穿脱的外观副本。需要通过 Tailor 的 Set/确认分配流程并正常退出；单独添加“+”或预览不等于确认套装。
- **关闭（默认）：**确认退出后使用真实 DD 装备，保留音效、动画等原有效果；补丁对自己管理的“角色—设备”组合授予自由穿脱权限，在 Tailor 和普通物品栏均可更换。
- **开启：**确认退出后遵守真实 DD 原有穿脱限制，装备冲突、脚本和锁定规则仍由 DD 处理。
- 要改变已经分配套装的处理方式，请调整开关后重新确认这套衣服。关闭不是全局解锁所有既有 DD 的命令。
- 只处理受支持且由补丁管理的通用设备。不会禁用声音减弱、音效、动画等其他 DD 特性，不强制解除任务设备、阻止通用卸装的设备、未知自定义脚本或其他玩法独立锁上的设备。

## 装备数据、升级与存档

自动识别依据已加载记录与经过核查的 DD 脚本继承，不按衣服名称判断。自定义/未知脚本、任务设备及 block-generic 限制仍按原策略排除。新增受支持衣服后正常重启游戏即可，已有身份编号不会重排。

请随存档备份 `文档\My Games\Skyrim Special Edition\SKSE\TailorDDBridge.AutomaticSlots.json`。这是只追加的身份记录，不是日志；不要删除、压缩/重新编号随包的完整 ESP，或复用已移除衣服的编号。插件提供 65,536 个身份，占用普通插件槽。

旧 schema-2 的 TailorDDCosmetics.esp 及配套 TailorDDSafeFitting.json 仍受支持。已有存档/套装用过时保留二者，不要为升级而重新生成或删除。新用户只需随包自动插件；旧生成器源码保留作为参考，正常安装不再运行它。

保留 Tailor 自身的跨存档套装分配；自由穿脱授权仍使用各存档的 SKSE 共存档。不清空分配，不强制清除旧档里的 DD 标记/锁定。若旧版失败已写入存档，请用失败前的存档测试。更新先备份；卸载 DLL 不会移除设备或解除锁定。

## 本版修复与验证范围

0.6.14 保留自动试衣数据识别、DD 脱卸标记修复、外观恢复及默认关闭的开关。自 0.6.6 以来改善试衣中的姿势选择与清理，丢弃被新请求替代的旧套装请求，并防止自动待机替换已选择的 Pet 预览。可选 PAR 适配仅针对当前试衣 NPC，调用 PAR 自身的条件检查，在拘束装备脱下后跳过已失效的骨骼覆盖；仍有效的缓存及范围外角色继续原处理。

随包 OAR JSON 选择用户 DD 安装中已有的预览动画，不分发 DD 动画文件。试衣后真实 DD 动画与音效保留，Tailor 自身的跨存档套装功能不变。

2026-10-21 本地实测中，用户确认拘束姿势→普通衣服→Pet 装备切换正常。日志记录至少 64 次跳过失效 PAR 覆盖、5 次预期 Pet 动画快照、5 次自动待机请求被处理，以及约 16 秒内 Pet 条件 767/767 命中；旧套装请求被丢弃，观察到的穿脱回调成功，退出时清除预览范围。64 条是日志上限，不是全部检查次数。实测 DLL 哈希及边界见 [VALIDATION-0.6.14.md](VALIDATION-0.6.14.md)，不代表所有装备、角色、游戏版本或语言的全面验证。

## 内容与致谢

感谢 ersh（ersh1）及 Open Animation Replacer 贡献者提供公开 Conditions API 源码；原始 COPYING 和 EXCEPTIONS 文件保留在源码及安装包许可声明中。

感谢 [PapyrusUtil SE](https://www.nexusmods.com/skyrimspecialedition/mods/13048) 的 exiledviper（持续维护与重构）及 meh321（原始版本与构想）。补丁自有辅助脚本调用其 StorageUtil 函数；PapyrusUtil 是需单独安装的运行时前置，不随包分发其 DLL 或实现脚本。

包含独立 DLL、自有辅助 PEX、双语 FOMOD、自动试衣插件、试衣预览 OAR 条件 JSON、双语说明及许可证。JSON 引用已安装的 DD 资源，不包含 DD 动画或 PAR 配置资源。不包含 Tailor 本体/字体、他人汉化、DD/Mu/RaceMenu 实现及资源、个人预设/生成装备数据、存档、日志或临时崩溃诊断工具。

感谢 ColdSun / heathbrownkeyworks 的 Tailor 和 API、DD/DD NG 开发者、Mu 作者 Acro、RaceMenu 作者 expired6978，以及独立发布 Tailor 汉化的 Llinnn。相关作品均需自行下载。本补丁为非官方兼容扩展。

对应源码及构建说明见 **Source.zip** 和 [GitHub](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch)。Source.zip 不是 MO2 安装包。补丁使用 GPL-3.0-or-later，详见 LICENSE 与 THIRD_PARTY_NOTICES.txt。

源码交付：完整 Source.zip 内的 SOURCE-ACCESS.md 和 dependency-sources/README.md 列出实际解析的 8 项依赖、源码 SHA-256、构建配方/配置和已配置头文件副本。完整源码也包含公开 OAR Conditions API 及许可声明。发布整理保留游戏实测的 0.6.14 DLL 和辅助 PEX，不重新编译。

完整作者与许可清单：**CREDITS.md**、**PREREQUISITE-NOTICES.md**、**RESOURCE-AUDIT.json**（安装包与源码包均包含）。涵盖 SKSE 团队、meh321/Address Library、DD/NG 作者、PapyrusUtil 作者、可选集成、链接库及嵌入代码。已获得 Tailor 作者 ColdSun 发布本兼容补丁的许可，详见 [AUTHOR-PERMISSIONS.md](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/blob/main/AUTHOR-PERMISSIONS.md)。
