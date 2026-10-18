# Tailor Devious Device Compatibility Patch 0.6.4

Local testing confirmed DD clothing switching and Mu NPC appearance recovery when changing from fully covering DD headgear to face-visible equipment.

- **FOMOD.zip**: main MO2 installer; choose 中文 or English. Default Off.
- **Source.zip**: corresponding source and dependencies; not an MO2 installation.
- **SHA256SUMS.txt**: archive checksums.

Off retains genuine DD effects after Set and normal exit while permitting supported managed clothing changes through Tailor and the normal inventory. On preserves normal DD clothing locks. Previews remain removable appearance copies.

Fixes include DD script binding/order, incomplete equip handling, Mu/RaceMenu preset paths, and missed restoration after delayed removal or initially hidden heads. Tailor's original shared NPC assignments remain unchanged.

Requires the exact reviewed Tailor 3.0.1 and DD NG v0.4.3 rc2 builds, matching game/SKSE/Address Library, DD dependencies, PapyrusUtil and profile-specific Generated Data. Mu/RaceMenu are optional for the reviewed Mu integration. Check README hashes; this is not universal runtime or DD NG support.

Tailor, third-party translations, DD/Mu/RaceMenu implementations/assets, personal presets/generated data and startup diagnostic tools are not included. Existing matching schema-2 Generated Data can be retained; do not renumber it merely to update the patch.

**中文：**已获本地实测成功反馈。关闭时保留真实 DD 效果并允许受支持装备自由穿脱；开启保留原穿脱规则。FOMOD 自选补丁语言，Tailor 汉化请自行下载。更新保留匹配的 Generated Data，禁用旧独立中英文测试补丁，避免重复加载及残留临时诊断工具。

Offline checks passed. A complete device/player/NPC/runtime/language compatibility matrix is not claimed. See bilingual installation instructions.
