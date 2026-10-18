Scriptname TailorDDFreeWear Hidden

Bool Function Authorize(Actor who, Armor device, Int requestId) Global Native
Bool Function AuthorizeEquip(Actor who, Armor device, Int requestId) Global Native
Bool Function EquipBusy(Actor who, Armor device) Global Native
Bool Function RemovalBusy(Actor who, Armor device) Global Native
Function Report(Actor who, Armor device, Int requestId, String stage) Global Native
Bool Function AppearanceAllowed(Actor who, Int requestGeneration) Global Native

; Zero is RaceMenu's face-only apply mode. Retain existing morphs, node
; overrides and skin overrides rather than clearing/reimporting them.
Bool Function RestoreFace(Actor who, Race targetRace, String presetPath, Int requestGeneration) Global
    If !who || !targetRace || presetPath == ""
        Return False
    EndIf
    If !AppearanceAllowed(who, requestGeneration)
        Return False
    EndIf
    Return CharGen.LoadCharacterEx(who, targetRace, presetPath, 0)
EndFunction

Bool Function Lock(Actor who, Armor inventoryDevice, Armor renderedDevice, Int requestId) Global
    Report(who, inventoryDevice, requestId, "DD script equip helper started")
    Int earlierRemovalAttempts = 600
    While RemovalBusy(who, inventoryDevice) && earlierRemovalAttempts > 0
        Utility.WaitMenuMode(0.05)
        earlierRemovalAttempts -= 1
    EndWhile
    If RemovalBusy(who, inventoryDevice)
        Report(who, inventoryDevice, requestId, "earlier DD removal still active; equip cancelled")
        Return False
    EndIf
    zadlibs libs = Quest.GetQuest("zadQuest") as zadlibs
    If !libs || !who || !inventoryDevice || !renderedDevice || !AuthorizeEquip(who, inventoryDevice, requestId)
        Return False
    EndIf
    ; Recover only our own inventory-only failed equip. Use DD's own removal
    ; API, not deleting the item or forcing rendered armor onto the actor.
    If who.IsEquipped(inventoryDevice) && !who.IsEquipped(renderedDevice)
        String oldToken = "zad_RemovalToken" + inventoryDevice
        If StorageUtil.GetIntValue(who, oldToken, 0) != 0
            Report(who, inventoryDevice, requestId, "equip repair declined: existing removal token")
            Return False
        EndIf
        libs.UnlockDevice(who, inventoryDevice, renderedDevice, None, False, True)
        Int repairAttempts = 200
        While repairAttempts > 0 && (who.IsEquipped(inventoryDevice) || StorageUtil.GetIntValue(who, oldToken, 0) != 0)
            Utility.WaitMenuMode(0.05)
            repairAttempts -= 1
        EndWhile
        If who.IsEquipped(inventoryDevice) || StorageUtil.GetIntValue(who, oldToken, 0) != 0
            Report(who, inventoryDevice, requestId, "equip repair did not finish")
            Return False
        EndIf
    EndIf
    If who.GetItemCount(inventoryDevice) <= 0
        who.AddItem(inventoryDevice, 1, True)
    EndIf
    ; Adding from C++ and equipping in the same frame previously missed the
    ; item's OnEquipped event. Wait for the actual inventory script to exist.
    Int bindAttempts = 200
    While bindAttempts > 0 && zadNativeFunctions.GetInventoryObjectPropertyObject(who, inventoryDevice, "", "deviceInventory") != inventoryDevice
        Utility.WaitMenuMode(0.05)
        bindAttempts -= 1
    EndWhile
    If zadNativeFunctions.GetInventoryObjectPropertyObject(who, inventoryDevice, "", "deviceInventory") != inventoryDevice || !AuthorizeEquip(who, inventoryDevice, requestId)
        Report(who, inventoryDevice, requestId, "DD inventory script not ready; equip cancelled")
        Return False
    EndIf
    Report(who, inventoryDevice, requestId, "inventory script ready; calling DD LockDevice")
    If !libs.LockDevice(who, inventoryDevice, False)
        Return False
    EndIf
    Int attempts = 300
    While attempts > 0 && (!who.IsEquipped(renderedDevice) || zadNativeFunctions.GetInventoryObjectPropertyObject(who, inventoryDevice, "", "DeviceWearer") != who)
        Utility.WaitMenuMode(0.05)
        attempts -= 1
    EndWhile
    Bool completed = who.IsEquipped(renderedDevice) && zadNativeFunctions.GetInventoryObjectPropertyObject(who, inventoryDevice, "", "DeviceWearer") == who
    If completed
        Report(who, inventoryDevice, requestId, "DD script equip completed, rendered device worn")
    Else
        Report(who, inventoryDevice, requestId, "DD script equip did not produce the rendered device")
    EndIf
    Return completed
EndFunction

; This is an independent helper. No DD implementation or translated scripts
; are replaced. The token string and UnlockDevice signature were checked
; against the locally winning DD scripts.
Bool Function Unlock(Actor who, Armor inventoryDevice, Armor renderedDevice, Int requestId) Global
    Report(who, inventoryDevice, requestId, "helper started")
    zadlibs libs = Quest.GetQuest("zadQuest") as zadlibs
    If !libs || !who || !inventoryDevice || !renderedDevice
        Report(who, inventoryDevice, requestId, "invalid dependency or device")
        Return False
    EndIf
    Keyword questItem = Keyword.GetKeyword("zad_QuestItem")
    Keyword blockGeneric = Keyword.GetKeyword("zad_BlockGeneric")
    If (questItem && (inventoryDevice.HasKeyword(questItem) || renderedDevice.HasKeyword(questItem))) || (blockGeneric && (inventoryDevice.HasKeyword(blockGeneric) || renderedDevice.HasKeyword(blockGeneric)))
        Return False
    EndIf
    ; Wait only for our own equip job. A missing rendered device can be a
    ; failed equip, which must still be removable through DD's standard API.
    Int equipAttempts = 600
    While EquipBusy(who, inventoryDevice) && equipAttempts > 0
        Utility.WaitMenuMode(0.05)
        equipAttempts -= 1
    EndWhile
    If EquipBusy(who, inventoryDevice)
        Report(who, inventoryDevice, requestId, "DD equip job still active; removal postponed")
        Return False
    EndIf
    ; Mark the operation after the token exists, so reentrant native unequip
    ; calls are allowed only through DD's authorized removal path.
    String token = "zad_RemovalToken" + inventoryDevice
    Int previous = StorageUtil.GetIntValue(who, token, 0)
    If previous != 0
        Report(who, inventoryDevice, requestId, "another removal token already exists")
        Return False
    EndIf
    Bool wasInventoryEquipped = who.IsEquipped(inventoryDevice)
    StorageUtil.SetIntValue(who, token, 1)
    If !Authorize(who, inventoryDevice, requestId)
        If previous == 0
            StorageUtil.UnsetIntValue(who, token)
        Else
            StorageUtil.SetIntValue(who, token, previous)
        EndIf
        Return False
    EndIf
    Report(who, inventoryDevice, requestId, "calling DD UnlockDevice")
    Bool accepted = libs.UnlockDevice(who, inventoryDevice, renderedDevice, None, False, True)
    If !accepted
        StorageUtil.UnsetIntValue(who, token)
        Return False
    EndIf
    If !wasInventoryEquipped && !who.IsEquipped(renderedDevice)
        StorageUtil.UnsetIntValue(who, token)
    EndIf
    Int attempts = 300
    While attempts > 0 && (who.IsEquipped(inventoryDevice) || who.IsEquipped(renderedDevice) || StorageUtil.GetIntValue(who, token, 0) != 0)
        Utility.WaitMenuMode(0.05)
        attempts -= 1
    EndWhile
    Bool completed = !who.IsEquipped(inventoryDevice) && !who.IsEquipped(renderedDevice) && StorageUtil.GetIntValue(who, token, 0) == 0
    If completed
        Report(who, inventoryDevice, requestId, "DD removal completed")
    Else
        Report(who, inventoryDevice, requestId, "DD removal wait expired")
    EndIf
    Return completed
EndFunction
