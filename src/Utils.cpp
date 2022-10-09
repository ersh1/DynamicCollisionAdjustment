#include "Utils.h"

namespace Utils
{

	void FillCloningProcess(RE::NiCloningProcess& a_cloningProcess, const RE::NiPoint3& a_scale)
	{
		auto cloneMap = reinterpret_cast<uintptr_t>(&a_cloningProcess.cloneMap);
		auto value1 = reinterpret_cast<void**>(cloneMap + 0x18);
		*value1 = g_unkCloneValue1;

		auto processMap = reinterpret_cast<uintptr_t>(&a_cloningProcess.processMap);
		auto value2 = reinterpret_cast<void**>(processMap + 0x18);
		*value2 = g_unkCloneValue2;

		a_cloningProcess.copyType = 1;
		a_cloningProcess.appendChar = '$';

		a_cloningProcess.unk68 = a_scale;
	}

	void ToggleCharacterBumper(RE::Actor* a_actor, bool a_bEnable)
	{
		if (auto characterController = a_actor->GetCharController()) {
			float relevantWaterHeight = TESObjectREFR_GetRelevantWaterHeight(a_actor);
			bhkCharacterController_ToggleCharacterBumper(characterController, a_bEnable);
			if (auto loadedData = a_actor->loadedData) {
				if (relevantWaterHeight > loadedData->relevantWaterHeight) {
					loadedData->relevantWaterHeight = relevantWaterHeight;
				}
			}
		}
	}

}
