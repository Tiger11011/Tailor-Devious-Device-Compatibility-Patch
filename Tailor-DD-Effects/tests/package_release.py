"""Package the tested release from explicit runtime inputs and source allowlists."""
from pathlib import Path, PurePosixPath
import argparse,hashlib,json,subprocess,sys,zipfile,xml.etree.ElementTree as ET

project=Path(__file__).resolve().parents[1]; repo=project.parent
version='0.6.4'; package_revision='0.6.4-permissions.1'
prefix=f'Tailor-Devious-Device-Compatibility-Patch-{package_revision}'
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--dll',type=Path,required=True)
parser.add_argument('--pex',type=Path,required=True)
parser.add_argument('--dependency-source-dir',type=Path,required=True,help='Directory containing the eight verified upstream source archives')
parser.add_argument('--output',type=Path,default=project/'releases'/prefix)
args=parser.parse_args();out=args.output.resolve();out.mkdir(parents=True,exist_ok=True)
def sha(data):return hashlib.sha256(data).hexdigest()
dll,pex=args.dll.read_bytes(),args.pex.read_bytes()
assert sha(dll)=='561c55693a066cac0678a39fe44524449aecf83fe19476454cfb42ea7e32e78a'
assert sha(pex)=='82162769dba8821dd75429a39ebd438a8dc5f3a80ef1370a2c83849158e0a311'
assert b'Tailor DD bridge 0.6.4' in dll and b'TailorDDStartupTrace' not in dll
docs=project/'release-docs';old=repo/'Tailor-DD-Bridge/package/Common'
notices=(repo/'THIRD_PARTY_NOTICES.txt').read_bytes()
base={'SKSE/Plugins/TailorDDBridge.dll':dll,'Scripts/TailorDDFreeWear.pex':pex,
      'LICENSE':(repo/'LICENSE').read_bytes(),'COPYRIGHT.md':(repo/'COPYRIGHT.md').read_bytes(),'THIRD_PARTY_NOTICES.txt':notices}
base['SOURCE-ACCESS.md']=(repo/'SOURCE-ACCESS.md').read_bytes()
for name in ['CREDITS.md','PREREQUISITE-NOTICES.md','RESOURCE-AUDIT.json','AUTHOR-PERMISSIONS.md']:
 base[name]=(repo/name).read_bytes()
for p in (old/'licenses').iterdir():
 if p.is_file():base['licenses/'+p.name]=p.read_bytes()
for p in (project/'licenses').iterdir():
 if p.is_file():
  name='licenses/'+p.name
  assert name not in base
  base[name]=p.read_bytes()
license_sources=json.loads(base['licenses/LICENSE-SOURCES.json'])
assert license_sources['revision']=='a6088e22b06f6e97ab617d2f79420d6a99228f52'
for row in license_sources['files']:
 assert sha(base['licenses/'+row['file']])==row['sha256']
assert b'Copyright (c) 2026 ColdSun' in base['licenses/TailorAPI-MIT.txt']
resource_notices=json.loads(base['licenses/RESOURCE-NOTICE-SOURCES.json'])
for row in resource_notices['files']:
 assert sha(base['licenses/'+row['file']])==row['sha256']
resource_audit=json.loads(base['RESOURCE-AUDIT.json'])
assert len(resource_audit['components'])==29 and resource_audit['bugFixPurposeConfirmedByMaintainer'] is True
assert resource_audit['tailorAuthorPermission']['granted'] is True
assert resource_audit['tailorAuthorPermission']['evidenceType']=='Maintainer confirmation'
for name in ['README-English.md','README-中文.md']:base[name]=(docs/name).read_bytes()
for name in ['build_cosmetics.py','pex_metadata.py']:base['Tools/'+name]=(repo/'Tailor-DD-SafeFitting/tools'/name).read_bytes()
files={'Common/'+n:b for n,b in base.items()}
for name in ['ModuleConfig.xml','info.xml']:files['fomod/'+name]=(docs/name).read_bytes()
files['fomod/screenshot.png']=(docs/'LanguagePrompt.png').read_bytes()
for name,code in [('Chinese','zh-CN'),('English','en')]:
 files[f'Options/{name}/TailorDDBridge.json']=(json.dumps({'language':code,'activateDDOnExit':False},indent=2)+'\n').encode()

def writezip(path,entries):
 with zipfile.ZipFile(path,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  for name,data in sorted(entries.items()):
   assert not PurePosixPath(name).is_absolute() and '..' not in PurePosixPath(name).parts
   info=zipfile.ZipInfo(name,date_time=(2026,10,18,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED
   z.writestr(info,data)
 with zipfile.ZipFile(path) as z:assert z.testzip() is None

installer=out/(prefix+'-FOMOD.zip');writezip(installer,files)
config=ET.fromstring(files['fomod/ModuleConfig.xml']);ET.fromstring(files['fomod/info.xml'])
group=config.find('./installSteps/installStep/optionalFileGroups/group')
assert group.attrib['type']=='SelectExactlyOne'
choices=group.findall('./plugins/plugin');assert [p.attrib['name'] for p in choices]==['中文','English']
results=[]
for choice in choices:
 mapped={}
 for entry in list(config.find('requiredInstallFiles'))+list(choice.find('files')):
  source=entry.attrib['source'].replace('\\','/');dest=entry.attrib['destination'].replace('\\','/')
  assert '..' not in PurePosixPath(source).parts and '..' not in PurePosixPath(dest).parts
  if entry.tag=='folder':
   path=source.rstrip('/')+'/'
   matches=[name for name in files if name.startswith(path)];assert matches
   for name in matches:mapped[(dest.rstrip('/')+'/' if dest else '')+name[len(path):]]=files[name]
  else:
   assert entry.tag=='file' and dest not in mapped
   mapped[dest]=files[source]
 assert set(mapped)==set(base)|{'SKSE/Plugins/TailorDDBridge.json'}
 code='zh-CN' if choice.attrib['name']=='中文' else 'en'
 assert json.loads(mapped['SKSE/Plugins/TailorDDBridge.json'])=={'language':code,'activateDDOnExit':False}
 assert mapped['SKSE/Plugins/TailorDDBridge.dll']==dll and mapped['Scripts/TailorDDFreeWear.pex']==pex
 assert choice.find('image').attrib['path'] in files
 results.append(mapped)
differences=[n for n in results[0] if results[0][n]!=results[1][n]]
assert differences==['SKSE/Plugins/TailorDDBridge.json']
assert [n for n in files if n.lower().endswith('.dll')]==['Common/SKSE/Plugins/TailorDDBridge.dll']
assert [n for n in files if n.lower().endswith('.pex')]==['Common/Scripts/TailorDDFreeWear.pex']
assert not any(Path(n).suffix.lower() in ['.esp','.esm','.esl','.nif','.dds','.ttf','.otf','.pdb','.log','.ess','.skse','.exe'] for n in files)
assert not any('startuptrace' in n.lower() or Path(n).name.lower()=='tailor.dll' or 'generated' in n.lower() for n in files)
# Test the tools after the same file mapping that the installer uses.
check=project/'build/package-check'
for name in ['Tools/build_cosmetics.py','Tools/pex_metadata.py']:
 p=check/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(results[0][name])
subprocess.run([sys.executable,str(check/'Tools/build_cosmetics.py'),'--help'],stdout=subprocess.DEVNULL,check=True)

# Include repository sources and exact dependency checkouts, not private workspace
# backups, forensic projects, downloaded game/mod fixtures or build outputs.
blocked={'.dll','.exe','.pex','.esp','.esm','.esl','.nif','.dds','.ttf','.otf','.pdb','.obj','.lib','.a','.so','.dylib','.zip','.7z','.log','.ess','.skse','.pyc','.pch','.ipch'}
source={}
def addtree(folder,prefix,untracked=False,roots=None):
 command=['git','-C',str(folder),'ls-files','-z','--cached']
 if untracked:command+=['--others','--exclude-standard']
 listing=subprocess.check_output(command).decode('utf8').split('\0')
 for name in listing:
  if not name:continue
  if roots is not None and len(Path(name).parts)>1 and Path(name).parts[0] not in roots:continue
  p=folder/name
  if not p.is_file() or p.suffix.lower() in blocked:continue
  if any(part in ['.git','build','deployment','evidence','releases','diagnostics','__pycache__'] or part.startswith('.xmake') for part in Path(name).parts):continue
  source[(prefix+'/' if prefix else '')+Path(name).as_posix()]=p.read_bytes()
addtree(repo,'',True)
common=repo/'Tailor-DD-Compat/lib/commonlibsse-ng'
assert subprocess.check_output(['git','-C',str(common),'rev-parse','HEAD'],text=True).strip()=='8b032fa992750d654d6d38a33731714d8b86be1f'
vr=common/'extern/openvr'
assert subprocess.check_output(['git','-C',str(vr),'rev-parse','HEAD'],text=True).strip()=='60eb187801956ad277f1cae6680e3a410ee0873b'
addtree(common,'Tailor-DD-Compat/lib/commonlibsse-ng',roots={'include','src','cmake','scripts','res','licenses'})
addtree(vr,'Tailor-DD-Compat/lib/commonlibsse-ng/extern/openvr',roots={'headers','src','cmake'})
vendored={n:sha(data) for n,data in source.items() if n.startswith('Tailor-DD-Compat/lib/commonlibsse-ng/')}
assert not any('/samples/' in n or '/tests/' in n or Path(n).suffix.lower() in {'.bin','.dbg','.unity','.eps'} for n in vendored)
source['dependency-sources/VENDORED-SOURCE-SHA256.json']=(json.dumps(vendored,indent=2)+'\n').encode()
dependency_manifest=json.loads((repo/'dependency-sources/DEPENDENCIES.json').read_text(encoding='utf8'))
for row in dependency_manifest['dependencies']:
 archive=args.dependency_source_dir/Path(row['sourceArchive']).name
 data=archive.read_bytes()
 assert sha(data)==row['sha256'],row['name']
 source['dependency-sources/'+row['sourceArchive']]=data
assert len(dependency_manifest['dependencies'])==8
assert 'Tailor-DD-Effects/Papyrus/TailorDDFreeWear.psc' in source
assert 'Tailor-DD-Compat/lib/commonlibsse-ng/extern/openvr/headers/openvr.h' in source
assert not any(b'C:\\Users\\Admin' in data or b'D:\\Modding\\MO2' in data for name,data in source.items() if name.startswith(('Tailor-DD-Effects/','Tailor-DD-SafeFitting/')))
assert not any('StartupTrace' in name or 'NpcSaveScope' in name or Path(name).name.lower()=='tailor.dll' for name in source)
sourcezip=out/(prefix+'-Source.zip');writezip(sourcezip,source)
for name in ['Release-Notes.md','README-English.md','README-中文.md','Nexus-Description-English.txt','Nexus-Description-中文.txt']:
 (out/name).write_bytes((docs/name).read_bytes())
for language in ['English','中文']:
 (out/f'Upload-Description-{language}.md').write_bytes((docs/f'README-{language}.md').read_bytes())
packages=[{'file':p.name,'bytes':p.stat().st_size,'sha256':sha(p.read_bytes())} for p in [installer,sourcezip]]
(out/'SHA256SUMS.txt').write_text(''.join(f"{r['sha256']}  {r['file']}\n" for r in packages),encoding='utf8')
report={'version':version,'packageRevision':package_revision,'sourceAndNoticeOnlyRevision':True,'dependencySourceArchivesVerified':8,'upstreamLicenseRevision':license_sources['revision'],'upstreamLicenseHashesVerified':True,'dllSha256':sha(dll),'helperSha256':sha(pex),'packages':packages,'installerFileCount':len(files),'sourceFileCount':len(source),'bothLanguageInstallMappingsVerified':True,'languageDifferences':differences,'defaultsOff':True,'originalTailorOrTranslationIncluded':False,'personalGeneratedDataIncluded':False,'startupDiagnosticsIncluded':False,'userConfirmedRuntimeScenario':'Local NPC DD switching and Mu fully-covered-to-visible-head appearance recovery','fullCompatibilityMatrixVerified':False,'MO2InstallerGuiRetested':False}
(out/'PACKAGE-CHECK.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf8')
(out/'上传说明.txt').write_text('主文件：'+installer.name+'\n对应开发源码：'+sourcezip.name+'\n安装包修订：'+package_revision+'；游戏内功能版本仍为 0.6.4。\n仅源码/说明/许可修订，DLL 和 PEX 保持原样。不要覆盖或删除旧 GitHub 标签。\nSource.zip 内已附 8 项实际版本依赖源码、配方、配置差异和校验清单，不要安装到 MO2。\nNexus 主文件旁提供本修订的对应 Source.zip（Optional Files），或清楚链接完全对应的 GitHub 发布页。\n英文/中文页面说明：Nexus-Description-English.txt / Nexus-Description-中文.txt（BBCode）；另有 Markdown 全文。\n前置、受支持版本及安装顺序见双语说明。FOMOD 默认关闭，二选一语言，不包含 Tailor 或他人汉化。\n不上传私人 Generated Data、存档、日志、备份和启动诊断工具。本目录尚未自动上传至 Nexus。\n',encoding='utf8')
print(json.dumps(report,ensure_ascii=False,indent=2))
