; Compile-only interface declaration. Never install or distribute this as a PEX.
; Signature checked against the locally winning DD zadlibs script.
Scriptname zadlibs extends Quest
Bool Function LockDevice(Actor who, Armor inventoryDevice, Bool force = False)
    Return False
EndFunction
Bool Function UnlockDevice(Actor who, Armor inventoryDevice, Armor renderedDevice = None, Keyword deviceKeyword = None, Bool destroyDevice = False, Bool genericOnly = False)
    Return False
EndFunction
