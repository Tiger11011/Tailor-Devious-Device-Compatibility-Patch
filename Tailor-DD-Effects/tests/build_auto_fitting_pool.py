"""Generate our generic empty record pool. No DD/game/third-party records copied."""
from pathlib import Path
import struct,hashlib,json
FIRST=0x800
COUNT=65536
def sub(t,b):return t+struct.pack('<H',len(b))+b
def record(t,i,b,flags=0):return struct.pack('<4sIIIIHH',t,len(b),flags,i,0,44,0)+b
def build(output):
    output=Path(output);output.parent.mkdir(parents=True,exist_ok=True)
    # Full plugin: do not ESL-flag/compact these stable slot IDs.
    header=sub(b'HEDR',struct.pack('<fII',1.7,COUNT+1,FIRST+COUNT))
    header+=sub(b'CNAM',b'Tailor DD compatibility project\0')
    header+=sub(b'SNAM',b'Empty fitting slots. Populated in memory by TailorDDBridge.\0')
    # No masters or VMAD. Empty name/model/slots make unallocated entries inert.
    rows=b''.join(record(b'ARMO',i,sub(b'EDID',f'TDD_Auto_{i:06X}\0'.encode())+sub(b'BOD2',struct.pack('<II',0,2))+sub(b'DATA',struct.pack('<If',0,0.0))+sub(b'DNAM',bytes(4))) for i in range(FIRST,FIRST+COUNT))
    blob=record(b'TES4',0,header)+struct.pack('<4sI4sIII',b'GRUP',len(rows)+24,b'ARMO',0,0,0)+rows
    output.write_bytes(blob)
    return {'file':str(output),'slots':COUNT,'bytes':len(blob),'sha256':hashlib.sha256(blob).hexdigest(),'thirdPartyRecords':False,'masters':[]}
if __name__=='__main__':
    import argparse
    p=argparse.ArgumentParser();p.add_argument('output');a=p.parse_args();print(json.dumps(build(a.output),indent=2))
