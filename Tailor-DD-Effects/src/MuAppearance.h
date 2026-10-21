#pragma once
namespace MuAppearance {
void Init();
bool RegisterPapyrus(RE::BSScript::IVirtualMachine* vm);
void Clear(bool forgetActors=false);
void Before(RE::Actor* actor);
void After(RE::Actor* actor);
// Reconcile only loaded NPCs with a verified current Mu preset after a load.
// A proven native-base regression may reapply the same Mu selection outside UI;
// body-tint-only recovery itself does not import/reset geometry.
void AfterLoad(const std::vector<RE::FormID>& managedActors={});
}
