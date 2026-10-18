; Compile-only native signatures. Never install or distribute this as a PEX.
Scriptname StorageUtil Hidden
Int Function GetIntValue(Form obj, String keyName, Int missing = 0) Global Native
Int Function SetIntValue(Form obj, String keyName, Int value) Global Native
Bool Function UnsetIntValue(Form obj, String keyName) Global Native
