# Tailor Devious Device Compatibility Patch 0.6.14

## English

0.6.14 improves DD fitting poses when switching from restraint gear to ordinary clothing and Pet gear. It rechecks stale optional PAR pose overrides for the current fitting NPC, discards superseded outfit requests, and protects the selected Pet preview from automatic idle selection. Preview scope is cleared on exit. The patch still relaxes only supported clothing locks when the switch is Off; DD animations, sounds and Tailor's own cross-save outfit behavior remain.

- Install the bilingual FOMOD and enable **TailorDDAutoFitting.esp**. New users need no Generated Data, Python, external generator or preliminary enter/exit cycle. Keep legacy Generated Data only when existing saves/outfits use it, and back up **TailorDDBridge.AutomaticSlots.json** with saves; do not compact or renumber the plugin.
- **Open Animation Replacer is required** for fitting-preview conditions; locally tested 3.1.6, public Conditions interface V3 (numeric API value 2). The optional PAR 0.2.0 adapter supports the reviewed DLL supplied with DD NG. Exact versions/hashes are listed in INSTALLATION-English.md; unsupported native builds do not enable the corresponding adapter.
- The 2026-10-21 test passed the reported restraint → ordinary clothing → Pet sequence by user observation. Logs show at least 64 stale PAR overrides skipped, five intended Pet clip snapshots, five `PickNewIdle` requests handled, and 767/767 condition matches over about 16 seconds. The observed equip/removal callbacks succeeded, an obsolete request was discarded, and preview scopes cleared. **64 is the log cap**, not all checks. See VALIDATION-0.6.14.md for limits; this is not a complete compatibility matrix.
- Release preparation changes documentation/packaging only and reuses the tested DLL and helper PEX without rebuilding. Patch DLL SHA-256: `e34a3e1cbf44508b268b5f058baa4741fedc2cb1ed6bd1ab1ad9989a6230d4f2`.
- The retained build-time startup wording “0.6.14 local test” / “runtime verification pending” predates the successful 2026-10-21 test. It does not identify another version or mean this reported sequence remains untested. It is preserved with the tested binary.
- **FOMOD.zip** is the player installer; **Source.zip** is the complete corresponding source, including eight resolved dependency sources, build records and the public OAR Conditions API/notices. Do not install Source.zip in MO2. ColdSun's permission record and existing licenses are retained; the permission screenshot is source/repository only. Tailor, translations, DD/Mu/PAR assets and personal data are not bundled. OAR JSON references installed DD animations. SHA256SUMS.txt verifies release archives.

## 中文

0.6.14 改善从拘束装备切换普通衣服、再切换 Pet 装备时的试衣姿势。对当前试衣 NPC 重新核查可选 PAR 的姿势缓存，跳过已失效的覆盖；丢弃被新请求替代的旧套装请求，并保护已选择的 Pet 预览不被自动待机替换。退出时清除预览范围。开关关闭时仍只放开受支持衣服的穿脱限制，保留 DD 动画、音效及 Tailor 自身的跨存档套装功能。

- 安装双语 FOMOD 并启用 **TailorDDAutoFitting.esp**。新用户不需要 Generated Data、Python、外部生成器或先进入再退出一次游戏。旧存档/套装已使用的 Generated Data 保留；随存档备份 **TailorDDBridge.AutomaticSlots.json**，不要压缩或重新编号插件。
- 试衣预览条件需要 **Open Animation Replacer**，本地实测 3.1.6，使用公开 Conditions 接口 V3（API 数值为 2）。可选 PAR 0.2.0 适配支持 DD NG 随附的已核查 DLL；精确版本及哈希见 INSTALLATION.zh-CN.md，未知原生构建不启用对应适配。
- 2026-10-21 用户目测确认拘束装备→普通衣服→Pet 流程正常。日志记录至少 64 次跳过失效 PAR 覆盖、5 次预期 Pet 动画快照、5 次 `PickNewIdle` 被处理、约 16 秒内条件 767/767 命中；观察到的穿脱回调成功，旧请求被丢弃，退出范围清理正常。**64 条是日志上限**，不是全部检查次数。详细边界见 VALIDATION-0.6.14.md，不宣称全面兼容矩阵。
- 发布整理仅更新文档与打包，沿用实测 DLL 和辅助 PEX，不重新编译。DLL SHA-256：`e34a3e1cbf44508b268b5f058baa4741fedc2cb1ed6bd1ab1ad9989a6230d4f2`。
- DLL 保留构建时的“0.6.14 local test”/“runtime verification pending”启动诊断文字，后续 2026-10-21 测试已完成。旧文字不表示另一个版本，也不表示本次报告流程仍未测试；随实测二进制保留。
- **FOMOD.zip** 是玩家安装包；**Source.zip** 是完整对应源码，含 8 项实际依赖源码、构建记录及公开 OAR Conditions API/许可声明，不要安装到 MO2。保留 ColdSun 授权记录与原许可证，授权截图仅收入源码包/仓库。不随包分发 Tailor、他人汉化、DD/Mu/PAR 资源或个人数据；OAR JSON 引用已安装的 DD 动画。发布附件由 SHA256SUMS.txt 校验。
