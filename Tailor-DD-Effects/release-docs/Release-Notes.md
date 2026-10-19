# Tailor Devious Device Compatibility Patch 0.6.6

Automatic equipment setup and the tested DD removal / Mu notification fixes.

- Install the FOMOD, choose 中文 or English, and enable **TailorDDAutoFitting.esp**. No Python, external generator or preliminary enter/exit cycle. New supported clothing is detected next launch.
- Keep existing legacy Generated Data if your saves/outfits use it. Back up **TailorDDBridge.AutomaticSlots.json** with your saves; do not delete it or compact/renumber the plugin.
- Fix DD removal-token leaks and redundant Mu notifications while retaining DD effects, selected-face recovery and the default-Off switch. Local logs show 11/11 removals and 14/14 equips successful; see VALIDATION-0.6.6.md for scope/limits.
- Exact tested DLL/PEX retained. Tailor, translations, DD/Mu assets and personal data are not bundled. Permission record and existing licenses retained; permission screenshot is source-only.
- **FOMOD.zip** is the player installer. **Source.zip** is the complete corresponding source including eight verified dependency archives; do not install Source.zip in MO2. SHA256SUMS.txt verifies both.

中文版：安装 FOMOD 选语言并启用随包 ESP 即可正常使用，无需生成器或先退出一次游戏；新增受支持服装下次启动自动识别。已有旧 Generated Data 保留，备份自动编号 JSON，不要重新编号 ESP。已通过本轮 DD 穿脱及 Mu 外观测试，发布包保留同一份实测二进制。日志重启前复制；旧存档已有的 DD 标记不会强制清除。
