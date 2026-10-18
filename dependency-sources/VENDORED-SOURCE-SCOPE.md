# Scope of CommonLib / OpenVR source delivery

The complete release Source.zip carries the actual library source/build files
needed by the documented **xmake** build, pinned to the recorded upstream commits.
It does not copy every unrelated file from the upstream SDK repositories.

- CommonLib: root project/build/license files and `include`, `src`, `cmake`,
  `scripts`, `res`, `licenses`. The retained configuration has `tests=false`.
- OpenVR: root project/build/license files and `headers`, `src`, `cmake`.
  CommonLib's xmake build uses its headers; no OpenVR sample binaries are linked.
- The eight other upstream dependency source archives remain original archives
  with their original notices. They are verified by the documented SHA-256 values.

CommonLib unit-test address databases, optional examples, Flash projects,
OpenVR/Unity/SDL examples, precompiled SDK/debug libraries, controller artwork
and sample textures are **not part of this Source.zip**. They are unnecessary
for the accepted build and are not asserted as new distributed resources of
this patch. The full upstream repositories remain available independently
under their respective terms. A Git submodule clone naturally differs from
this source distribution by also containing those upstream optional files.

`VENDORED-SOURCE-SHA256.json` inside Source.zip identifies every retained
CommonLib/OpenVR file and its unchanged upstream bytes. The patch's functional
inputs, DLL and helper PEX remain byte-identical. This source-distribution cleanup
does not rebuild the game plugin or alter any game behavior.

中文：源码包只保留实际构建所需的 CommonLib/OpenVR 源码、构建文件和许可；
剔除不参与构建的上游示例、测试地址库、Unity 工程、SDK 调试二进制和示例美术。
8 项依赖原始源码包仍保留完整上游通知。包内逐文件 SHA-256 可核对保留的
第三方源码，游戏功能和已测试二进制没有改动。
