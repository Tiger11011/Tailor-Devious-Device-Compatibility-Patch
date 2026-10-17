"""Portable, synthetic MO2 fixture; does not require Skyrim or a user's mod files."""
import importlib.util
import pathlib
import struct
import tempfile
import json
import hashlib
import contextlib
import io
import unittest

spec = importlib.util.spec_from_file_location('generator', pathlib.Path(__file__).parents[1] / 'tools/build_cosmetics.py')
g = importlib.util.module_from_spec(spec)
spec.loader.exec_module(g)


def word(text):
    raw = text.encode('utf8')
    return struct.pack('<H', len(raw)) + raw


def pex(name, parent):
    def string(value):
        raw = value.encode()
        return struct.pack('>H', len(raw)) + raw
    return (bytes.fromhex('fa57c0de03020001') + bytes(8) + string('fixture.psc') + string('') + string('')
            + struct.pack('>H', 3) + string(name) + string(parent) + string('')
            + b'\x00' + struct.pack('>HHHIH', 0, 1, 0, 19, 1)
            + struct.pack('>HIHHHH', 2, 0, 2, 0, 0, 0))


def device_plugin(addon):
    props = b''
    for name, form in [('deviceInventory', 0x01000800), ('deviceRendered', 0x01000801)]:
        props += word(name) + b'\x01\x01' + struct.pack('<HHI', 0, 0, form)
    vmad = struct.pack('<HHH', 5, 2, 1) + word('zadEquipScript') + b'\x00' + struct.pack('<H', 2) + props
    header = g.record(b'TES4', 0, g.sub(b'HEDR', struct.pack('<fII', 1.7, 3, 0x802)) + g.sub(b'MAST', b'Skyrim.esm\0') + g.sub(b'DATA', bytes(8)))
    inv = g.record(b'ARMO', 0x01000800, g.sub(b'FULL', b'Test outfit\0') + g.sub(b'VMAD', vmad))
    ren = g.record(b'ARMO', 0x01000801, g.sub(b'FULL', b'Rendered\0') + g.sub(b'MODL', struct.pack('<I', addon)) + g.sub(b'KWDA', struct.pack('<I', 0x1111)) + g.sub(b'KSIZ', struct.pack('<I', 1)) + g.sub(b'EITM', struct.pack('<I', 0x1112)))
    return header + inv + ren


class PortableGenerator(unittest.TestCase):
    def test_nondefault_profile_custom_paths_and_winning_override(self):
        with tempfile.TemporaryDirectory(prefix='Tailor fixture ') as directory:
            root = pathlib.Path(directory)
            data = root / 'Game with spaces' / 'Data'
            mods = root / 'custom mods'
            profiles = root / 'custom profiles'
            selected = profiles / 'English Test'
            selected.mkdir(parents=True)
            data.mkdir(parents=True)
            (data / 'Skyrim.esm').write_bytes(g.record(b'TES4', 0, g.sub(b'HEDR', struct.pack('<fII', 1.7, 1, 0x1236))))
            for name, addon in [('Low priority', 0x1234), ('High priority', 0x1235)]:
                folder = mods / name
                folder.mkdir(parents=True)
                (folder / 'TestDD.esp').write_bytes(device_plugin(addon))
            scripts = mods / 'High priority' / 'scripts'
            scripts.mkdir()
            (scripts / 'zadEquipScript.pex').write_bytes(pex('zadEquipScript', 'ObjectReference'))
            # MO2's list is highest priority first; the high-priority copy must win.
            (selected / 'modlist.txt').write_text('+High priority\n+Low priority\n', encoding='utf8')
            (selected / 'plugins.txt').write_text('*TestDD.esp\n', encoding='utf8')
            (root / 'ModOrganizer.ini').write_text(
                '[General]\nselected_profile=@ByteArray(English Test)\n'
                'gamePath=@ByteArray(' + str(data.parent).replace('\\', '\\\\') + ')\n'
                'mod_directory=custom mods\nprofiles_directory=custom profiles\n', encoding='utf8')
            inputs = {p: hashlib.sha256(p.read_bytes()).hexdigest() for p in root.rglob('*') if p.is_file()}
            output = root / 'Generated mod'
            with contextlib.redirect_stdout(io.StringIO()):
                g.build(root, output)
            manifest = json.loads((output / 'SKSE/Plugins/TailorDDSafeFitting.json').read_text(encoding='utf8'))
            self.assertEqual(len(manifest['mappings']), 1)
            self.assertEqual(manifest['mappings'][0]['sourcePlugin'], 'TestDD.esp')
            records = list(g.records((output / 'TailorDDCosmetics.esp').read_bytes()))
            armors = [list(g.subs(b)) for sig, fid, flags, b in records if sig == b'ARMO']
            self.assertEqual(next(g.U32(v) for k, v in armors[0] if k == b'MODL'), 0x1235)
            self.assertFalse(any(k in {b'VMAD', b'KWDA', b'KSIZ', b'EITM'} for k, v in armors[0]))
            self.assertTrue((output / 'TailorDD-generation-report.json').is_file())
            self.assertFalse((root / 'generation-report.json').exists())
            self.assertFalse(any(name in json.dumps(manifest) for name in ['Predator', 'UBE', 'Captured Dreams']))
            for path, before in inputs.items():
                self.assertEqual(hashlib.sha256(path.read_bytes()).hexdigest(), before)
            initial_id=manifest['mappings'][0]['targetId']
            # Adding a device that sorts BEFORE the old one must not renumber a saved copy.
            (mods/'High priority/AAA New.esp').write_bytes(device_plugin(0x1234))
            (selected/'plugins.txt').write_text('*AAA New.esp\n*TestDD.esp\n',encoding='utf8')
            with contextlib.redirect_stdout(io.StringIO()):g.build(root,output)
            updated=json.loads((output/'SKSE/Plugins/TailorDDSafeFitting.json').read_text(encoding='utf8'))
            self.assertEqual(next(r['targetId'] for r in updated['mappings'] if r['sourcePlugin']=='TestDD.esp'),initial_id)
            added_id=next(r['targetId'] for r in updated['mappings'] if r['sourcePlugin']=='AAA New.esp')
            (selected/'plugins.txt').write_text('*TestDD.esp\n',encoding='utf8')
            with contextlib.redirect_stdout(io.StringIO()):g.build(root,output)
            updated=json.loads((output/'SKSE/Plugins/TailorDDSafeFitting.json').read_text(encoding='utf8'))
            self.assertIn(added_id,updated['retiredIds'])
            self.assertEqual(updated['mappings'][0]['targetId'],initial_id)
            with self.assertRaises(ValueError):
                g.build(root, root / 'Bad profile output', profile='../outside')

    def test_behavior_based_script_detection(self):
        with tempfile.TemporaryDirectory() as directory:
            root = pathlib.Path(directory)
            low, high = root/'low', root/'high'
            for folder in (low, high):
                (folder/'scripts').mkdir(parents=True)
            (low/'scripts/zadEquipScript.pex').write_bytes(pex('zadEquipScript', 'ObjectReference'))
            (low/'scripts/MyCatsuit.pex').write_bytes(pex('MyCatsuit', 'zadEquipScript'))
            (low/'scripts/FakeDevice.pex').write_bytes(pex('FakeDevice', 'ObjectReference'))
            index = g.ScriptIndex([low, high])
            self.assertTrue(index.is_dd_device('MyCatsuit')[0])
            self.assertFalse(index.is_dd_device('FakeDevice')[0])
            self.assertFalse(index.is_dd_device('Missing')[0])
            # A higher-priority override can change the actual inheritance.
            (high/'scripts/MyCatsuit.pex').write_bytes(pex('MyCatsuit', 'ObjectReference'))
            self.assertFalse(g.ScriptIndex([low, high]).is_dd_device('MyCatsuit')[0])
            (high/'scripts/MyCatsuit.pex').write_bytes(b'broken')
            self.assertFalse(g.ScriptIndex([low, high]).is_dd_device('MyCatsuit')[0])


if __name__ == '__main__':
    unittest.main()
