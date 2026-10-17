"""Read Skyrim PEX identity/inheritance, without treating strings as executable calls.

Format cross-checked against Orvid/Champollion Pex/FileReader.cpp (main).
This deliberately rejects other games and multiple-object files.
"""
import hashlib
import struct


def metadata(data):
    pos = 0

    def take(n):
        nonlocal pos
        if n < 0 or pos + n > len(data):
            raise ValueError('Truncated PEX metadata')
        result = data[pos:pos+n]
        pos += n
        return result

    def number(fmt):
        return struct.unpack('>' + fmt, take(struct.calcsize('>' + fmt)))[0]

    def string():
        return take(number('H')).decode('utf8')

    if take(4) != bytes.fromhex('fa57c0de'):
        raise ValueError('Not a Skyrim big-endian PEX')
    major, minor, game = number('B'), number('B'), number('H')
    if major != 3 or minor not in (1, 2) or game != 1:
        raise ValueError('Unsupported PEX version/game')
    take(8)
    for _ in range(3):
        string()
    strings = [string() for _ in range(number('H'))]
    debug = number('B')
    if debug not in (0, 1):
        raise ValueError('Invalid debug flag')
    if debug:
        take(8)
        for _ in range(number('H')):
            take(7)  # object, state, function indexes; function kind
            take(number('H') * 2)
    take(number('H') * 3)  # user flag index + bit
    if number('H') != 1:
        raise ValueError('Expected one Skyrim script object')
    name_index = number('H')
    size = number('I')
    parent_index = number('H')
    if name_index >= len(strings) or parent_index >= len(strings) or size < 2:
        raise ValueError('Invalid PEX object metadata')
    return {'name': strings[name_index].lower(), 'parent': strings[parent_index].lower(),
            'sha256': hashlib.sha256(data).hexdigest()}


class ScriptIndex:
    def __init__(self, folders):
        self.paths = {}
        self.cache = {}
        for folder in folders:  # MO2 low to high priority
            for path in (folder / 'scripts').glob('*.pex'):
                self.paths[path.stem.lower()] = path

    def ancestry(self, name):
        chain = []
        current = name.lower()
        while current not in ('objectreference', 'form', 'quest', 'activemagiceffect', ''):
            if current in {row['name'] for row in chain} or len(chain) >= 64:
                return chain, 'cyclic/excessive script inheritance'
            if current not in self.cache:
                path = self.paths.get(current)
                if not path:
                    return chain, 'winning loose PEX unavailable (BSA/runtime requires review): ' + current
                try:
                    row = metadata(path.read_bytes())
                    if row['name'] != current:
                        raise ValueError('PEX object name differs from file name')
                    self.cache[current] = {**row, 'path': str(path)}
                except Exception as error:
                    return chain, str(error)
            row = self.cache[current]
            chain.append(row)
            current = row['parent']
        return chain, None

    def is_dd_device(self, name):
        chain, error = self.ancestry(name)
        # Resolve the complete chain, including the actual winning DD base PEX.
        return not error and any(row['name'] == 'zadequipscript' for row in chain), chain, error
