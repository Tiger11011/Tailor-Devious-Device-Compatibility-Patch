"""Build public uploads from a strict allowlist. Does not change MO2 or game files."""
from pathlib import Path
import argparse,hashlib,json,zipfile,subprocess,sys,xml.etree.ElementTree as ET
root=Path(__file__).parents[1];workspace=root.parent
out=root/'releases/Tailor-Devious-Device-Compatibility-Patch-0.4';out.mkdir(parents=True,exist_ok=True)
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--dll',type=Path,default=workspace/'build/windows/x64/release/TailorDDBridge.dll')
parser.add_argument('--check-mo2',type=Path,help='Optional local installation comparison; does not modify the instance')
args=parser.parse_args();binary=args.dll
expected='e56e757673e30c40e02ddfa27b31364efc53fbcaf86cd44fee01d065e7d0b787'
def sha(b):return hashlib.sha256(b).hexdigest()
blob=binary.read_bytes();assert sha(blob)==expected
assert b'NPC save scope' not in blob
assert not list((root/'src').glob('NpcSaveScope*'))
if args.check_mo2:
    for lang in ['English','Chinese']:
        installed=args.check_mo2/'mods'/f'Tailor DD Bridge 0.4 - {lang}'/'SKSE/Plugins/TailorDDBridge.dll'
        assert installed.read_bytes()==blob
docs=root/'release-docs';common=root/'package/Common'
notices='''Tailor Devious Device Compatibility Patch 0.4 is distributed under GPL-3.0-or-later.
The companion Source archive contains the bridge and data-generator source.
The versioned C++ interface declaration derives from heathbrownkeyworks/Tailor (GPL-3.0); this is a declaration for inter-plugin calls, not Tailor's implementation or runtime assets.
CommonLibSSE-NG: GPL-3.0-or-later with the included Modding and Linking Exceptions.
Dear ImGui 1.92.6: MIT. FreeType: FTL. MinHook 1.3.4: BSD-2-Clause.
nlohmann/json 3.12.0 and spdlog 1.16.0: MIT.
DirectXMath and DirectXTK: MIT. rapidcsv: BSD-3-Clause. OpenVR: Valve BSD license.
Full notices and licenses are in licenses/.
Tailor, its translations and all DD frameworks/assets must be obtained separately.
No Tailor.dll, third-party translation, game font, DD script/mesh/texture, or personal generated plugin is distributed here.
'''.encode()
licenses={f'licenses/{p.name}':p.read_bytes() for p in (common/'licenses').iterdir() if p.is_file()}
licenses['licenses/OpenVR-BSD.txt']=(workspace/'Tailor-DD-Compat/lib/commonlibsse-ng/extern/openvr/LICENSE').read_bytes()
base={'SKSE/Plugins/TailorDDBridge.dll':blob,'LICENSE':(common/'LICENSE').read_bytes(),'THIRD_PARTY_NOTICES.txt':notices,**licenses}
for n in ['README-English.md','README-中文.md']:base[n]=(docs/n).read_bytes()
for n in ['build_cosmetics.py','pex_metadata.py']:base['Tools/'+n]=(workspace/'Tailor-DD-SafeFitting/tools'/n).read_bytes()
manifest=[]
def writezip(path,files):
    with zipfile.ZipFile(path,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for n,b in sorted(files.items()):
            info=zipfile.ZipInfo(n,date_time=(2026,10,16,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED
            z.writestr(info,b)
    with zipfile.ZipFile(path) as z:assert z.testzip() is None
    manifest.append({'file':path.name,'bytes':path.stat().st_size,'sha256':sha(path.read_bytes())})
files={'Common/'+n:b for n,b in base.items()}
files.update({'fomod/ModuleConfig.xml':(docs/'ModuleConfig.xml').read_bytes(),
              'fomod/info.xml':(docs/'info.xml').read_bytes(),'fomod/screenshot.png':(docs/'LanguagePrompt.png').read_bytes()})
for label,lang in [('Chinese','zh-CN'),('English','en')]:
    files['Options/'+label+'/TailorDDBridge.json']=(json.dumps({'language':lang,'activateDDOnExit':False},indent=2)+'\n').encode()
writezip(out/'Tailor-Devious-Device-Compatibility-Patch-0.4-FOMOD.zip',files)
with zipfile.ZipFile(out/'Tailor-Devious-Device-Compatibility-Patch-0.4-FOMOD.zip') as z:
    config=ET.fromstring(z.read('fomod/ModuleConfig.xml'));ET.fromstring(z.read('fomod/info.xml'))
    group=config.find('./installSteps/installStep/optionalFileGroups/group')
    assert group.attrib['type']=='SelectExactlyOne'
    choices=group.findall('./plugins/plugin')
    assert [p.attrib['name'] for p in choices]==['中文','English']
    # Resolve the actual FOMOD file mappings for each choice. A language branch
    # must install precisely the common payload plus its one language JSON.
    results=[]
    for choice in choices:
        result={}
        entries=list(config.find('requiredInstallFiles'))+list(choice.find('files'))
        for entry in entries:
            src=entry.attrib['source'].replace('\\','/');dest=entry.attrib['destination'].replace('\\','/')
            assert not src.startswith('/') and '..' not in Path(src).parts and '..' not in Path(dest).parts
            if entry.tag=='folder':
                prefix=src.rstrip('/')+'/'
                matches=[n for n in z.namelist() if n.startswith(prefix)]
                assert matches
                for n in matches:result[(dest.rstrip('/')+'/' if dest else '')+n[len(prefix):]]=z.read(n)
            elif entry.tag=='file':
                assert dest not in result
                result[dest]=z.read(src)
            else:raise AssertionError('Unsupported install entry')
        assert result['SKSE/Plugins/TailorDDBridge.dll']==blob
        language='zh-CN' if choice.attrib['name']=='中文' else 'en'
        assert json.loads(result['SKSE/Plugins/TailorDDBridge.json'])=={'language':language,'activateDDOnExit':False}
        assert set(result)==set(base)|{'SKSE/Plugins/TailorDDBridge.json'}
        assert choice.find('image').attrib['path'] in z.namelist()
        results.append(result)
    differences=[n for n in results[0] if results[0][n]!=results[1][n]]
    assert differences==['SKSE/Plugins/TailorDDBridge.json']
    assert [n for n in z.namelist() if n.lower().endswith('.dll')]==['Common/SKSE/Plugins/TailorDDBridge.dll']
    assert not any(Path(n).suffix.lower() in ['.esp','.esm','.esl','.pex','.nif','.dds','.ttf','.otf','.pdb','.log','.ess','.skse'] for n in z.namelist())
    assert not any('tailor.dll' in n.lower() or 'generated' in n.lower() for n in z.namelist())
    # Verify the installed payload tools can be invoked after mapping.
    temp=root/'build/upload-tool-check'
    temp.mkdir(parents=True,exist_ok=True)
    for n in ['Tools/build_cosmetics.py','Tools/pex_metadata.py']:
        p=temp/n;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(results[0][n])
    subprocess.run([sys.executable,str(temp/'Tools/build_cosmetics.py'),'--help'],stdout=subprocess.DEVNULL,check=True)

source={}
for folder in ['src','lib/minhook-1.3.4']:
    for p in (root/folder).rglob('*'):
        if p.is_file() and not any(part.startswith('.') for part in p.relative_to(root).parts) and p.suffix.lower() not in ['.dll','.exe','.pdb','.lib','.obj','.pyc']:
            source['Tailor-DD-Bridge/'+p.relative_to(root).as_posix()]=p.read_bytes()
for n in ['xmake.lua']:source['Tailor-DD-Bridge/'+n]=(root/n).read_bytes()
for n in ['device_policy_test.cpp','prologue_test.cpp']:source['Tailor-DD-Bridge/tests/'+n]=(root/'tests'/n).read_bytes()
source['Tailor-DD-Bridge/LICENSE']=base['LICENSE'];source['Tailor-DD-Bridge/THIRD_PARTY_NOTICES.txt']=notices
for n,b in licenses.items():source['Tailor-DD-Bridge/'+n]=b
source['Tailor-DD-Bridge/package/Common/LICENSE']=base['LICENSE']
for n,b in licenses.items():source['Tailor-DD-Bridge/package/Common/'+n]=b
for n in ['README-English.md','README-中文.md']:source['Tailor-DD-Bridge/'+n]=base[n]
for n in ['ModuleConfig.xml','info.xml','LanguagePrompt.png','Make-LanguagePrompt.ps1','README-English.md','README-中文.md']:
    source['Tailor-DD-Bridge/release-docs/'+n]=(docs/n).read_bytes()
source['Tailor-DD-Bridge/tests/build_upload_packages.py']=Path(__file__).read_bytes()
for n in ['build_cosmetics.py','pex_metadata.py']:source['Tailor-DD-SafeFitting/tools/'+n]=base['Tools/'+n]
source['Tailor-DD-SafeFitting/tests/test_portable_generator.py']=(workspace/'Tailor-DD-SafeFitting/tests/test_portable_generator.py').read_bytes()
lib=workspace/'Tailor-DD-Compat/lib/commonlibsse-ng'
for p in lib.rglob('*'):
    if not p.is_file() or any(part.startswith('.') for part in p.relative_to(lib).parts):continue
    if p.suffix.lower() in ['.exe','.dll','.pdb','.lib','.obj','.pyc','.zip']:continue
    source['Tailor-DD-Compat/lib/commonlibsse-ng/'+p.relative_to(lib).as_posix()]=p.read_bytes()
source['BUILD.txt']=b'''Corresponding source for Tailor Devious Device Compatibility Patch 0.4 (button-centering build).
Developer archive: do not install into MO2. No original Tailor implementation, translated implementation, fonts or game assets are included.
Use Visual Studio 2022 C++ x64 tools and xmake 3.1.1; allow xmake to obtain the pinned dependencies.
Keep the three sibling directories when extracting.
From Tailor-DD-Bridge:
  xmake f -m release -y
  xmake -j 8 -y
CommonLibSSE-NG snapshot: 8b032fa992750d654d6d38a33731714d8b86be1f (included).
MinHook 1.3.4 source is included. ImGui 1.92.6 and JSON 3.12.0 are pinned in xmake.lua; other library dependency versions are pinned in CommonLib's xmake.lua.
Original Tailor binaries are external prerequisites, not included. The prologue test requires the two separately obtained exact supported DLLs.
The generator test is synthetic and requires no game or third-party mod assets:
  python Tailor-DD-SafeFitting/tests/test_portable_generator.py
The experiment that isolated NPC assignments by save was reverted and is not in this source.
FOMOD maintenance inputs are in Tailor-DD-Bridge/release-docs/.
Edit ModuleConfig.xml for the language choices. The language configurations and shared-file whitelist are in tests/build_upload_packages.py.
The source prompt image and its PowerShell generator are included; they are original installer artwork, not Tailor assets.
Repackage with: python tests/build_upload_packages.py --dll path/to/TailorDDBridge.dll
The expected binary hash near the top of that script must be updated deliberately for a new build.
'''
assert not any('NpcSaveScope' in n or 'withdrawn' in n.lower() or Path(n).name.lower()=='tailor.dll' or Path(n).suffix.lower() in ['.esp','.pex','.nif','.dds','.ttf','.otf'] for n in source)
assert not any(b'C:\\Users\\Admin' in b or b'D:\\Modding\\MO2' in b for n,b in source.items() if n.startswith(('Tailor-DD-Bridge/','Tailor-DD-SafeFitting/')))
writezip(out/'Tailor-Devious-Device-Compatibility-Patch-0.4-Source.zip',source)

for lang in ['English','中文']:
    # The bilingual standalone descriptions are ready to copy into a mod page.
    (out/f'Upload-Description-{lang}.md').write_bytes((docs/f'README-{lang}.md').read_bytes())
(out/'SHA256SUMS.txt').write_text(''.join(f"{r['sha256']}  {r['file']}\n" for r in manifest),encoding='utf8')
report={'displayName':'Tailor Devious Device Compatibility Patch','version':'0.4 experimental, button-centered build','dllSha256':expected,'runtimeDifferences':differences,'packages':manifest,
        'matchesCurrentInstalledEnglishAndChinese':bool(args.check_mo2),'personalGeneratedDataIncluded':False,'TailorRuntimeOrThirdPartyTranslationIncluded':False,
        'fomodXmlParsed':True,'bothLanguageInstallMappingsVerified':True,'fixedBilingualPromptImageInspected':True,'MO2InstallerGuiVerified':False,
        'crossSaveIsolationIncluded':False,'extractedGeneratorHelpPassed':True,'gameVisualCenteringVerified':False,'completeRuntimeMatrixVerified':False}
(out/'PACKAGE-CHECK.json').write_text(json.dumps(report,indent=2),encoding='utf8')
(out/'上传说明.txt').write_text('''上传文件：
1. Tailor-Devious-Device-Compatibility-Patch-0.4-FOMOD.zip：唯一主文件，安装时选择“中文”或“English”。左侧固定显示双语语言问题。
2. Tailor-Devious-Device-Compatibility-Patch-0.4-Source.zip：对应源码，作为开发者/可选文件一并提供，不作为游戏模组安装。

Upload-Description-English.md 和 Upload-Description-中文.md 是可复制到发布页的完整说明。
SHA256SUMS.txt 和 PACKAGE-CHECK.json 是文件校验清单；无需安装进 MO2。
请将版本标为 0.4 Experimental，说明支持的是指定的 Tailor 3.0.1 DLL，不承诺其他构建。
不要上传个人 Generated Data、MO2 配置、存档、日志、Tailor 本体或他人汉化。
当前仅有有限的本机游戏测试，完整运行环境矩阵及最新按钮居中目视验证尚未完成，说明中已经注明。
不含撤回的 0.5 存档隔离逻辑；NPC 分配继续按 Tailor 原版跨档共享。
''',encoding='utf8')
print(json.dumps(report,indent=2))
