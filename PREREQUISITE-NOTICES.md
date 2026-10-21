# Prerequisite notices and permission status / 前置许可与待确认事项

Updated **2026-10-21** for 0.6.14. This is an evidence record, not a new license for other
authors' mods and not a guarantee of legal or Nexus publication approval.
See CREDITS.md for attribution and RESOURCE-AUDIT.json for file/call evidence.

## Tailor author permission — confirmed by the maintainer

ColdSun has granted permission to publish this compatibility patch, as confirmed
by the maintainer on 2026-10-18. See AUTHOR-PERMISSIONS.md for the recorded scope
and evidence status. A permission screenshot is retained in the repository and Source.zip, excluded from the installer. This updates
Tailor's project-specific permission status; existing licenses and other authors'
permissions remain separate.

## Devious Devices: an unresolved publication question

The locally installed **DD 5.2** file
`Docs/Devious Devices/Devious Devices Readme.txt`, lines **129–146**, both
welcomes use as a dependency and restricts patch publication. In particular:

> You can NOT post or share any patches against this mod without my express permission, unless their sole purpose is fixing bugs.

The full local README's SHA-256 is
`b094d4e7531b6cd25e9e8c88428a091f02dae57dcdce672b8b80af709cb95ff2`.
The original permissions section is retained as a reference in
`licenses/DD-5.2-PERMISSIONS-reference.txt`. It is not represented as a
permission obtained by this project. The release page was not accessible to
the audit tool, so this is confirmed shipped-version evidence, not a claim
about the publication date or priority of an inaccessible newer statement.

The maintainer has confirmed that this project's sole purpose is fixing
Tailor/DD outfit-change compatibility. The existing switch controls that
compatibility handling; its supported actor/device scope and exclusions remain
documented in the README. On that stated purpose, this project relies on the
shipped terms' **bug-fix exception**. This records the maintainer's purpose and
interpretation; it is not a statement that DD's author has individually approved
the patch or confirmed the interpretation. No such individual written approval
has been verified. Credit/GPL files do not themselves supply that approval.

The reviewed **DD NG plugin source** separately has Apache-2.0 terms,
Copyright 2022 ponzipyramid, at
https://github.com/IHateMyKite/DeviousDevicesNG/blob/main/LICENSE . The retained
reference copy comes from commit `f521932f2c479dd0389c0ad3ee830cc5e86fa5bb`.
This does not establish Apache licensing for every DD/NG script, clothing
asset, animation or inherited DD 5.2 component. Our patch carries only its own
hooks and short runtime validation fingerprints; it does not distribute the
original DeviousDevices.dll or original DD script implementations.

**Publication basis:** compatibility bug-fix purpose confirmed by the maintainer,
with reliance on the upstream bug-fix exception. Do not describe this audit as
having obtained DD's separate written publication approval. Any later expansion
of purpose needs a fresh permission review. An author may interpret the scope
differently; retaining the terms and seeking clarification remains prudent.

## Other separately installed mods

0.6.14 optional PAR adapter: Partial Animation Replacer, naitro2010 and
contributors (https://github.com/naitro2010/PartialAnimationReplacer), is provided
by the user's separate DD NG installation. Only the reviewed PAR 0.2.0 binary
is accepted. Independently authored opaque call adapters do not copy PAR's
implementation or manipulate its C++ containers. No PAR DLL/PDB, source,
configuration, animations or bone-pose data are distributed. No general
redistribution permission is inferred. No additional runtime is added to the
installer. 中文：PAR 为用户另装 DD NG 已提供的可选组件；本补丁只做受版本检查
的运行时兼容，不随包分发其实现或资源，也不推定资源再分发许可。

| Project | Evidence and treatment |
| --- | --- |
| Tailor | Current upstream LICENSING.md, EXCEPTIONS.md, LICENSE, API MIT and THIRD_PARTY_NOTICES are retained verbatim at commit a6088e2. ColdSun's permission to publish this compatibility patch has been confirmed by the maintainer; see AUTHOR-PERMISSIONS.md. Existing license/platform obligations remain separate. No Tailor.dll or artwork/translation is redistributed. |
| PapyrusUtil SE | Official project page credits exiledviper and meh321. Our source has three minimal StorageUtil native signatures; our helper calls them in the separately installed runtime. No original implementation is copied. No unverified blanket redistribution license is claimed. https://www.nexusmods.com/skyrimspecialedition/mods/13048 |
| Mu / MDNR | Acro / Acro748. Its page permits credited modifications but restricts original-file uploads and asset use. Our optional interoperability adapter does not bundle Mu files or presets. Its current page version is not substituted for the already-tested supported 2.1.4 DLL. https://www.nexusmods.com/skyrimspecialedition/mods/150931 |
| RaceMenu | Expired / expired6978; PurpleLunchbox is also credited on the page. Its author instructions prohibit redistributing original files. We ship only our minimal compile-time CharGen signature, not original CharGen.psc/PEX, skee64.dll, presets or textures. https://www.nexusmods.com/skyrimspecialedition/mods/19080 |
| Tailor CHS | Llinnn. Page restrictions on uploads/modification/assets are respected by obtaining the translation separately. None of the translator's Chinese UI files is redistributed. Our compatibility data includes a supported DLL hash/short validation signatures, not a translated DLL. https://www.nexusmods.com/skyrimspecialedition/mods/184318 |
| SKSE64 | SKSE team; runtime separately installed, CommonLib supplies our compiled interface implementation. No original loader/runtime or SDK implementation is added to the installer. https://skse.silverlock.org/ |
| Address Library | meh321. Page terms distinguish uploads, modifications and asset use. We do not redistribute its address databases; CommonLib reads the user's matching installed database. https://www.nexusmods.com/skyrimspecialedition/mods/32444 |

The compile-only directory contains minimal signatures/empty compiler stubs,
not the third-party runtime implementations. Do not compile and distribute
their PEX outputs as replacement mods. No third-party preset, skin, face/head
mesh, armor mesh/texture, animation or sound is in the runtime installer.

## Fonts, sources and embedded code

The FOMOD language PNG is fixed text generated with Microsoft YaHei/Segoe UI.
Microsoft's Windows font FAQ permits this kind of phrase graphic under the
stated conditions, while prohibiting general font redistribution/bitmap-font
conversion. The existing PNG is unchanged; no font file or glyph atlas is
distributed. https://learn.microsoft.com/en-us/typography/fonts/font-faq

Upstream dependency source archives retain their own notices, including test
or example material. Inclusion in a developer source archive does not make all
that material a runtime dependency or relicense it under the patch's GPL.
Our runtime notices cover the libraries actually linked/incorporated, including
CommonLib's historical MIT attribution, MinHook/HDE, ImGui/STB, FreeType's
contributed code, JSON's embedded algorithms/Hedley, and the other dependencies
listed in CREDITS.md. Optional build components disabled in the recorded
configuration are not silently declared as required runtime mods.

## Nexus

Nexus has separate [file submission rules](https://help.nexusmods.com/article/28-file-submission-guidelines),
including approval expectations for content primarily interacting with other
users' content. Preserve written author confirmations; credits alone are not
permission. Do not claim endorsement or universal version compatibility. Use
appropriate AI/adult categorization under the current rules; no donation or
commercial eligibility is established by this source/credit audit.

## 中文

所有已确认的作者与通知已列在 CREDITS.md；没有把独立前置的许可擅自改成 GPL/MIT。
安装包不分发 Tailor/汉化、PapyrusUtil、DD/DD NG、Mu、RaceMenu 的原始实现或资产。

维护者已确认本项目的唯一目的是修复 Tailor/DD 换装兼容问题，本项目据此按
DD 5.2 自带条款的纯 bug 修复例外处理。该记录不是“已经取得 DD 作者单独
书面授权”的声明，也不能代替作者对例外范围的解释。保留原条款；后续若扩展
用途应重新审查。DD NG 插件的 Apache 许可不能代替全部 DD 资产/脚本的授权。
Tailor 作者 ColdSun 的发布许可现已由维护者确认取得，见 AUTHOR-PERMISSIONS.md；不将此记录扩展为其他作者的授权。

ImGui also retains its MIT-licensed ProggyClean default-font payload inside the accepted DLL. This is separate from the fixed-text Windows-font PNG and from Tailor's runtime fonts. Tristan Grimmer's copyright and full license are retained as ProggyClean-MIT.txt. No font payload or tested binary was changed.

## Open Animation Replacer public API

OAR is a separate runtime prerequisite for the fitting pose integration, by
ersh (ersh1) and contributors. Five unmodified public Conditions API support
sources are compiled into the patch. Their original GPL COPYING, upstream
EXCEPTIONS, source URLs and per-file SHA-256 values are retained in src/OAR and
the installer notices. No separate permissive license is inferred, and the OAR
DLL or DD animation assets are not redistributed. See CREDITS.md.
