# 0.6.14 validation / 验证范围

## English

The 2026-10-21 local test passed the reported fitting sequence: remove a genuine arm restraint, switch to ordinary clothing, then preview Pet gear. The user reported normal arm poses and Pet movement. This release reuses that tested patch DLL without recompiling:

`e34a3e1cbf44508b268b5f058baa4741fedc2cb1ed6bd1ab1ad9989a6230d4f2` (SHA-256)

The test used Skyrim 1.6.1170, a supported Tailor/DD NG setup, Open Animation Replacer 3.1.6 and the reviewed optional Partial Animation Replacer 0.2.0. This is a sanitized summary of the frozen local evidence; private actor identifiers, outfit/preset names, original logs and local paths are not distributed.

| Observation | What this test supports |
| --- | --- |
| PAR adapter passed its exact-version checks and activated for the fitting NPC. | The adapter actually ran in the tested session. |
| At least 64 cached applications reported `currentAllowed=false; stale bone override skipped` during restraint removal and the ordinary-clothing transition. | A stale pose override was actively skipped, consistent with the user's observation that the arms recovered. |
| The restraint inventory/render pieces were removed, the helper succeeded, and DD selected its unbound-animation branch. | The recorded restraint removal completed. |
| The ordinary outfit initialized all 9/9 pieces and reached zero incomplete pieces; an older pending outfit request was discarded. | The observed new request completed without a late obsolete request replacing it. |
| Five clip snapshots showed the expected single active Pet idle from the installed DD OAR replacement, including a snapshot after a loop. | The sampled Pet binding stayed on the intended clip; no arm-restraint or HH clip appeared in those snapshots. |
| Five automatic `PickNewIdle` requests were handled; Pet conditions matched 767/767 times over about 16 seconds. | The selected Pet preview and its idle protection were exercised for that interval. |
| PAR/OAR preview scopes cleared on exit; genuine Pet inventory/render pieces equipped and the helper succeeded. | The observed exit cleanup and real-device equip completed. |

No PAR actor/root identity mismatch, adapter-entry installation failure, version/byte-check failure, helper failure or equip/removal timeout was recorded in this test. A protected generic-blocked device retained its cosmetic preview as designed.

**Limits:** 64 is the rejection-message cap, so it proves at least 64 skips, not exactly 64 checks. Reaching the cap does not stop condition checking or skipping; missing later messages do not prove the stale cache disappeared or that no later skips occurred. The log identifies a cached object, not its configuration filename, and does not record final matrices for each bone. The five clip snapshots are samples, not a continuous capture of every animation frame.

This session did not include a `currentAllowed=true` sample. Valid-cache preservation, player behavior and unrelated-NPC behavior were reviewed in code, rather than individually demonstrated by this runtime trace. Existing DD initialization messages about `ResetLockShield`/None and `struggleIdlesHob` remained in Papyrus; the observed helper operations succeeded, and this test does not establish that the whole mod setup is script-error-free. Other equipment, actors, runtimes/VR, languages, save/load and cross-save persistence were not exhaustively tested.

**Retained startup wording:** the tested DLL still contains the build-time diagnostic text “0.6.14 local test” / “runtime verification pending”. The later 2026-10-21 test described above completed successfully. The old wording does not indicate a different binary version or an outstanding test of this reported sequence; it remains unchanged to preserve the tested DLL hash.

## 中文

2026-10-21 本地实测通过了用户报告的试衣流程：脱下真实臂缚、切换普通衣服、再预览 Pet 装备。用户确认手臂姿势和 Pet 动作正常。本次正式发布沿用这份实测补丁 DLL，不重新编译，SHA-256 为：

`e34a3e1cbf44508b268b5f058baa4741fedc2cb1ed6bd1ab1ad9989a6230d4f2`

测试使用 Skyrim 1.6.1170、受支持的 Tailor/DD NG 环境、Open Animation Replacer 3.1.6 及已核查的可选 Partial Animation Replacer 0.2.0。本文为冻结本地证据的脱敏概述，不公开角色标识、套装/预设名称、原始日志或私人路径。

| 观察结果 | 本次能够确认的结论 |
| --- | --- |
| PAR 适配通过精确版本检查，并对试衣 NPC 启用。 | 适配器在本轮实际运行。 |
| 臂缚脱卸及切换普通衣服期间，至少 64 条缓存应用记录为 `currentAllowed=false; stale bone override skipped`。 | 实际跳过了已失效的姿势覆盖，与用户观察到的手臂恢复一致。 |
| 臂缚库存件和显示件均脱下，辅助函数成功，DD 执行恢复无拘束动画分支。 | 记录中的臂缚脱卸完成。 |
| 普通套装 9/9 件初始化，稳定后 0 件未完成；等待中的旧套装请求被丢弃。 | 观察到的新请求完成，没有被迟到的旧请求覆盖。 |
| 5 次动画快照均为已安装 DD OAR 替换提供的单个预期 Pet 待机，包含一次循环后的快照。 | 采样时实际绑定保持正确，这些快照中没有臂缚或 HH 动画。 |
| 5 次自动 `PickNewIdle` 请求被处理，约 16 秒内 Pet 条件 767/767 命中。 | 本轮实际执行了 Pet 预览及自动待机保护。 |
| 退出时清除 PAR/OAR 预览范围，真实 Pet 库存件和显示件穿戴，辅助函数成功。 | 观察到的退出清理和真实设备穿戴完成。 |

本轮没有 PAR 角色/根节点身份不符、适配入口安装失败、版本/字节检查失败、辅助函数失败或穿脱超时记录。带通用卸装保护的设备按既定策略保留外观副本。

**结论边界：**64 条是拒绝日志上限，只能证明至少 64 次跳过，不是全部检查次数。达到上限后仍继续检查/跳过；之后不再打印，不能解释为缓存消失或之后没有拦截。日志记录缓存对象，未直接确认其配置文件名，也未逐根记录骨骼最终矩阵。5 次动画快照是采样，不是每帧连续记录。

本轮没有 `currentAllowed=true` 样本；有效缓存保留、玩家及其他 NPC 的分支依赖代码检查，没有宣称本次均逐一实测。Papyrus 仍有既有 DD 初始化阶段的 `ResetLockShield`/None 和 `struggleIdlesHob` 记录；本轮辅助穿脱均成功，不代表整个模组环境的所有脚本均无错误。未全面验证其他装备、角色、游戏版本/VR、语言、读档或跨存档持久化。

**保留的启动文字：**实测 DLL 中仍有构建时的“0.6.14 local test”和“runtime verification pending”诊断文字。后续 2026-10-21 测试已按上文完成；旧文字不表示另一个二进制版本，也不表示本次报告流程仍待验证。为保持实测 DLL 哈希，发布整理不改动这段文字。
