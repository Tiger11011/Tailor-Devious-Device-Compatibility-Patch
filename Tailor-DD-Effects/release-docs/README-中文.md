# Tailor Devious Device Compatibility Patch 0.6.4

**仅许可文件修订：0.6.4-licenses.1，2026-10-18。** 功能、DLL、辅助 PEX 和功能源码保持实测成功的 0.6.4 不变，仅补充 ColdSun 版权署名和上游许可附件。安装包内 COPYRIGHT.md、licenses/LICENSE-SOURCES.json 记录来源和校验值。

**本二进制对应源码：**在[完全对应的发布页](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch/releases/tag/v0.6.4-licenses.1)免费下载 Tailor-Devious-Device-Compatibility-Patch-0.6.4-licenses.1-Source.zip，与 FOMOD 同页提供。上传 Nexus 时，将这份源码放到 Optional Files，或在主文件下载说明旁明确链接该对应源码。源码包含构建文件及固定版本依赖源码。


在 Tailor 中自由试穿受支持的 DD 装备。确认退出后保留真实 DD 的音效、动画等效果，由开关决定是否保留正常穿脱限制。独立补丁，不替换 Tailor.dll，不包含任何人的 Tailor 汉化。

## 前置要求

以下模组和各自的前置均需自行安装：

- [Tailor 3.0.1](https://www.nexusmods.com/skyrimspecialedition/mods/173691)，仅支持下表已核查的英文或汉化构建。
- Skyrim SE/AE、与游戏匹配的 [SKSE64](https://skse.silverlock.org/)、[Address Library](https://www.nexusmods.com/skyrimspecialedition/mods/32444) 及原生插件需要的 Microsoft Visual C++ x64 运行库。本次实测使用 Skyrim 1.6.1170；其他游戏版本及 VR 未验证。
- Devious Devices、服装本身的全部依赖，以及 **DD NG v0.4.3 rc2，且 DLL 哈希与下表一致**。[DD NG 项目页面](https://www.loverslab.com/files/file/29779-devious-devices-ng/) 仅供查找项目，并不保证页面当前下载就是受支持构建。请从作者渠道获取匹配版本，不能仅凭版本名称判断兼容。
- 与游戏/SKSE 匹配的 [PapyrusUtil](https://www.nexusmods.com/skyrimspecialedition/mods/13048)，补丁辅助脚本需要其 StorageUtil 函数。
- MO2、Python 3.10 或更高版本，用于按玩家自己的启用列表生成装备数据。Python 只在生成时需要，无需 pip 安装第三方库。
- **可选 Mu 外观适配：**[Mu Dynamic NPC Replacer 2.1.4](https://www.nexusmods.com/skyrimspecialedition/mods/150931)，DLL 需符合下表，并安装 Mu 要求的 RaceMenu。普通使用不强制要求 Mu；未知 Mu DLL 不启用外观适配。
- **可选 Tailor 汉化：**自行下载[受支持的汉化](https://www.nexusmods.com/skyrimspecialedition/mods/184318)。补丁中文依赖 Tailor 汉化提供的中文字库；英文玩家无需安装汉化。

| 受支持的文件 | SHA-256 |
| --- | --- |
| Tailor 3.0.1 英文 Tailor.dll | `261f415773d550e16809cbdfa39863451ea3eeba247e30b996afabe1e1d47850` |
| Tailor 3.0.1 受支持汉化 Tailor.dll | `83dc9c56627d163e00eb9e5f1cceb1af3edb84deaa286ec828c95fb2a0d0df4d` |
| DD NG v0.4.3 rc2 DeviousDevices.dll | `f68147a58cdffa4c4bd9c69844c073a8a274abe30357a76402e6e5b78ffc69b4` |
| 可选 Mu 2.1.4 MuDynamicNPCReplacer.dll | `009113664ae10e406914bc6fa8a394290aaa03cfd68367dce154e84543c7da0a` |

本版原生兼容范围有意限定于已核查的构建。未知构建会拒绝启用对应功能，请勿绕过哈希检查。

## MO2 安装

1. 关闭游戏，安装全部前置和服装。需要汉化时自行安装，并遵循汉化本身的覆盖顺序。
2. 安装 **Tailor-Devious-Device-Compatibility-Patch-0.6.4-licenses.1-FOMOD.zip**，右侧选择“中文”或“English”。左侧图片显示“你用什么语言？”和“What language do you use?”。这里只选择补丁新增文字的语言，不会安装 Tailor 汉化。
3. 禁用旧的独立 Chinese/English 补丁，以及曾替换 Tailor.dll 的旧实验版本，只启用一个新版补丁。从本地测试版升级时，禁用旧语言模组也可避免其临时启动诊断工具与新版一起载入。**仅更新 DLL 时，保留现有匹配的 Generated Data，不要为了更新而删除或重新编号。**
4. 首次安装需要生成装备数据：在 MO2 选好要游玩的配置，启用服装及插件，再关闭 MO2 以保存列表。通过 Windows 终端运行补丁自带工具，替换为自己的路径；不要通过 MO2 虚拟文件系统运行：

   ```powershell
   py -3 Tools/build_cosmetics.py --mo2 "D:/YourMO2" --profile "Default" --output "D:/TailorDDGenerated" --zip
   ```

5. 将生成的 ZIP 作为单独模组安装启用，并在右侧启用 **TailorDDCosmetics.esp**。ESP、对应 JSON 和报告必须来自同一份生成结果；阅读报告确认哪些设备被纳入或排除。
6. 通过 MO2/SKSE 进入游戏，用自己的快捷键（通常是 Shift+Z）打开 Tailor，再打开 **设置 → DD 设置**。开关默认关闭。

建议左侧顺序：

```text
Tailor
Tailor 汉化（可选，自行下载）
Tailor Devious Device Compatibility Patch 0.6.4
Tailor Devious Device Compatibility Patch - Generated Data
```

补丁只提供自己的 TailorDDBridge.dll 和 TailorDDFreeWear.pex，不应被旧补丁覆盖。不替换 Tailor.dll、DD、Mu 或汉化文件。右侧需要启用的是生成的 TailorDDCosmetics.esp。

## 开关行为

- **试衣过程中：**受支持的 DD 使用可自由穿脱的外观副本。需要通过 Tailor 的 Set/确认分配流程并正常退出；单独添加“+”或预览不等于确认套装。
- **关闭（默认）：**确认退出后使用真实 DD 装备，保留音效、动画等原有效果；补丁对自己管理的“角色—设备”组合授予自由穿脱权限，在 Tailor 和普通物品栏均可更换。
- **开启：**确认退出后遵守真实 DD 原有穿脱限制，装备冲突、脚本和锁定规则仍由 DD 处理。
- 要改变已经分配套装的处理方式，请调整开关后重新确认这套衣服。关闭不是全局解锁所有既有 DD 的命令。
- 只处理受支持且由补丁管理的通用设备。不会禁用声音减弱、音效、动画等其他 DD 特性，不强制解除任务设备、阻止通用卸装的设备、未知自定义脚本或其他玩法独立锁上的设备。

## 装备数据、升级与存档

生成器根据实际胜出的松散 PEX 继承关系和库存/渲染设备配对判断 DD 设备，不根据“catsuit”等名字判断。第三方服装可能被纳入；仅调用部分 DD 函数并不等于自动兼容。BSA 内脚本、未知继承关系和被排除的自定义设备需要单独核查。

启用的插件、脚本胜出文件或 MO2 配置改变后，需要重新生成。务必沿用**同一个输出目录及 manifest**，以保留已分配的 FormID，再更新生成数据模组。不同配置使用不同生成结果。不要把个人生成 ESP 或 manifest 分发给其他玩家。

从 0.4 或本地 0.6.x 升级，可以保留现有匹配的 schema-2 Generated Data。0.4 已分配的外观副本不会自动获得真实 DD 效果；只需进入 Tailor，设置开关，重新确认需要转换的套装并正常退出，不需要清空所有 NPC 分配。

保留 Tailor 原有跨存档共享 NPC 套装行为，不新增按存档隔离或清空旧分配。补丁自由穿脱权限使用自己的 SKSE 配套存档记录，请将游戏存档与对应 co-save 一起保留；这不改变 Tailor 自身的套装存储方式。

更新先备份存档。禁用 DLL 不会自动移除真实 DD 或解除其锁定。卸载时优先回到使用补丁和生成插件之前的存档，不提供清档或强制解锁工具。

## 本版修复与验证范围

0.6.4 保留自由穿脱模式下的真实 DD 效果，修复穿戴未完成、异步卸装处理、Mu/RaceMenu 预设路径扩展名及卸装后的外观漏检查。首次头部被遮挡时，不再一直跳过之后露脸时的恢复；检查也包含脸部染色贴图和着色变化。

本地测试者已反馈 DD 换装及“全包头 DD → 露脸 DD”后的 Mu NPC 外观恢复成功，离线接口、任务顺序和预设路径检查通过。这不代表所有玩家/NPC、设备、语言、游戏版本和模组组合均已测试。

报告问题时附游戏/SKSE/Tailor/DD NG 版本、玩家或 NPC、开关状态、具体确认退出步骤、装备名称及 `Documents/My Games/Skyrim Special Edition/SKSE/TailorDDBridge.log`。Mu 外观问题同时附相关 RaceMenu/Mu 日志，公开前检查个人信息。

## 内容与致谢

包含独立 DLL、自有辅助 PEX、双语 FOMOD、生成工具、双语说明及许可证。不包含 Tailor 本体/字体、他人汉化、DD/Mu/RaceMenu 实现及资源、个人预设/生成装备数据、存档、日志或临时崩溃诊断工具。

感谢 ColdSun / heathbrownkeyworks 的 Tailor 和 API、DD/DD NG 开发者、Mu 作者 Acro、RaceMenu 作者 expired6978，以及独立发布 Tailor 汉化的 Llinnn。相关作品均需自行下载。本补丁为非官方兼容扩展。

对应源码及构建说明见 **Source.zip** 和 [GitHub](https://github.com/Tiger11011/Tailor-Devious-Device-Compatibility-Patch)。Source.zip 不是 MO2 安装包。补丁使用 GPL-3.0-or-later，详见 LICENSE 与 THIRD_PARTY_NOTICES.txt。
