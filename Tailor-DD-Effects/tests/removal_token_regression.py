"""Execute the helper's restricted Unlock control flow against simulated DD APIs.

This exercises the actual PSC statements, not Skyrim's VM or DD's full runtime.
Pass --before PATH to demonstrate the old check/call race before checking the fix.
"""
from pathlib import Path
import argparse, re, types

def function(path, name, env):
    source = path.read_text(encoding='utf-8-sig')
    match = re.search(r'Bool Function '+name+r'\([^\n]+\) Global\s*\n(.*?)\nEndFunction', source, re.S | re.I)
    assert match, name
    output = ['def run(who, inventoryDevice, renderedDevice, requestId):']
    depth = 1
    for original in match[1].splitlines():
        line = re.sub(r'("(?:[^"\\]|\\.)*")|;.*', lambda m: m[1] or '', original).strip()
        if not line: continue
        if line.lower() in ('endif', 'endwhile'):
            depth -= 1
            continue
        line = re.sub(r'\s+as\s+zadlibs\b', '', line, flags=re.I)
        line = re.sub(r'^(Bool|Int|String|Keyword|zadlibs)\s+', '', line, flags=re.I)
        line = line.replace('&&', ' and ').replace('||', ' or ')
        line = re.sub(r'!(?!=)', 'not ', line)
        for keyword in ('If', 'While', 'Return', 'True', 'False'):
            line = re.sub(r'\b'+keyword+r'\b', keyword.lower() if keyword in ('If', 'While', 'Return') else keyword, line, flags=re.I)
        if re.match(r'ElseIf\b', line, re.I):
            depth -= 1
            line = re.sub(r'^ElseIf\b', 'elif', line, flags=re.I) + ':'
        elif line.lower() == 'else':
            depth -= 1
            line = 'else:'
        elif line.startswith(('if ', 'while ')):
            line += ':'
        output.append('    '*depth + line)
        if line.endswith(':'): depth += 1
    exec('\n'.join(output), env)
    return env['run']

class Armor(str):
    def HasKeyword(self, _): return False

class Scenario:
    def __init__(self, mode='normal', token=0, authorized=True):
        self.mode, self.token, self.authorized = mode, token, authorized
        self.inventory = self.rendered = True
        self.calls = self.waits = 0
        self.events = []
    def IsEquipped(self, armor):
        return self.inventory if armor == 'inventory' else self.rendered
    def GetIntValue(self, *_): return self.token
    def SetIntValue(self, who, key, value): self.token = value
    def UnsetIntValue(self, *_): self.token = 0
    def UnlockDevice(self, *_):
        self.calls += 1
        if self.mode == 'refused': return False
        # The observed DD fallback: inventory is no longer equipped by the
        # time UnlockDevice checks it. DD removes render and emits events,
        # but this branch neither creates nor consumes a removal token.
        if self.mode == 'inventory-became-unworn': self.inventory = False
        if self.inventory:
            self.token = 1  # DD owns the token in its OnUnequipped route.
            self.inventory = False
        else:
            self.rendered = False
        return True
    def WaitMenuMode(self, _):
        self.waits += 1
        if self.mode == 'normal': self.rendered, self.token = False, 0
    def env(self):
        return dict(StorageUtil=self, Utility=self,
                    Quest=types.SimpleNamespace(GetQuest=lambda _: self),
                    Keyword=types.SimpleNamespace(GetKeyword=lambda _: None),
                    Authorize=lambda *_: self.authorized,
                    AuthorizeEquip=lambda *_: self.authorized,
                    EquipBusy=lambda *_: False, RemovalBusy=lambda *_: False,
                    Report=lambda *args: self.events.append(args[-1]))
    def run(self, path, name='Unlock'):
        return function(path, name, self.env())(self, Armor('inventory'), Armor('render'), 1)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--before', type=Path)
    args = parser.parse_args()
    fixed = Path(__file__).resolve().parents[1]/'Papyrus/TailorDDFreeWear.psc'
    if args.before:
        old = Scenario('inventory-became-unworn')
        assert not old.run(args.before)
        assert old.token == 1 and not old.inventory and not old.rendered and old.waits == 300
        print('Before: reproduced observed unworn/unworn + leaked token + timeout.')
    race = Scenario('inventory-became-unworn')
    assert race.run(fixed) and race.token == 0 and race.waits == 0
    normal = Scenario()
    assert normal.run(fixed) and normal.token == 0 and normal.waits == 1
    foreign = Scenario(token=7)
    assert not foreign.run(fixed) and foreign.token == 7 and foreign.calls == 0
    denied = Scenario(authorized=False)
    assert not denied.run(fixed) and denied.token == 0 and denied.calls == 0
    refused = Scenario('refused')
    assert not refused.run(fixed) and refused.token == 0 and refused.calls == 1
    relock = Scenario(token=7)
    assert not relock.run(fixed, 'Lock') and relock.token == 7 and relock.calls == 0
    print('After: DD fallback, asynchronous DD-owned cleanup, foreign-token preservation,')
    print('authorization/refusal paths and blocked relock passed. Skyrim VM not exercised.')
