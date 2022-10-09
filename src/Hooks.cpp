#include "Hooks.h"

#include "AdjustmentHandler.h"
#include "Settings.h"

#include <xbyak/xbyak.h>

namespace Hooks
{
	void Install()
	{
		logger::trace("Hooking...");

		SneakHooks::Hook();
		CharacterStateHooks::Hook();
		MainUpdateHook::Hook();
		CharControllerHooks::Hook();

		logger::trace("...success");
	}

	bool SneakHooks::AddMovementFlags(RE::ActorState* a_this, int16_t a_flag)
	{
		bool ret = _AddMovementFlags(a_this, a_flag);

		auto actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB8);
		if (actor) {
			AdjustmentHandler::GetSingleton()->ActorSneakStateChanged(actor->GetHandle(), true);
		}
		
		return ret;
	}

	bool SneakHooks::RemoveMovementFlags(RE::ActorState* a_this, int16_t a_flag)
	{
		bool ret = _RemoveMovementFlags(a_this, a_flag);

		auto actor = SKSE::stl::adjust_pointer<RE::Actor>(a_this, -0xB8);
		if (actor) {
			AdjustmentHandler::GetSingleton()->ActorSneakStateChanged(actor->GetHandle(), false);
		}

		return ret;
	}

	void SneakHooks::ProcessButton(RE::SneakHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (playerCharacter && playerCharacter->IsSneaking()) {
			if (!AdjustmentHandler::CheckEnoughSpaceToStand(playerCharacter->GetHandle())) {
				return;
			}
		}
		
		_ProcessButton(a_this, a_event, a_data);
	}

	/*bool SneakHooks::CanProcess(RE::SneakHandler* a_this, RE::InputEvent* a_event)
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		
		if (playerCharacter && playerCharacter->IsSneaking()) {
			if (!AdjustmentHandler::CheckEnoughSpaceToStand(playerCharacter->GetHandle())) {
				return false;
			}
		}
			
		return _CanProcess(a_this, a_event);
	}*/

	void MainUpdateHook::Nullsub()
	{
		_Nullsub();

		AdjustmentHandler::GetSingleton()->DrawVerts();
	}

	void CharControllerHooks::bhkCharacterController_dtor_Proxy(RE::bhkCharProxyController* a_this)
	{
		AdjustmentHandler::RemoveControllerFromMap(a_this);
		
		_bhkCharacterController_dtor_Proxy(a_this);
	}

	void CharControllerHooks::bhkCharacterController_dtor_RigidBody(RE::bhkCharRigidBodyController* a_this)
	{
		AdjustmentHandler::RemoveControllerFromMap(a_this);

		_bhkCharacterController_dtor_RigidBody(a_this);
	}

	struct Unk58
	{
		uint64_t unk00;
		uint32_t unk08;
		uint32_t flags;
		uint64_t unk10;
		uint64_t unk18;
		uint64_t unk20;
		RE::NiAVObject* object;

		inline bool CheckFlags() {
			return (((flags & 0x70000000) - 0x30000000) & 0xEFFFFFFF) == 0;
		}
	};

	void CharControllerHooks::bhkCharacterController_func(RE::bhkCharacterController* a_this, uint32_t a2, RE::Actor* a_actor)
	{
		_bhkCharacterController_func(a_this, a2, a_actor);

		if (a_actor) {
			if (auto loadedData = a_actor->loadedData) {
				RE::NiAVObject* object = loadedData->data3D.get();
				if (loadedData->unk60 && loadedData->unk58) {
					if (auto pUnk58 = reinterpret_cast<Unk58**>(loadedData->unk58)) {
						auto unk58 = *pUnk58;
						if (unk58->CheckFlags()) {
							object = unk58->object;
						}
					}
				}
				if (AdjustmentHandler::CheckSkeletonForCollisionShapes(object) || (a_actor->HasKeyword(Settings::kywd_NPC) && !a_actor->IsChild())) {
					AdjustmentHandler::AddControllerToMap(a_this, a_actor->GetHandle());
				}
			}
		}
	}

	void CharacterStateHooks::SetWantedState1(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}
		
		_SetWantedState1(a_this);
	}

	void CharacterStateHooks::SetWantedState2(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState2(a_this);
	}

	void CharacterStateHooks::SetWantedState3(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState3(a_this);
	}

	void CharacterStateHooks::SetWantedState4A(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState4A(a_this);
	}

	void CharacterStateHooks::SetWantedState4B(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState4B(a_this);
	}

	void CharacterStateHooks::SetWantedState5A(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState5A(a_this);
	}

	void CharacterStateHooks::SetWantedState5B(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState5B(a_this);
	}

	void CharacterStateHooks::SetWantedState6A(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState6A(a_this);
	}

	void CharacterStateHooks::SetWantedState6B(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState6B(a_this);
	}

	void CharacterStateHooks::SetWantedState6C(RE::bhkCharacterController* a_this)
	{
		if (a_this->wantState != RE::hkpCharacterStateType::kTotal) {
			AdjustmentHandler::GetSingleton()->CharacterControllerStateChanged(a_this, a_this->wantState);
		}

		_SetWantedState6C(a_this);
	}

}
