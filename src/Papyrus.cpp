#include "Papyrus.h"

#include "Settings.h"

namespace Papyrus
{
	void DynamicCollisionAdjustment_MCM::OnConfigClose(RE::TESQuest*)
	{
		Settings::ReadSettings();
	}

	bool DynamicCollisionAdjustment_MCM::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("OnConfigClose", "DynamicCollisionAdjustment_MCM", OnConfigClose);

		logger::info("Registered DynamicCollisionAdjustment_MCM class");
		return true;
	}

	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(DynamicCollisionAdjustment_MCM::Register);
		logger::info("Registered papyrus functions");
	}
}
