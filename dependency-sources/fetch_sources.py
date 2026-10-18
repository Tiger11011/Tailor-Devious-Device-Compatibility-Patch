"""Retrieve or verify the exact upstream source archives; never run game code.

The complete release Source.zip already contains archives/. A Git clone needs
--download. Network access is explicit and only used by this developer tool.
"""
from pathlib import Path
import argparse, hashlib, json, urllib.request

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--download',action='store_true',help='Download missing archives from the pinned source URLs')
    args=parser.parse_args()
    root=Path(__file__).resolve().parent
    manifest=json.loads((root/'DEPENDENCIES.json').read_text(encoding='utf8'))
    for entry in manifest['dependencies']:
        target=root/entry['sourceArchive']
        if not target.exists():
            if not args.download:raise SystemExit('Missing '+target.name+'; use the complete release Source.zip or --download.')
            target.parent.mkdir(parents=True,exist_ok=True)
            request=urllib.request.Request(entry['sourceUrl'],headers={'User-Agent':'Tailor-DD-source-fetch'})
            with urllib.request.urlopen(request,timeout=120) as response:data=response.read()
            if hashlib.sha256(data).hexdigest()!=entry['sha256']:raise SystemExit('Download checksum mismatch: '+target.name)
            target.write_bytes(data)
        if hashlib.sha256(target.read_bytes()).hexdigest()!=entry['sha256']:raise SystemExit('Checksum mismatch: '+target.name)
        print('Verified '+entry['name']+' '+entry['version'])
    print('All 8 dependency source archives verified.')

if __name__=='__main__':main()
