"""Build new, script-free ARMO records from the enabled MO2 profile; never edits inputs."""
from __future__ import annotations
import argparse, collections, hashlib, json, pathlib, struct, zlib, zipfile, runpy
ScriptIndex = runpy.run_path(str(pathlib.Path(__file__).with_name('pex_metadata.py')))['ScriptIndex']

U32=lambda b:struct.unpack('<I',b)[0]
def records(data,start=0,end=None):
    end=len(data) if end is None else end
    p=start
    while p+24<=end:
        sig=data[p:p+4];size=U32(data[p+4:p+8])
        if sig==b'GRUP':
            yield from records(data,p+24,p+size);p+=size;continue
        flags=U32(data[p+8:p+12]);fid=U32(data[p+12:p+16]);body=data[p+24:p+24+size]
        if flags&0x40000: body=zlib.decompress(body[4:])
        yield sig,fid,flags,body
        p+=24+size
    if p!=end:raise ValueError(f'Invalid record boundary {p}/{end}')
def subs(data):
    p=0;large=None
    while p+6<=len(data):
        sig=data[p:p+4];n=struct.unpack_from('<H',data,p+4)[0];p+=6
        if sig==b'XXXX':large=U32(data[p:p+4]);p+=n;continue
        if large is not None:n=large;large=None
        if p+n>len(data):raise ValueError('Invalid subrecord size')
        yield sig,data[p:p+n];p+=n
    if p!=len(data):raise ValueError('Invalid subrecord boundary')
def sub(sig,value):
    if len(value)>65535:return b'XXXX\x04\x00'+struct.pack('<I',len(value))+sig+b'\0\0'+value
    return sig+struct.pack('<H',len(value))+value
def record(sig,fid,body,flags=0):
    return struct.pack('<4sIIIIHH',sig,len(body),flags,fid,0,44,0)+body
def text(v):return v.rstrip(b'\0').decode('utf8',errors='replace')
def vmad(data):
    p=6;result=[]
    def take(fmt):
        nonlocal p
        x=struct.unpack_from(fmt,data,p);p+=struct.calcsize(fmt);return x[0] if len(x)==1 else x
    def string():
        nonlocal p
        n=take('<H');s=data[p:p+n].decode('utf8');p+=n;return s
    def val(t):
        if t==0:return None
        if t==1:return take('<HHI')[2]
        if t==2:return string()
        if t==3:return take('<i')
        if t==4:return take('<f')
        if t==5:return take('<B')
        if 11<=t<=15:return [val(t-10) for _ in range(take('<I'))]
        raise ValueError(f'Unknown VMAD type {t}')
    for _ in range(struct.unpack_from('<H',data,4)[0]):
        name=string();take('<B');n=take('<H');props={}
        for _ in range(n):
            key=string().lower();typ=take('<B');take('<B');props[key]=val(typ)
        result.append((name,props))
    return result
def identity(fid,names):
    if not fid:return None
    idx=fid>>24
    if idx>=len(names):raise ValueError(f'Invalid master index {fid:08X}')
    return names[idx].lower(),fid&0xffffff
REFS={b'RNAM',b'ETYP',b'BIDS',b'BAMT',b'YNAM',b'ZNAM',b'MODL'}
PLAIN={b'OBND',b'MOD2',b'MO2T',b'MOD4',b'MO4T',b'ICON',b'MICO',b'ICO2',b'MIC2',b'BOD2',b'BODT',b'DATA',b'DNAM'}
DROP={b'VMAD',b'EITM',b'EAMT',b'KSIZ',b'KWDA',b'TNAM',b'DESC',b'DEST',b'DSTD',b'DSTF',b'DMDL',b'DMDT',b'YNAM',b'ZNAM'}
ALT={b'MO2S',b'MO4S',b'MODS'}
def alt_offsets(b):
    p=4;offsets=[]
    for _ in range(U32(b[:4])):
        n=U32(b[p:p+4]);p+=4+n;offsets.append(p);p+=8
    if p!=len(b):raise ValueError('Unsupported alternate texture encoding')
    return offsets

def mo2_settings(root):
    settings = {}
    for line in (root/'ModOrganizer.ini').read_text(encoding='utf-8-sig').splitlines():
        if '=' not in line or line.startswith(('#',';')): continue
        key,value = line.split('=',1)
        if key in ('selected_profile','gamePath','mod_directory','profiles_directory','overwrite_directory','base_directory'):
            if value.startswith('@ByteArray(') and value.endswith(')'): value=value[11:-1]
            settings[key]=value.replace('\\\\','\\').strip('"')
    return settings

def build(root,out,profile=None,game_data=None,mods_dir=None,profiles_dir=None,overwrite_dir=None):
    root=root.resolve()
    settings=mo2_settings(root)
    base=settings.get('base_directory') or str(root)
    def folder(value,default):
        value=value or default
        value=value.replace('%BASE_DIR%',base).replace('%MO2_DIR%',str(root))
        if '%' in value: raise ValueError('Unresolved MO2 path variable; provide an explicit directory: '+value)
        result=pathlib.Path(value)
        return result if result.is_absolute() else root/result
    profile=profile or settings.get('selected_profile')
    if not profile or pathlib.Path(profile).name!=profile or profile in ('.','..'):
        raise ValueError('Provide a profile name with --profile')
    mod_root=folder(str(mods_dir) if mods_dir else settings.get('mod_directory'),'mods')
    profile_root=folder(str(profiles_dir) if profiles_dir else settings.get('profiles_directory'),'profiles')
    overwrite=folder(str(overwrite_dir) if overwrite_dir else settings.get('overwrite_directory'),'overwrite')
    if not game_data:
        if not settings.get('gamePath'): raise ValueError('Game path is missing; provide --game-data')
        game_data=folder(settings['gamePath'],'')/'Data'
    if not game_data.is_dir(): raise FileNotFoundError('Game Data directory not found: '+str(game_data))
    out.mkdir(parents=True,exist_ok=True)
    ml=profile_root/profile/'modlist.txt';pl=profile_root/profile/'plugins.txt'
    mods=[game_data]+[mod_root/s[1:] for s in reversed(ml.read_text(encoding='utf-8-sig').splitlines()) if s.startswith('+')]+[overwrite]
    providers={}
    for folder in mods:
        for f in folder.glob('*'):
            if f.suffix.lower() in ('.esp','.esm','.esl') and f.is_file():providers[f.name.lower()]=f
    active=[s[1:] for s in pl.read_text(encoding='utf-8-sig').splitlines() if s.startswith('*')]
    implicit=['Skyrim.esm','Update.esm','Dawnguard.esm','HearthFires.esm','Dragonborn.esm']
    active=[n for n in implicit if n.lower() not in {a.lower() for a in active} and n.lower() in providers]+active
    armors={};inputs=[];display_names={n.lower():n for n in active}
    for name in active:
        if name.lower().startswith('tailordd'):continue
        f=providers.get(name.lower())
        if not f:raise FileNotFoundError(f'Enabled plugin missing: {name}')
        blob=f.read_bytes();head=next(records(blob));header=list(subs(head[3]));names=[text(v) for k,v in header if k==b'MAST']+[name]
        display_names.update({n.lower():n for n in names})
        has_relevant=False
        for sig,fid,flags,body in records(blob):
            if sig!=b'ARMO':continue
            key=identity(fid,names)
            armors[key]={'key':key,'flags':flags,'subs':list(subs(body)),'names':names,'path':str(f),'localized':bool(head[2]&0x80)}
            has_relevant=True
        if has_relevant:inputs.append({'path':str(f),'sha256':hashlib.sha256(blob).hexdigest()})
    script_index=ScriptIndex(mods)
    candidates=[];skipped=[];classification=[]
    for key,row in sorted(armors.items()):
        if row['flags']&0x20:continue
        properties=[]
        for k,v in row['subs']:
            if k==b'VMAD':
                try:properties.extend(vmad(v))
                except Exception as e:raise ValueError(f'VMAD decode failed in {key}, {row["path"]}: {e}') from e
        for script,props in properties:
            render_id=props.get('devicerendered');inv_id=props.get('deviceinventory')
            if not isinstance(render_id,int) or not isinstance(inv_id,int):continue
            if identity(inv_id,row['names'])!=key:continue
            render_key=identity(render_id,row['names']);render=armors.get(render_key)
            reason=None
            proven,chain,script_error=script_index.is_dd_device(script)
            classification.append({'source':key,'script':script,'ancestry':chain,'provenDD':proven,'error':script_error})
            if not proven:reason=script_error or 'script does not inherit winning zadEquipScript; custom behavior requires review'
            elif not render or render['flags']&0x20:reason='missing/deleted rendered record'
            elif any(k==b'VMAD' and vmad(v) for k,v in render['subs']):reason='rendered record itself has scripts'
            elif any(k==b'TNAM' and U32(v) for k,v in render['subs']):reason='templated rendered record needs manual review'
            elif not any(k==b'MODL' and U32(v) for k,v in render['subs']):reason='no static armor addon; appearance requires manual review'
            elif row['localized']:reason='localized inventory FULL requires string-table resolution'
            full=next((v for k,v in row['subs'] if k==b'FULL'),b'')
            if not full:reason='unnamed inventory device'
            if reason:skipped.append({'source':key,'render':render_key,'reason':reason});break
            candidates.append((row,render,full));break
    if not candidates:raise ValueError('No supported DD inventory/render pairs')
    dependencies=set()
    for inv,render,full in candidates:
        dependencies.add(render['key'][0])
        for k,v in render['subs']:
            if k in DROP or k in (b'EDID',b'FULL') or k in PLAIN:continue
            if k in REFS:
                if len(v)!=4:raise ValueError(f'Unexpected reference field {k}')
                ref=identity(U32(v),render['names'])
                if ref:dependencies.add(ref[0])
            elif k in ALT:
                for p in alt_offsets(v):
                    ref=identity(U32(v[p:p+4]),render['names'])
                    if ref:dependencies.add(ref[0])
            else:raise ValueError(f'Unreviewed rendered ARMO field {k!r} in {render["key"]}')
    masters=[n for n in active if n.lower() in dependencies]
    if len(masters)!=len(dependencies):raise ValueError(f'Unresolved cosmetic masters: {dependencies-{n.lower() for n in masters}}')
    if len(masters)>=254:raise ValueError('Too many masters')
    index={n.lower():i for i,n in enumerate(masters)}
    def remap(fid,names):
        ref=identity(fid,names)
        return 0 if ref is None else index[ref[0]]<<24|ref[1]
    # Rebuild into the SAME output directory to preserve saved cosmetic FormIDs.
    # Removed records are retired permanently; a new device never inherits an old ID.
    old_manifest_path=out/'SKSE/Plugins/TailorDDSafeFitting.json'
    old_ids={};retired=set()
    if old_manifest_path.is_file():
        previous=json.loads(old_manifest_path.read_text(encoding='utf8'))
        if previous.get('schema')!=2:raise ValueError('Output contains an older manifest; use a new output directory')
        old_ids={(r['sourcePlugin'].lower(),r['sourceId']):r['targetId'] for r in previous['mappings']}
        retired=set(previous.get('retiredIds',[]))
        values=list(old_ids.values())+list(retired)
        if len(values)!=len(set(values)) or any(not isinstance(v,int) or not 0x800<=v<0xffffff for v in values):
            raise ValueError('Invalid/duplicate previous cosmetic IDs')
    current_keys={inv['key'] for inv,render,full in candidates}
    retired.update(value for key,value in old_ids.items() if key not in current_keys)
    next_id=max([0x7ff]+list(old_ids.values())+list(retired))+1
    bodies=[];mapping=[];fid_base=0x800
    for i,(inv,render,full) in enumerate(candidates):
        local=old_ids.get(inv['key'])
        if local is None:local=next_id;next_id+=1
        if local>=0xffffff:raise ValueError('Cosmetic FormID space exhausted')
        fid=len(masters)<<24|local
        new=[(b'EDID',f'TDD_Fitting_{local:06X}'.encode()+b'\0'),(b'FULL',full)]
        for k,v in render['subs']:
            if k in DROP or k in (b'EDID',b'FULL'):continue
            if k in REFS:v=struct.pack('<I',remap(U32(v),render['names']))
            elif k in ALT:
                mutable=bytearray(v)
                for p in alt_offsets(v):struct.pack_into('<I',mutable,p,remap(U32(v[p:p+4]),render['names']))
                v=bytes(mutable)
            new.append((k,v))
        # No DD keyword, script, enchantment or template survives into a fitting copy.
        assert not any(k in {b'VMAD',b'EITM',b'KWDA',b'TNAM'} for k,v in new)
        bodies.append(record(b'ARMO',fid,b''.join(sub(k,v) for k,v in new),flags=render['flags']&0x40))
        mapping.append({'sourcePlugin':display_names[inv['key'][0]],'sourceId':inv['key'][1],'renderPlugin':display_names[render['key'][0]],'renderId':render['key'][1],'targetId':local,'name':text(full)})
    for local in sorted(retired):
        bodies.append(record(b'ARMO',len(masters)<<24|local,sub(b'EDID',f'TDD_Retired_{local:06X}'.encode()+b'\0'),flags=0x20))
    header=[(b'HEDR',struct.pack('<fII',1.7,len(bodies)+1,next_id)),(b'CNAM',b'Tailor DD Safe Fitting\0'),(b'SNAM',b'Cosmetic DD fitting copies; original devices are unchanged.\0')]
    for name in masters:header.extend([(b'MAST',name.encode()+b'\0'),(b'DATA',bytes(8))])
    tes4=record(b'TES4',0,b''.join(sub(k,v) for k,v in header))
    body=b''.join(bodies);group=struct.pack('<4sI4sIII',b'GRUP',len(body)+24,b'ARMO',0,0,0)+body
    plugin=out/'TailorDDCosmetics.esp';plugin.write_bytes(tes4+group)
    mp=out/'SKSE/Plugins/TailorDDSafeFitting.json';mp.parent.mkdir(parents=True,exist_ok=True)
    mp.write_text(json.dumps({'schema':2,'classification':'winning-PEX-DD-inheritance-and-inventory-render-pair',
        'targetPlugin':plugin.name,'targetSha256':hashlib.sha256(plugin.read_bytes()).hexdigest(),'retiredIds':sorted(retired),
        'pluginProofs':[{'file':pathlib.Path(r['path']).name,'sha256':r['sha256']} for r in inputs],
        'scriptProofs':[{'file':r['name']+'.pex','sha256':r['sha256']} for r in script_index.cache.values()],
        'mappings':mapping,'excluded':[{'sourcePlugin':display_names[r['source'][0]],'sourceId':r['source'][1],'renderPlugin':display_names[r['render'][0]],'renderId':r['render'][1],'reason':r['reason']} for r in skipped]},indent=2,ensure_ascii=False),encoding='utf8')
    audit={'count':len(mapping),'profile':profile,'masters':masters,'sourceCounts':dict(collections.Counter(r['sourcePlugin'] for r in mapping)),'skipped':skipped,'classification':classification,'inputs':inputs,'scriptInputs':list(script_index.cache.values()),'pluginSha256':hashlib.sha256(plugin.read_bytes()).hexdigest()}
    (out/'TailorDD-generation-report.json').write_text(json.dumps(audit,indent=2,ensure_ascii=False),encoding='utf8')
    # Reparse the emitted plugin and validate stable IDs and forbidden fields.
    emitted=list(records(plugin.read_bytes()));ars=[r for r in emitted if r[0]==b'ARMO']
    assert len(ars)==len(mapping)+len(retired)
    assert len({r[1] for r in ars})==len(ars)
    for _,fid,flags,data in ars:
        assert fid>>24==len(masters)
        assert not any(k in {b'VMAD',b'EITM',b'KWDA',b'TNAM'} for k,v in subs(data))
    print(json.dumps({k:v for k,v in audit.items() if k not in ('inputs','classification','scriptInputs')},ensure_ascii=False,indent=2))
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--mo2',type=pathlib.Path,required=True,help='MO2 instance directory containing ModOrganizer.ini')
    p.add_argument('--profile',help='Profile name; defaults to the selected MO2 profile')
    p.add_argument('--game-data',type=pathlib.Path,help='Game Data directory; defaults to MO2 gamePath/Data')
    p.add_argument('--mods-dir',type=pathlib.Path)
    p.add_argument('--profiles-dir',type=pathlib.Path)
    p.add_argument('--overwrite-dir',type=pathlib.Path)
    p.add_argument('--output',type=pathlib.Path,required=True,help='New mod directory to receive the ESP and manifest')
    p.add_argument('--zip',action='store_true',help='Also create an MO2-installable ZIP; the local input audit is excluded')
    a=p.parse_args();build(a.mo2,a.output,a.profile,a.game_data,a.mods_dir,a.profiles_dir,a.overwrite_dir)
    if a.zip:
        archive=pathlib.Path(str(a.output)+'.zip')
        with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED) as z:
            for relative in ('TailorDDCosmetics.esp','SKSE/Plugins/TailorDDSafeFitting.json'):
                z.write(a.output/relative,relative)
        print('MO2 archive: '+str(archive))

