# Copyright and license attribution

## Tailor-derived interface declarations

**Copyright (c) 2026 ColdSun.**

The Tailor interface declarations and their upstream explanatory comments in:

- `Tailor-DD-Effects/src/TailorInterface1.h`
- `Tailor-DD-Bridge/src/TailorInterface1.h` (retained historical source)

originate from [heathbrownkeyworks/Tailor](https://github.com/heathbrownkeyworks/Tailor). The original author retains copyright in that material. This project does not claim authorship of those declarations or comments.

The current upstream license was verified against Tailor 3.0.1 commit [`a6088e22b06f6e97ab617d2f79420d6a99228f52`](https://github.com/heathbrownkeyworks/Tailor/commit/a6088e22b06f6e97ab617d2f79420d6a99228f52), dated 2026-10-07. The implementation is GPL-3.0-or-later with the supplied upstream exceptions. The separately distributable `docs/api/TailorAPI.h` is also offered under MIT. We retain that MIT copyright/permission text for the corresponding public interface material; this does not describe all of Tailor as MIT-licensed.

The complete, unmodified upstream notices are under `Tailor-DD-Effects/licenses/` in source and under `licenses/` in the installer:

- `Tailor-GPL-3.0.txt`
- `Tailor-LICENSING.md`
- `Tailor-EXCEPTIONS.md`
- `TailorAPI-MIT.txt`
- `Tailor-THIRD_PARTY_NOTICES.md`
- `LICENSE-SOURCES.json` — exact upstream revision, original URLs, retrieval date and file hashes.

Relative links inside the verbatim upstream notices refer to the upstream repository layout. The manifest maps each copied file to its original path. Upstream notices about Tailor's fonts, preview assets or binaries are included as provenance; those assets and binaries are not part of this add-on.

## Independent patch and other dependencies

This independent compatibility patch is maintained by Tiger11011 and distributed under **GPL-3.0-or-later**. It is an unofficial add-on, not a Tailor release or an endorsement by the upstream author. Functional changes are recorded in CHANGELOG.md; corresponding source and build instructions accompany each binary release.

CommonLibSSE-NG, MinHook, Dear ImGui, FreeType and the other included dependencies retain their own copyright notices, licenses and applicable exceptions. See THIRD_PARTY_NOTICES.txt and the individual texts under `licenses/` in the installation package.

The included `TailorDDFreeWear.pex` is this patch's helper. Original Tailor, DD, Mu, RaceMenu and third-party translation implementations/assets must be obtained separately and are not redistributed here.

## License and source packaging revisions

**2026-10-18 — package 0.6.4-licenses.1.** Added attribution, verbatim upstream license files and source-distribution directions.

**2026-10-18 — package 0.6.4-sources.1.** Adds eight exact dependency source archives, retained build recipes/configurations, configured-header copies and the zlib notice. See SOURCE-ACCESS.md and dependency-sources/README.md. The tested 0.6.4 DLL, helper PEX, functional C++/Papyrus sources, default switch state and language choices are unchanged. Earlier tags and releases remain intact.

**2026-10-18 — package 0.6.4-sources.2.** Corrects missing PapyrusUtil SE credits (exiledviper and meh321) in the notices and bilingual documentation. No runtime binary, functional source, FOMOD choice or default was changed.

The same notice revision includes CREDITS.md, PREREQUISITE-NOTICES.md and RESOURCE-AUDIT.json covering the 28 reviewed runtime/library/resource/reference components and their individual attribution/permission boundaries. Embedded and historical code notices remain their original authors' terms. The maintainer confirms compatibility bug-fix purpose and relies on DD's shipped bug-fix exception; no additional individual author approval is asserted.
