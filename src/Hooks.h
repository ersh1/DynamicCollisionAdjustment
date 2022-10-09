#pragma once

#include "Havok.h"

namespace Hooks
{
	class MainUpdateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook{ RELOCATION_ID(35565, 36564) };  // 5B2FF0, 5D9F50, main update

			auto& trampoline = SKSE::GetTrampoline();
			_Nullsub = trampoline.write_call<5>(hook.address() + RELOCATION_OFFSET(0x748, 0xC26), Nullsub);  // 5B3738, 5DAB76
		}

	private:
		static void Nullsub();
		static inline REL::Relocation<decltype(Nullsub)> _Nullsub;
	};

	class CharControllerHooks
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook1{ RELOCATION_ID(77273, 79161) };  // DF0940, E335B0, bhkCharProxyController::dtor
			REL::Relocation<uintptr_t> hook2{ RELOCATION_ID(77331, 79212) };  // DF37F0, E36290, bhkCharRigidBodyController::dtor
			REL::Relocation<uintptr_t> hook3{ RELOCATION_ID(36192, 37171) };  // 5CD010, 5F14A0, Actor::InitHavok

			auto& trampoline = SKSE::GetTrampoline();
			_bhkCharacterController_dtor_Proxy = trampoline.write_call<5>(hook1.address() + RELOCATION_OFFSET(0x19, 0x19), bhkCharacterController_dtor_Proxy);           // DF0959, E335C9
			_bhkCharacterController_dtor_RigidBody = trampoline.write_call<5>(hook2.address() + RELOCATION_OFFSET(0x19, 0x10E), bhkCharacterController_dtor_RigidBody);  // DF3809, E3639E
			_bhkCharacterController_func = trampoline.write_call<5>(hook3.address() + RELOCATION_OFFSET(0x34E, 0x3B7), bhkCharacterController_func);                     // 5CD35E, 5F1857
		}
		
	private:
		static void bhkCharacterController_dtor_Proxy(RE::bhkCharProxyController* a_this);
		static void bhkCharacterController_dtor_RigidBody(RE::bhkCharRigidBodyController* a_this);
		static void bhkCharacterController_func(RE::bhkCharacterController* a_this, uint32_t a2, RE::Actor* a_actor);

		static inline REL::Relocation<decltype(bhkCharacterController_dtor_Proxy)> _bhkCharacterController_dtor_Proxy;
		static inline REL::Relocation<decltype(bhkCharacterController_dtor_RigidBody)> _bhkCharacterController_dtor_RigidBody;
		static inline REL::Relocation<decltype(bhkCharacterController_func)> _bhkCharacterController_func;
	};

	class SneakHooks
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook1{ RELOCATION_ID(36926, 37951) };  // 6089E0, 6301D0, Actor::SetSneaking

			auto& trampoline = SKSE::GetTrampoline();
			_AddMovementFlags = trampoline.write_call<5>(hook1.address() + RELOCATION_OFFSET(0xE4, 0xA0), AddMovementFlags);  // 608AC4, 630270
			_RemoveMovementFlags = trampoline.write_call<5>(hook1.address() + RELOCATION_OFFSET(0xEB, 0xB2), RemoveMovementFlags);  // 608ACB, 630282

			REL::Relocation<std::uintptr_t> SneakHandlerVtbl{ RE::VTABLE_SneakHandler[0] };
			//_CanProcess = SneakHandlerVtbl.write_vfunc(0x1, CanProcess);
			_ProcessButton = SneakHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}

	private:
		static bool AddMovementFlags(RE::ActorState* a_this, int16_t a_flag);
		static bool RemoveMovementFlags(RE::ActorState* a_this, int16_t a_flag);

		//static bool CanProcess(RE::SneakHandler* a_this, RE::InputEvent* a_event);
		static void ProcessButton(RE::SneakHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data); 


		static inline REL::Relocation<decltype(AddMovementFlags)> _AddMovementFlags;
		static inline REL::Relocation<decltype(RemoveMovementFlags)> _RemoveMovementFlags;

		//static inline REL::Relocation<decltype(CanProcess)> _CanProcess;
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class CharacterStateHooks
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook1{ RELOCATION_ID(39096, 39896) };  // 6903D0, 6A4EE0, Actor::SetInAir?
			REL::Relocation<uintptr_t> hook2{ RELOCATION_ID(78195, 80093) };  // E1D520, E62B90, bhkCharacterStateClimbing
			REL::Relocation<uintptr_t> hook3{ RELOCATION_ID(78200, 80098) };  // E1D5C0, E62C30, bhkCharacterStateInAir
			REL::Relocation<uintptr_t> hook4{ RELOCATION_ID(78204, 80102) };  // E1D960, E62FA0, bhkCharacterStateJumping
			REL::Relocation<uintptr_t> hook5{ RELOCATION_ID(78208, 80106) };  // E1DC30, E63240, bhkCharacterStateOnGround
			REL::Relocation<uintptr_t> hook6{ RELOCATION_ID(78212, 80110) };  // E1E270, E63820, bhkCharacterStateSwimming
			
			// I avoid xbyak where possible
			auto& trampoline = SKSE::GetTrampoline();
			_SetWantedState1 = trampoline.write_call<5>(hook1.address() + RELOCATION_OFFSET(0x29, 0x3CD), SetWantedState1);     // 6903F9, 6A52AD
			_SetWantedState2 = trampoline.write_branch<6>(hook2.address() + RELOCATION_OFFSET(0xC, 0xC), SetWantedState2);    // E1D52C, E62B9C
			_SetWantedState3 = trampoline.write_call<5>(hook3.address() + RELOCATION_OFFSET(0xAF, 0xB1), SetWantedState3);     // E1D66F, E62CE1
			_SetWantedState4A = trampoline.write_call<5>(hook4.address() + RELOCATION_OFFSET(0x20, 0x23), SetWantedState4A);   // E1D980, E62FC3
			_SetWantedState4B = trampoline.write_call<5>(hook4.address() + RELOCATION_OFFSET(0x1BD, 0x176), SetWantedState4B);  // E1DB1D, E63116
			_SetWantedState5A = trampoline.write_call<5>(hook5.address() + RELOCATION_OFFSET(0x2CD, 0x2A8), SetWantedState5A);  // E1DEFD, E634E8
			_SetWantedState5B = trampoline.write_call<5>(hook5.address() + RELOCATION_OFFSET(0x3B5, 0x37D), SetWantedState5B);  // E1DFE5, E635BD
			_SetWantedState6A = trampoline.write_call<5>(hook6.address() + RELOCATION_OFFSET(0x17F, 0x16F), SetWantedState6A);  // E1E3EF, E6398F
			_SetWantedState6B = trampoline.write_call<5>(hook6.address() + RELOCATION_OFFSET(0x1F2, 0x1DF), SetWantedState6B);  // E1E462, E639FA
			_SetWantedState6C = trampoline.write_call<5>(hook6.address() + RELOCATION_OFFSET(0x29E, 0x274), SetWantedState6C);  // E1E50E, E63A94
		}
	
	private:
		static void SetWantedState1(RE::bhkCharacterController* a_this);
		static void SetWantedState2(RE::bhkCharacterController* a_this);
		static void SetWantedState3(RE::bhkCharacterController* a_this);
		static void SetWantedState4A(RE::bhkCharacterController* a_this);
		static void SetWantedState4B(RE::bhkCharacterController* a_this);
		static void SetWantedState5A(RE::bhkCharacterController* a_this);
		static void SetWantedState5B(RE::bhkCharacterController* a_this);
		static void SetWantedState6A(RE::bhkCharacterController* a_this);
		static void SetWantedState6B(RE::bhkCharacterController* a_this);
		static void SetWantedState6C(RE::bhkCharacterController* a_this);
		
		static inline REL::Relocation<decltype(SetWantedState1)> _SetWantedState1;
		static inline REL::Relocation<decltype(SetWantedState2)> _SetWantedState2;
		static inline REL::Relocation<decltype(SetWantedState3)> _SetWantedState3;
		static inline REL::Relocation<decltype(SetWantedState4A)> _SetWantedState4A;
		static inline REL::Relocation<decltype(SetWantedState4B)> _SetWantedState4B;
		static inline REL::Relocation<decltype(SetWantedState5A)> _SetWantedState5A;
		static inline REL::Relocation<decltype(SetWantedState5B)> _SetWantedState5B;
		static inline REL::Relocation<decltype(SetWantedState6A)> _SetWantedState6A;
		static inline REL::Relocation<decltype(SetWantedState6B)> _SetWantedState6B;
		static inline REL::Relocation<decltype(SetWantedState6C)> _SetWantedState6C;
	};


	void Install();
}
