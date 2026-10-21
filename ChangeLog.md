# Changelog / 更新日志

## 0.6.14 — 2026-10-21

Includes changes developed in local test builds 0.6.7–0.6.13. / 包含 0.6.7–0.6.13 本地测试阶段积累的修复。

### English

- Added restraint-pose previews for supported DD equipment inside Tailor, using Open Animation Replacer (OAR).
- Fixed stale arm-restraint poses after removing equipment and switching to ordinary clothing or Pet gear, with the supported optional PAR adapter.
- Fixed Pet previews returning to a standing idle while still selected.
- Fixed delayed outfit requests overriding newer selections; improved cleanup when switching outfits, changing targets or exiting Tailor.
- Improved body-colour recovery for supported Mu Dynamic NPC Replacer appearances after loading a save.
- Fixed crash risks in animation-data reading and certain DD equipment checks.

Updated bilingual instructions, FOMOD and complete source/licensing files. Release packaging reuses the tested runtime files without rebuilding. OAR is required for preview poses; PAR support is optional and limited to the reviewed build.

### 中文

- 新增受支持 DD 装备在 Tailor 内的拘束姿势预览，使用 Open Animation Replacer（OAR）。
- 修复脱掉臂缚后切换普通衣服或 Pet 装备时，手臂拘束姿势仍残留的问题，适用于受支持的可选 PAR 适配。
- 修复 Pet 仍被选中时，预览角色自动从爬行变回站立的问题。
- 修复延迟执行的旧套装请求覆盖新选择，改善换装、切换目标及退出 Tailor 时的清理。
- 改善读档后受支持 Mu Dynamic NPC Replacer 外观的身体肤色恢复。
- 修复动画数据读取及部分 DD 装备检查中的崩溃风险。

同步双语说明、FOMOD、完整源码及许可文件；发布打包沿用实测运行文件，不重新编译。姿势预览需要 OAR；PAR 适配可选，仅支持已核查构建。

**Unchanged / 保持不变：** Default Off, genuine DD sounds/effects, separate Tailor translations and Tailor's shared outfit assignments. / 开关默认关闭，保留真实 DD 音效等效果、独立汉化兼容及 Tailor 自身的跨存档套装机制。

The reported NPC switching sequence passed local testing; this is not a complete compatibility matrix. / 本地实测通过了已报告的 NPC 换装流程，不代表所有装备与环境均已验证。See / 详见 [validation](VALIDATION-0.6.14.md) and [installation requirements](INSTALLATION-English.md) / [安装要求](INSTALLATION.zh-CN.md)。

<details>
<summary>Earlier releases / 历史版本</summary>

## 0.6.6-credits.2 — 2026-10-19

- Add a concise GPL-3.0-or-later statement and third-party license boundary to both README languages and Nexus description copies.
- Refresh matching installer/source documentation and source-access links only; all runtime files, settings, licenses and functional sources remain unchanged.

## 0.6.6-credits.1 — 2026-10-19

- Explicitly credit CommonLibSSE-NG, MinHook/HDE and Dear ImGui authors in both README languages and Nexus description copies.
- Synchronize installer/source documentation and source-access links. Tested 0.6.6 DLL, PEX, ESP, settings and functional sources remain byte-identical; no recompilation.

## 0.6.6 — 2026-10-19

- Automatic fitting setup on normal game launch: no Python, manual Generated Data or preliminary restart cycle. Newly installed supported clothing is discovered next launch; persistent identities and legacy Generated Data remain supported.
- Fix helper-owned DD removal-token leaks in the render-only fallback; block relocking while DD removal remains active.
- Gate Mu notifications on geometry changes and supersede older check chains, retaining selected-face restoration.
- Local runtime logs: 11/11 removals and 14/14 equips succeeded, with no token refusals/timeouts; two successful face restores. See VALIDATION-0.6.6.md.
- Publish the exact tested DLL/PEX with bilingual FOMOD, complete corresponding source and existing permission/license records.

## 0.6.4-permissions.3 — 2026-10-18

- Synchronize the maintainer's final English/Chinese publication copy into README and Nexus description files, preserving wording and restoring formatting/links.
- Retain detailed installation, generation, compatibility and diagnostic documentation in INSTALLATION-English.md / INSTALLATION.zh-CN.md, linked from README and included in the installer.
- Refresh source/FOMOD documentation only; permission screenshot remains source-only. Runtime and functional inputs are unchanged.

## 0.6.4-permissions.2 — 2026-10-18

- Refresh bilingual README, permission documentation and log/reporting instructions.
- Include the uploaded permission screenshot in Source.zip only; omit it from the FOMOD.
- Retain the tested DLL/PEX, functional source, language choices and default Off setting unchanged.
- Nexus descriptions are maintained separately by the maintainer; no Nexus description changes in this revision.

## 0.6.4-permissions.1 — 2026-10-18

- Record the maintainer's confirmation of ColdSun's permission to publish this Tailor compatibility patch.
- Add AUTHOR-PERMISSIONS.md to the repository, both installer language choices and corresponding source; synchronize bilingual README/Nexus descriptions and permission records.
- No original author message, grant date or unspecified conditions are invented. Existing licenses and all tested runtime/functional files remain unchanged.

## 0.6.4-sources.4 — 2026-10-18

- Final notice/source packaging revision: all 29 reviewed component/resource credits and notices retained.
- Restrict vendored CommonLib/OpenVR sources to actual build/implementation/header/license files; omit unrelated SDK/Unity/SDL examples, test address databases and debug binaries. Add a per-file source hash inventory.
- Accepted DLL, helper PEX, functional inputs and FOMOD behavior remain byte-identical.

## 0.6.4-sources.3 — 2026-10-18

- Complete the resource audit at 29 components; retain ProggyClean's Tristan Grimmer copyright and MIT license after confirming its existing compressed payload in the accepted DLL.
- Includes the complete prerequisite/embedded-code notices prepared in sources.2. Runtime and functional source remain byte-identical.

## 0.6.4-sources.2 — 2026-10-18

- Correct the omitted PapyrusUtil SE credits: exiledviper (maintenance/refactoring), meh321 (original version/idea).
- Explain the helper's StorageUtil use and separately installed prerequisite; PapyrusUtil implementation is not redistributed.
- Documentation/credit correction only; tested runtime, functional source, settings and language choices remain unchanged.
- Extend the review to 28 direct/runtime/library/resource/reference components; add full CREDITS, prerequisite permission evidence and machine-readable resource audit.
- Retain CommonLib legacy MIT, ImGui/STB, JSON embedded MIT/CC0, FreeType contributed-code notices and DD/NG license references. Record reliance on DD's bug-fix exception without claiming separate author approval.

## 0.6.4-sources.1 — 2026-10-18

- Source/notice packaging revision only; tested DLL, helper PEX, C++/Papyrus/generator sources, settings and FOMOD language choices are unchanged.
- Bundle eight exact upstream dependency source archives with SHA-256 values, retained xmake recipes/patches, sanitized package manifests and five installed configured-header copies.
- Record actual linked FreeType 2.14.1 and zlib 1.3.2; add the missing zlib license notice.
- Add SOURCE-ACCESS.md and a developer-only source retrieval/verification tool. Preserve earlier tags and binary assets.

## 0.6.4-licenses.1 — 2026-10-18

- License-only packaging revision; runtime remains the tested 0.6.4.
- Add ColdSun's copyright attribution and verbatim Tailor GPL, licensing, exception, API MIT and third-party notices from upstream commit a6088e2, with source URLs and hashes.
- Include COPYRIGHT.md in both FOMOD language installations and clarify corresponding-source download directions.
- Keep the accepted DLL, helper PEX, functional source, language choices and default Off byte-for-byte unchanged. Preserve the original v0.6.4 release/tag.

## 0.6.4

- Off retains genuine DD effects after confirmed exit while permitting supported managed clothing changes in Tailor and the normal inventory. On preserves normal DD restrictions.
- Wait for DD script binding, sequence managed equip/removal jobs and recover incomplete equip attempts through DD's normal API.
- Defer native equipment interception until needed and avoid initializing inventories during read-only checks.
- Optional reviewed Mu 2.1.4 adapter: correct preset paths, recheck after delayed DD removal, and recover previously hidden faces when visible.
- Observe face tint/shader changes and update the runtime version banner.
- Local tester confirmed DD switching and the Mu head-covering-to-visible-face appearance fix; full compatibility matrix remains unverified.
- Preserve Tailor shared NPC assignments, independent DLL, separate translation, bilingual FOMOD and default Off.
- Exclude temporary startup diagnostics and personal generated data from public downloads.

## 0.4 Experimental

- Independent SKSE bridge; does not replace Tailor.dll or third-party translations.
- Removable cosmetic copies while fitting supported DD equipment.
- Optional real-DD activation on confirmed exit; default Off.
- English/Chinese FOMOD language selection with a bilingual prompt.
- Centered DD Settings button label and shortened bilingual switch explanation.
- Preserves Tailor's normal shared NPC assignments across saves.
- Source and licenses included. Generated equipment data must be built locally.

Limited local testing; this is an experimental release. A complete runtime matrix and installer GUI verification are pending.

</details>
