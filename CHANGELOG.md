# Changelog

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
