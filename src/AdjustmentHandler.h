#pragma once

#include "Havok.h"

#include <shared_mutex>

class AdjustmentHandler
{
public:
	struct ControllerData
	{
	public:
		ControllerData(RE::bhkCharacterController* a_controller, RE::ActorHandle a_actorHandle) :
			controller(a_controller), actorHandle(a_actorHandle)
		{
			Initialize();
			AdjustScale();
			AdjustConvex();
		}

		RE::bhkCharacterController* controller;
		RE::ActorHandle actorHandle;
		
		void Initialize();
		void AdjustScale();
		void AdjustConvex();

		float actorScale = 1.f;
		bool bIsSneaking = false;
		RE::hkpCharacterStateType characterState = RE::hkpCharacterStateType::kOnGround;

		std::vector<RE::hkVector4> originalVerts{};		
		float originalConvexRadius;
	};

	static AdjustmentHandler* GetSingleton()
	{
		static AdjustmentHandler singleton;
		return std::addressof(singleton);
	}

	void OnPostLoadGame();

	void ActorSneakStateChanged(RE::ActorHandle a_actorHandle, bool a_bIsSneaking);
	void CharacterControllerStateChanged(RE::bhkCharacterController* a_charController, RE::hkpCharacterStateType a_stateType);

	static bool CheckEnoughSpaceToStand(RE::ActorHandle a_actorHandle);

	void DrawVerts();

	static void AddControllerToMap(RE::bhkCharacterController* a_controller, RE::ActorHandle a_actorHandle);
	static void RemoveControllerFromMap(RE::bhkCharacterController* a_controller);

	static void ForEachController(std::function<void(std::shared_ptr<ControllerData>)> a_func);

	static bool CheckSkeletonForCollisionShapes(RE::NiAVObject* a_object);

private:
	using Lock = std::shared_mutex;
	using ReadLocker = std::shared_lock<Lock>;
	using WriteLocker = std::unique_lock<Lock>;
	
	static inline Lock controllersLock;
	
	AdjustmentHandler() = default;
	AdjustmentHandler(const AdjustmentHandler&) = delete;
	AdjustmentHandler(AdjustmentHandler&&) = delete;
	virtual ~AdjustmentHandler() = default;

	AdjustmentHandler& operator=(const AdjustmentHandler&) = delete;
	AdjustmentHandler& operator=(AdjustmentHandler&&) = delete;

	static bool GetShapes(RE::bhkCharacterController* a_charController, const RE::hkpConvexVerticesShape*& a_outCollisionConvexVerticesShape, std::vector<RE::hkpCapsuleShape*>& a_outCollisionCapsules);
	static bool GetConvexShape(RE::bhkCharacterController* a_charController, RE::hkpCharacterProxy*& a_outProxy, RE::hkpCharacterRigidBody*& a_outRigidBody, RE::hkpListShape*& a_outListShape, RE::hkpConvexVerticesShape*& a_outCollisionConvexVerticesShape);

	static std::shared_ptr<ControllerData> GetControllerData(RE::ActorHandle a_actorHandle);
	static std::shared_ptr<ControllerData> GetControllerData(RE::bhkCharacterController* a_charController);

	static inline std::unordered_map<RE::bhkCharacterController*, std::shared_ptr<ControllerData>> _controllers{};
};
