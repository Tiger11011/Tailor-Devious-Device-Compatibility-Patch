#pragma once
namespace MuAppearance {
void Init();
bool RegisterPapyrus(RE::BSScript::IVirtualMachine* vm);
void Clear();
void Before(RE::Actor* actor);
void After(RE::Actor* actor);
}
