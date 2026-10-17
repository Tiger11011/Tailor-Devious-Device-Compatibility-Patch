# Tailor Devious Device Compatibility Patch 0.4 — 实验版

独立 SKSE 兼容插件，用于在 Tailor 内试穿受支持的 Devious Devices 装备。打开 Tailor → **设置 → DD 设置**。试衣时使用可以自由穿脱的外观副本，普通衣服沿用 Tailor 原有流程。本下载已包含最新的按钮文字居中调整。

## 前置要求：均需自行下载安装

1. **Tailor 3.0.1** 及其自身前置：https://www.nexusmods.com/skyrimspecialedition/mods/173691
2. 与游戏版本匹配的 **SKSE64**：https://skse.silverlock.org/
3. 与游戏版本匹配的 **Address Library for SKSE Plugins**：https://www.nexusmods.com/skyrimspecialedition/mods/32444
4. 希望试穿的 DD 装备模组、对应的 Devious Devices 框架／脚本及其全部正常前置。生成数据前，应在所选 MO2 配置中正确安装并启用这些模组和插件。
5. **Mod Organizer 2** 和 **Python 3.10 或以上**。Python 仅在生成数据时使用，游玩时不需要运行。生成器只用 Python 标准库，无需安装 pip 包、SSEEdit 或 .NET。
6. 原生 SKSE 插件所需的 **Microsoft Visual C++ x64 运行库**。

本实验版仅接受以下两份已经检查的 Tailor.dll：

| Tailor 版本 | SHA-256 |
| --- | --- |
| 英文 3.0.1 | `261f415773d550e16809cbdfa39863451ea3eeba247e30b996afabe1e1d47850` |
| 支持的独立下载 CHS 3.0.1 | `83dc9c56627d163e00eb9e5f1cceb1af3edb84deaa286ec828c95fb2a0d0df4d` |

仅版本号相同不足以保证兼容。其他 Tailor 构建、其他汉化和 Skyrim VR 不属于本版支持范围。SE／AE 用户仍须满足 Tailor 的运行环境要求，不能将本包理解为已经测试所有游戏运行版本。遇到未知 Tailor DLL 会拒绝挂接。

## FOMOD 安装时选择语言

- **English**：本补丁新增按钮和解释显示英文。
- **中文**：只把本补丁新增按钮和解释显示为中文；需用户另行安装支持的 Tailor 汉化，以使用其中文字库：https://www.nexusmods.com/skyrimspecialedition/mods/184318

现在只需下载一个 FOMOD 安装包。安装器左侧显示“你用什么语言？”及“What language do you use?”，右侧选择 **中文** 或 **English**。公共 DLL 和工具只有一份，选择仅改变本补丁 JSON 中的语言。本包不会汉化 Tailor，也不包含他人的汉化文件。

如果已有与当前模组配置匹配的 Generated Data，继续保留并启用即可；仅更换安装器格式或界面语言不需要重新生成。安装器默认关闭真正 DD 激活，需要保留该开关偏好时请先备份本补丁 JSON。

## MO2 安装步骤

1. 退出 Skyrim。安装 Tailor 及其前置；使用中文时，让自行安装的受支持汉化按其要求覆盖 Tailor。禁用曾替换 Tailor.dll 的旧试验原型，以及已经撤回的 0.5 存档隔离实验版。
2. 将 **Tailor-Devious-Device-Compatibility-Patch-0.4-FOMOD.zip** 安装为独立 MO2 模组，在安装器中选择 **中文** 或 **English** 后启用。禁用以前单独安装的中英文主补丁，仅保留这一份安装。本补丁包含 `SKSE/Plugins/TailorDDBridge.dll`，不会覆盖 `Tailor.dll`。
3. 在 MO2 选择实际游玩的配置，启用希望扫描的 DD／服装模组和插件。可先右键本补丁 → **在资源管理器中打开**，取得安装文件夹。关闭 MO2，让配置列表写入磁盘。接下来在该补丁文件夹打开 Windows 终端，从 Windows 直接运行生成器，不通过 MO2 的虚拟文件系统启动。
4. 生成自己的装备数据，替换命令中的示例路径：

   ```powershell
   py -3 Tools/build_cosmetics.py --mo2 "D:/YourMO2" --profile "Default" --output "D:/TailorDDGenerated" --zip
   ```

   没有 `py` 时，改用 `python` 或 Python 3.10+ 的完整程序路径。MO2 路径应指向包含 `ModOrganizer.ini` 的实例目录；`--profile` 填实际配置名称，省略则使用当前选中的配置。输出使用独立文件夹，放在游戏 Data、MO2 mods 和 overwrite **以外**。自定义布局可使用 `--game-data`、`--mods-dir`、`--profiles-dir`、`--overwrite-dir`；运行 `--help` 查看。

5. 重新打开 MO2，将生成的 `D:/TailorDDGenerated.zip` 安装为第二个模组，例如 **Tailor Devious Device Compatibility Patch - Generated Data**。启用该模组及 `TailorDDCosmetics.esp`，把 ESP 排在它列出的全部前置插件之后。若左侧存在同名 ESP／映射文件，让这一对新生成文件胜出。主补丁与生成数据通常没有相互覆盖的运行文件。
6. 通过 MO2 中的 SKSE 启动游戏。使用 Tailor 配置的快捷键（默认 **Shift+Z**）或法术，进入 **设置 → DD 设置**。若提示不可用，查看游戏常规 SKSE 日志目录下的 `TailorDDBridge.log`。

**不要把别人生成的 ESP／映射当成通用数据下载或上传。** 它们依赖对应玩家启用的模组；因此 FOMOD 主安装包不含 ESP。已有游戏进度不要从零生成另一份映射：保留输出目录及 manifest，确保外观 FormID 稳定。

### 典型启用组合

```text
Tailor 原版
Tailor 汉化（可选，另行下载）
Tailor Devious Device Compatibility Patch（在 FOMOD 中选择语言）
Tailor Devious Device Compatibility Patch - Generated Data
```

Tailor 与汉化之间的覆盖顺序仍按它们自己的要求设置。本补丁不会替换二者的 DLL。右侧插件列表只需启用生成数据的 ESP；主补丁本身是 SKSE DLL。

## DD 设置开关说明

- 试衣期间：受支持的设备使用可以自由穿脱的外观副本。
- **“确认退出后启用真正 DD”关闭（默认）**：按 **Set／确认分配**并正常退出后保留外观副本。
- **开启**：按 Set 并正常退出后，尝试为当前实际穿着的已确认套装换上真正 DD。最后是否成功装备和锁定，由 DD 自己的限制、冲突检查、提示及异步脚本决定。
- 退出前关闭开关会取消待执行激活。Set 之后才开启，需要再次确认。分配给当前未生效情境的套装，不会立刻强制穿上。
- 在套装编辑器添加部件或预览，不等于已经通过 Set 确认 NPC 的套装分配。
- 已经锁定的真正 DD 不会自动解锁。此开关不是紧急解锁工具；它只处理试穿／装备，不负责声音减弱、动画或其他 DD 效果。外观副本不复制原设备的全部脚本效果。

**保留 Tailor 原有的 NPC 分配跨存档共享行为。** 本版不提供 NPC 按存档隔离，不清空旧分配，也不迁移历史分配。

## 兼容范围、更新与卸载

通过实际胜出的散装 PEX 脚本继承及已验证的 DD inventory／render 关联识别装备，不按服装名称判断。第三方装备使用同一已验证协议时也可能被识别；仅自行调用 DD 的独立脚本、仅位于 BSA 的脚本继承、带脚本的渲染设备等不保证支持。请查看本地生成报告中的排除项目，不要认为所有标有 DD 的服装都已覆盖。

启用的源插件、脚本覆盖关系或模组配置变化后，需要重新生成。务必使用**同一个输出目录**，再替换 MO2 生成数据模组里的 ESP 与映射文件。已有编号保留，删除编号不会另给其他服装使用。运行时哈希检查会拒绝发生变化的输入，不要绕过检查；不同模组配置应保留各自的数据输出。

重装主补丁前可备份自己的 JSON，以保留开关偏好；下载包默认关闭真正 DD 激活。更换语言时重新运行 FOMOD 安装，仅启用一份主补丁。

先备份并使用测试存档。存档可能已经引用生成的 ESP；卸载时应回到使用本补丁及生成数据之前的存档。仅禁用 DLL 不会删除真正 DD 或解除锁定。本包不包含清理存档或强制解锁功能。

## 状态、包内内容与署名

这是实验版。本机 NPC 测试已反馈受支持 DD 外观副本可以自由切换，但尚未完成玩家／NPC、中英文及所有运行环境的完整测试矩阵。最新按钮居中改动已编译，仍待游戏内目视确认。反馈问题时请说明游戏／SKSE／Tailor 版本、DD 装备来源、玩家还是 NPC、开关状态、Set 和退出步骤，并附相关 Tailor／补丁日志。

包含：我们编写的独立 DLL、默认配置、两个 Python 生成工具、中英文说明及许可证。排除：Tailor.dll、Tailor 本体 ESP／资源／字体、第三方汉化文件、DD 脚本和资源、个人生成数据、个人配置、存档及日志。

感谢 ColdSun／heathbrownkeyworks 制作 Tailor 及其 API；感谢 Llinnn 提供另行发布的汉化，本包不重新分发该汉化。另附 **Source** 对应源码包，含补丁／生成器源码及构建输入，供开发者使用，不作为 MO2 模组安装。许可证与依赖声明见 `LICENSE` 和 `THIRD_PARTY_NOTICES.txt`。本项目是非官方兼容扩展。
