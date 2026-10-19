# 0.6.6 validation / 验证记录 — 2026-10-19

The maintainer tested the same Mu-replaced NPC through repeated DD outfit changes, including fully covered headgear, face-visible equipment and ordinary outfits. The loaded bridge identifies itself as 0.6.6; it retains the earlier local-test diagnostic banner. Release preparation does not rebuild it.

- 2,442 supported device pairs loaded automatically from 17,000 named armor records; 36 excluded forms.
- All 11 removal requests reached `DD removal completed` and `success=true`. That helper returns true only when inventory/render armor are both unworn **and the DD removal token is zero**.
- All 14 DD equip callbacks succeeded. No existing-token refusal, removal wait expiry, deferred removal timeout or failed callback occurred.
- DD's log records the previously failing catsuit fallback branch completing on two occasions; thus this was not merely a test avoiding that branch.
- Two selected-face restore callbacks succeeded. The user reports stable appearance. The new session has 30 appearance notifications; the earlier failing session had 117. Counts alone do not prove correctness: the deployed DLL hash was matched to the reviewed code, whose notification entry checks geometry identity and suppresses unchanged sets; older check chains are superseded. Legitimate geometry updates still produce notifications. Full geometry sets are not logged, so each individual event cannot be independently reconstructed from logs.
- This confirms the reported local scenario, not all actors/devices, languages, game versions or old corrupted saves. Saved pre-existing DD tokens are not forcibly erased.

DLL SHA-256: `d9f3188a177b2cf3a5fb930ff6dae29b17120f6e4cbec6920ce3296d00566811`

Helper PEX SHA-256: `8dd73193e0a3fd7eb162e98eda120ebcb0f47419a105943af65f7f1e2c7b3280`

Pool ESP SHA-256: `e5cb1696d58abcb67051504078a455600360fdbef55eb01cd4a0529dda0bcfb9`

本轮通过日志确认 11 次脱卸、14 次穿戴全部成功，脱卸成功条件包含 token=0；此前出错的 DD 分支也再次执行。两次外观恢复成功，用户确认画面正常。广播仍用于真实节点变化，不要求数量为零；仅凭次数不能证明逐次合理，已同时核对部署二进制及去重代码。没有改写 Tailor、DD、Mu 或他人汉化。
