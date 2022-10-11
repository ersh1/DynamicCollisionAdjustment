#include "AdjustmentHandler.h"
#include "Offsets.h"
#include "Settings.h"
#include "Utils.h"

void AdjustmentHandler::OnPostLoadGame()
{
}

void AdjustmentHandler::ActorSneakStateChanged(RE::ActorHandle a_actorHandle, bool a_bIsSneaking)
{
	if (auto controllerData = GetControllerData(a_actorHandle)) {
		controllerData->bIsSneaking = a_bIsSneaking;
		controllerData->AdjustConvex();
	}
}

void AdjustmentHandler::CharacterControllerStateChanged(RE::bhkCharacterController* a_charController, RE::hkpCharacterStateType a_stateType)
{
	if (auto controllerData = GetControllerData(a_charController)) {
		controllerData->characterState = a_stateType;
		controllerData->AdjustConvex();
	}
}

bool AdjustmentHandler::CheckEnoughSpaceToStand(RE::ActorHandle a_actorHandle)
{
	auto actor = a_actorHandle.get();
	if (!actor) {
		return true;
	}

	auto cell = actor->parentCell;
	if (!cell) {
		return true;
	}

	auto world = cell->GetbhkWorld();
	if (!world) {
		return true;
	}

	auto charController = actor->GetCharController();
	if (!charController) {
		return true;
	}

	auto controllerData = GetControllerData(a_actorHandle);
	
	if (!controllerData) {
		return true;
	}
	
	if (controllerData->originalVerts.size() != 18) {
		return true;
	}

	RE::hkVector4 charControllerPos, raycastStart, raycastEnd;
	charController->GetPosition(charControllerPos, false);

	raycastStart = charControllerPos;
	raycastEnd = raycastStart + ((controllerData->originalVerts[9] * 2.f) * controllerData->actorScale) + controllerData->originalVerts[8];
	
	RE::hkpWorldRayCastInput raycastInput;
	RE::hkpWorldRayCastOutput raycastOutput;
	
	uint32_t collisionFilterInfo = 0;
	actor->GetCollisionFilterInfo(collisionFilterInfo);
	uint16_t collisionGroup = collisionFilterInfo >> 16;
	raycastInput.filterInfo = (static_cast<uint32_t>(collisionGroup) << 16) | static_cast<uint32_t>(RE::COL_LAYER::kCharController);
	raycastInput.from = raycastStart;
	raycastInput.to = raycastEnd;
	
	{
		RE::BSReadLockGuard lock(world->worldLock);
		world->GetWorld1()->CastRay(raycastInput, raycastOutput);
	}

	if (raycastOutput.HasHit()) {
		if (Settings::g_trueHUD && Settings::uDisplayDebugShapes != DebugDrawMode::kNone) {
			Settings::g_trueHUD->DrawArrow(Utils::HkVectorToNiPoint(raycastStart, true), Utils::HkVectorToNiPoint(raycastEnd, true), 10.f, 1.f);
		}
		return false;
	}

	return true;
}

bool AdjustmentHandler::GetShapes(RE::bhkCharacterController* a_charController, const RE::hkpConvexVerticesShape*& a_outCollisionConvexVerticesShape, std::vector<RE::hkpCapsuleShape*>& a_outCollisionCapsules)
{
	const auto readShape = [&](const auto& self, const RE::hkpShape* a_shape) -> bool {
		if (a_shape) {
			switch (a_shape->type) {
			case RE::hkpShapeType::kCapsule:
				if (auto capsuleShape = const_cast<RE::hkpCapsuleShape*>(skyrim_cast<const RE::hkpCapsuleShape*>(a_shape))) {
					a_outCollisionCapsules.emplace_back(capsuleShape);
				}
				break;
			case RE::hkpShapeType::kConvexVertices:
				if (auto convexVerticesShape = const_cast<RE::hkpConvexVerticesShape*>(skyrim_cast<const RE::hkpConvexVerticesShape*>(a_shape))) {
					assert(a_outCollisionConvexVerticesShape == nullptr);
					a_outCollisionConvexVerticesShape = convexVerticesShape;
					return true;
				}
				break;
			case RE::hkpShapeType::kList:
				if (auto listShape = skyrim_cast<RE::hkpListShape*>(const_cast<RE::hkpShape*>(a_shape))) {
					for (auto& childInfo : listShape->childInfo) {
						if (auto child = childInfo.shape) {
							self(self, child);
						}
					}
				}
				break;
			case RE::hkpShapeType::kMOPP:
				if (auto moppShape = const_cast<RE::hkpMoppBvTreeShape*>(skyrim_cast<const RE::hkpMoppBvTreeShape*>(a_shape))) {
					RE::hkpShapeBuffer buf{};
					RE::hkpShapeKey shapeKey = moppShape->child.GetFirstKey();
					for (int i = 0; i < moppShape->child.GetNumChildShapes(); ++i) {
						if (auto child = moppShape->child.GetChildShape(shapeKey, buf)) {
							self(self, child);
						}
						shapeKey = moppShape->child.GetNextKey(shapeKey);
					}
				}
				break;
			}
		}

		return false;
	};
	
	if (auto proxyController = skyrim_cast<RE::bhkCharProxyController*>(a_charController)) {
		if (auto charProxy = static_cast<RE::hkpCharacterProxy*>(proxyController->proxy.referencedObject.get())) {

			readShape(readShape, charProxy->shapePhantom->collidable.shape);

			return a_outCollisionConvexVerticesShape || !a_outCollisionCapsules.empty();
		}
	} else if (auto rigidBodyController = skyrim_cast<RE::bhkCharRigidBodyController*>(a_charController)) {
		if (auto rigidBody = static_cast<RE::hkpCharacterRigidBody*>(rigidBodyController->rigidBody.referencedObject.get())) {
			
			readShape(readShape, rigidBody->character->collidable.shape);

			return a_outCollisionConvexVerticesShape || !a_outCollisionCapsules.empty();
		}
	}

	return false;
}

bool AdjustmentHandler::GetConvexShape(RE::bhkCharacterController* a_charController, RE::hkpCharacterProxy*& a_outProxy, RE::hkpCharacterRigidBody*& a_outRigidBody, RE::hkpListShape*& a_outListShape, RE::hkpConvexVerticesShape*& a_outCollisionConvexVerticesShape)
{
	if (auto proxyController = skyrim_cast<RE::bhkCharProxyController*>(a_charController)) {
		a_outProxy = static_cast<RE::hkpCharacterProxy*>(proxyController->proxy.referencedObject.get());
		if (a_outProxy) {
			a_outListShape = skyrim_cast<RE::hkpListShape*>(const_cast<RE::hkpShape*>(a_outProxy->shapePhantom->collidable.shape));
			a_outCollisionConvexVerticesShape = skyrim_cast<RE::hkpConvexVerticesShape*>(const_cast<RE::hkpShape*>(a_outListShape ? a_outListShape->childInfo[0].shape : a_outProxy->shapePhantom->collidable.shape));
			return a_outListShape && a_outCollisionConvexVerticesShape;
		}
	} else if (auto rigidBodyController = skyrim_cast<RE::bhkCharRigidBodyController*>(a_charController)) {
		a_outRigidBody = static_cast<RE::hkpCharacterRigidBody*>(rigidBodyController->rigidBody.referencedObject.get());
		if (a_outRigidBody) {
			a_outListShape = skyrim_cast<RE::hkpListShape*>(const_cast<RE::hkpShape*>(a_outRigidBody->character->collidable.shape));
			a_outCollisionConvexVerticesShape = skyrim_cast<RE::hkpConvexVerticesShape*>(const_cast<RE::hkpShape*>(a_outListShape ? a_outListShape->childInfo[0].shape : a_outRigidBody->character->collidable.shape));
			return a_outListShape && a_outCollisionConvexVerticesShape;
		}
	}

	return false;
}

std::shared_ptr<AdjustmentHandler::ControllerData> AdjustmentHandler::GetControllerData(RE::ActorHandle a_actorHandle)
{
	ReadLocker locker(controllersLock);

	if (auto actor = a_actorHandle.get()) {
		if (auto charController = actor->GetCharController()) {
			auto search = _controllers.find(charController);
			if (search != _controllers.end()) {
				return search->second;
			}
		}
	}
	
	return nullptr;
}

std::shared_ptr<AdjustmentHandler::ControllerData> AdjustmentHandler::GetControllerData(RE::bhkCharacterController* a_charController)
{
	ReadLocker locker(controllersLock);
	
	auto search = _controllers.find(a_charController);
	if (search != _controllers.end()) {
		return search->second;
	}

	return nullptr;
}

void AdjustmentHandler::ControllerData::Initialize()
{
	if (auto actor = actorHandle.get()) {
		RE::hkpCharacterProxy* proxy = nullptr;
		RE::hkpCharacterRigidBody* rigidBody = nullptr;
		RE::hkpListShape* listShape = nullptr;
		RE::hkpConvexVerticesShape* collisionConvexVerticesShape = nullptr;

		auto cell = actor->GetParentCell();
		if (!cell) {
			return;
		}

		auto world = RE::NiPointer<RE::bhkWorld>(cell->GetbhkWorld());
		if (!world) {
			return;
		}

		actorScale = actor->GetScale();
		bIsSneaking = actor->IsSneaking();

		RE::BSWriteLockGuard lock(world->worldLock);
		
		// save convex shape values
		if (GetConvexShape(controller, proxy, rigidBody, listShape, collisionConvexVerticesShape)) {
			RE::hkArray<RE::hkVector4> verts{};
			hkpConvexVerticesShape_getOriginalVertices(collisionConvexVerticesShape, verts);
			for (int i = 0; i < verts.size(); ++i) {
				originalVerts.emplace_back(verts[i]);
			}

			RE::NiPoint3 vert = Utils::HkVectorToNiPoint(verts[0]);
			vert.z = 0.f;
			originalConvexRadius = vert.Length();
		}
	}
}

void AdjustmentHandler::ControllerData::AdjustScale()
{
	if (Settings::bEnableActorScaleFix) {
		if (auto actor = actorHandle.get()) {
			auto cell = actor->GetParentCell();
			if (!cell) {
				return;
			}

			auto world = RE::NiPointer<RE::bhkWorld>(cell->GetbhkWorld());
			if (!world) {
				return;
			}

			if (!controller) {
				return;
			}

			RE::BSWriteLockGuard lock(world->worldLock);

			int8_t shapeIdx = -1;
			if (controller->shapes[0]) {
				shapeIdx = 0;
			} else if (controller->shapes[1]) {
				shapeIdx = 1;
			}

			if (shapeIdx < 0) {
				return;
			}

			if (auto proxyController = skyrim_cast<RE::bhkCharProxyController*>(controller)) {
				if (auto proxy = static_cast<RE::hkpCharacterProxy*>(proxyController->proxy.referencedObject.get())) {
					if (auto wrapper = proxy->shapePhantom->collidable.shape->userData) {
						auto clone = RE::NiPointer<RE::bhkShape>(Utils::Clone<RE::bhkShape>(wrapper, { actorScale, actorScale, actorScale }));
						proxy->shapePhantom->SetShape(static_cast<RE::hkpShape*>(clone->referencedObject.get()));
						controller->shapes[shapeIdx]->DecRefCount();
						controller->shapes[shapeIdx]->DecRefCount();
						controller->shapes[shapeIdx] = clone;
					}
				}
			} else if (auto rigidBodyController = skyrim_cast<RE::bhkCharRigidBodyController*>(controller)) {
				if (auto rigidBody = static_cast<RE::hkpCharacterRigidBody*>(rigidBodyController->rigidBody.referencedObject.get())) {
					if (auto wrapper = rigidBody->character->collidable.shape->userData) {
						auto clone = RE::NiPointer<RE::bhkShape>(Utils::Clone<RE::bhkShape>(wrapper, { actorScale, actorScale, actorScale }));
						rigidBody->character->SetShape(static_cast<RE::hkpShape*>(clone->referencedObject.get()));
						controller->shapes[shapeIdx]->DecRefCount();
						controller->shapes[shapeIdx]->DecRefCount();
						controller->shapes[shapeIdx] = clone;
					}
				}
			}

			bool* bumperEnabled = reinterpret_cast<bool*>(&controller->unk320);
			*bumperEnabled = true;
			Utils::ToggleCharacterBumper(actor.get(), false);
		}
	}	
}

// credit to https://github.com/adamhynek for controller shape adjustment code
void AdjustmentHandler::ControllerData::AdjustConvex()
{
	if (Settings::bEnableStateAdjustments) {
		if (auto actor = actorHandle.get()) {
			RE::hkpCharacterProxy* proxy = nullptr;
			RE::hkpCharacterRigidBody* rigidBody = nullptr;
			RE::hkpListShape* listShape = nullptr;
			RE::hkpConvexVerticesShape* collisionConvexVerticesShape = nullptr;

			auto cell = actor->GetParentCell();
			if (!cell) {
				return;
			}

			auto world = RE::NiPointer<RE::bhkWorld>(cell->GetbhkWorld());
			if (!world) {
				return;
			}

			float sneakMult = bIsSneaking && characterState == RE::hkpCharacterStateType::kOnGround ? Settings::fSneakControllerShapeHeightMultiplier : 1.f;
			float swimmingHeightMult = characterState == RE::hkpCharacterStateType::kSwimming ? Settings::fSwimmingControllerShapeHeightMultiplier : 1.f;
			float swimmingRadiusMult = characterState == RE::hkpCharacterStateType::kSwimming ? Settings::fSwimmingControllerShapeRadiusMultiplier : 1.f;
			float scaleMult = actorScale;

			float heightMult = sneakMult * swimmingHeightMult * scaleMult;
			float radiusMult = scaleMult * swimmingRadiusMult;

			RE::BSWriteLockGuard lock(world->worldLock);

			if (GetConvexShape(controller, proxy, rigidBody, listShape, collisionConvexVerticesShape)) {
				std::vector<RE::hkVector4> newVerts = originalVerts;
				if (originalVerts.size() == 18) {
					RE::hkVector4 topVert = originalVerts[9];
					RE::hkVector4 bottomVert = originalVerts[8];

					RE::hkVector4 newTopVert = ((topVert * 2.f) * heightMult) + bottomVert;
					float distance = topVert.GetDistance3(newTopVert);

					// Move the top vert
					newVerts[9] = newTopVert;
					// Move the top ring
					for (int i : { 1, 3, 4, 5, 7, 11, 13, 16 }) {  // top ring
						if (heightMult < 1.f) {
							newVerts[i].quad.m128_f32[2] = newVerts[i].quad.m128_f32[2] - distance;
						} else {
							newVerts[i].quad.m128_f32[2] = newVerts[i].quad.m128_f32[2] + distance;
						}
					}

					// move the rings' vertices inwards or outwards
					for (int i : {
							 1, 3, 4, 5, 7, 11, 13, 16,   // top ring
							 0, 2, 6, 10, 12, 14, 15, 17  // bottom ring
						 }) {
						RE::NiPoint3 vert = Utils::HkVectorToNiPoint(newVerts[i]);
						RE::NiPoint3 newVert = vert;
						newVert.z = 0;
						newVert.Unitize();
						newVert *= originalConvexRadius * radiusMult;
						newVert.z = vert.z;

						newVerts[i] = Utils::NiPointToHkVector(newVert);
					}
				}

				RE::hkStridedVertices stridedVerts(newVerts.data(), static_cast<int>(newVerts.size()));
				RE::hkpConvexVerticesShape::BuildConfig buildConfig{ false, false, true, 0.05f, 0, 0.f, 0.f, -0.1f };

				RE::hkpConvexVerticesShape* newShape = reinterpret_cast<RE::hkpConvexVerticesShape*>(hkHeapAlloc(sizeof(RE::hkpConvexVerticesShape)));
				hkpConvexVerticesShape_ctor(newShape, stridedVerts, buildConfig);  // sets refcount to 1

				// it's actually a hkCharControllerShape not just a hkpConvexVerticesShape
				reinterpret_cast<std::uintptr_t*>(newShape)[0] = RE::VTABLE_hkCharControllerShape[0].address();

				RE::bhkShape* wrapper = collisionConvexVerticesShape->userData;
				if (wrapper) {
					wrapper->SetReferencedObject(newShape);
				}

				// The listshape does not use a hkRefPtr but it's still setup to add a reference upon construction and remove one on destruction
				if (listShape) {
					listShape->childInfo[0].shape = newShape;
					collisionConvexVerticesShape->RemoveReference();  // this will usually call the dtor on the old shape

					// We don't need to remove a ref here, the ctor gave it a refcount of 1 and we assigned it to the listShape which isn't technically a hkRefPtr but still owns it (and the listShape's dtor will decref anyways)
					//newShape->RemoveReference();
				} else {
					if (proxy) {
						proxy->shapePhantom->SetShape(newShape);
					} else if (rigidBody) {
						rigidBody->character->SetShape(newShape);
					}

					newShape->RemoveReference();
				}
			}
		}
	}
}

void DrawLine(RE::hkVector4& a_start, RE::hkVector4& a_end, RE::NiPoint3& a_controllerPos) {
	RE::NiPoint3 a = Utils::HkVectorToNiPoint(a_start) * *g_worldScaleInverse;
	RE::NiPoint3 b = Utils::HkVectorToNiPoint(a_end) * *g_worldScaleInverse;
	a += a_controllerPos;
	b += a_controllerPos;
	Settings::g_trueHUD->DrawLine(a, b, 0.f, 0x00FF00FF);
}

void AdjustmentHandler::DrawVerts()
{
	auto trueHUD = Settings::g_trueHUD;
	if (trueHUD && Settings::uDisplayDebugShapes != DebugDrawMode::kNone) {
		if (RE::UI::GetSingleton()->GameIsPaused()) {
			return;
		}

		auto drawCharController = [&](RE::bhkCharacterController* a_controller, RE::ActorHandle a_actorHandle) {
			if (a_controller) {
				if (auto actor = a_actorHandle.get()) {
					const RE::hkpConvexVerticesShape* collisionConvexVerticesShape = nullptr;
					std::vector<RE::hkpCapsuleShape*> collisionCapsules{};

					auto cell = actor->GetParentCell();
					if (!cell) {
						return;
					}

					auto world = RE::NiPointer<RE::bhkWorld>(cell->GetbhkWorld());
					if (!world) {
						return;
					}

					RE::BSReadLockGuard lock(world->worldLock);

					GetShapes(a_controller, collisionConvexVerticesShape, collisionCapsules);

					RE::hkVector4 controllerPos;
					a_controller->GetPosition(controllerPos, false);
					auto controllerNiPos = Utils::HkVectorToNiPoint(controllerPos) * *g_worldScaleInverse;

					if (collisionConvexVerticesShape) {
						// The charcontroller shape is composed of two vertically concentric "rings" with a single point above and below the top/bottom ring.
						// verts 0,2,6,10,12,14,15,17 are bottom ring, 8-9 are bottom/top points, 1,3,4,5,7,11,13,16 are top ring
						
						RE::hkArray<RE::hkVector4> verts{};
						hkpConvexVerticesShape_getOriginalVertices(collisionConvexVerticesShape, verts);

						if (verts.size() == 18) {
							// draw top ring of verts
							std::vector<int> topRing = { 1, 4, 13, 7, 3, 16, 5, 11, 1 };
							for (auto iter = topRing.begin(); iter != topRing.end() - 1; ++iter) {
								DrawLine(verts[*iter], verts[*(iter + 1)], controllerNiPos);
							}

							// draw bottom ring of verts
							std::vector<int> bottomRing = { 0, 2, 12, 6, 15, 17, 14, 10, 0 };
							for (auto iter = bottomRing.begin(); iter != bottomRing.end() - 1; ++iter) {
								DrawLine(verts[*iter], verts[*(iter + 1)], controllerNiPos);
							}

							// draw vertical lines
							std::vector<int> pairs = { 1, 0, 4, 2, 13, 12, 7, 6, 3, 15, 16, 17, 5, 14, 11, 10, 9, 1, 9, 4, 9, 13, 9, 7, 9, 3, 9, 16, 9, 5, 9, 11, 8, 0, 8, 2, 8, 12, 8, 6, 8, 15, 8, 17, 8, 14, 8, 10, 0, 0 };
							for (auto iter = pairs.begin(); iter != pairs.end() - 2;) {
								DrawLine(verts[*iter], verts[*(iter + 1)], controllerNiPos);
								iter += 2;
							}
						} else if (verts.size() == 17) {  // very short - no top vert. 8 is bottom
							// draw top ring of verts
							std::vector<int> topRing = { 1, 4, 12, 7, 3, 15, 5, 10, 1 };
							for (auto iter = topRing.begin(); iter != topRing.end() - 1; ++iter) {
								DrawLine(verts[*iter], verts[*(iter + 1)], controllerNiPos);
							}

							// draw bottom ring of verts
							std::vector<int> bottomRing = { 0, 2, 11, 6, 14, 16, 13, 9, 0 };
							for (auto iter = bottomRing.begin(); iter != bottomRing.end() - 1; ++iter) {
								DrawLine(verts[*iter], verts[*(iter + 1)], controllerNiPos);
							}

							// draw vertical lines
							std::vector<int> pairs = { 1, 0, 4, 2, 12, 11, 7, 6, 3, 14, 15, 16, 5, 13, 10, 9, 8, 0, 8, 2, 8, 11, 8, 6, 8, 14, 8, 16, 8, 13, 8, 9, 0, 0 };
							for (auto iter = pairs.begin(); iter != pairs.end() - 2;) {
								DrawLine(verts[*iter], verts[*(iter + 1)], controllerNiPos);
								iter += 2;
							}
						} else {
							//logger::debug("{} - {}", actor->GetDisplayFullName(), verts.size());
						}
					}

					for (auto& capsule : collisionCapsules) {
						constexpr RE::NiPoint3 upVector{ 0.f, 0.f, 1.f };
						RE::NiQuaternion rotation{};
						rotation.w = 1.f;

						//auto collisionCapsuleHeight = capsule->vertexA.GetDistance3(capsule->vertexB) * *g_worldScaleInverse;
						auto collisionCapsuleOrigin = Utils::HkVectorToNiPoint((capsule->vertexA + capsule->vertexB) / 2.f) * *g_worldScaleInverse;
						collisionCapsuleOrigin = Utils::RotateAngleAxis(collisionCapsuleOrigin, -actor->data.angle.z, upVector);
						collisionCapsuleOrigin += controllerNiPos;
						auto collisionCapsuleRadius = capsule->radius * *g_worldScaleInverse;
						RE::NiPoint3 pointA = Utils::HkVectorToNiPoint(capsule->vertexA) * *g_worldScaleInverse;
						RE::NiPoint3 pointB = Utils::HkVectorToNiPoint(capsule->vertexB) * *g_worldScaleInverse;

						pointA = Utils::RotateAngleAxis(pointA, -actor->data.angle.z, upVector);
						pointB = Utils::RotateAngleAxis(pointB, -actor->data.angle.z, upVector);

						pointA += controllerNiPos;
						pointB += controllerNiPos;
						
						uint32_t color = 0xFFFF00FF;
						if (auto bhkShape = capsule->userData) {
							if (bhkShape->materialID == RE::MATERIAL_ID::kDragonSkeleton) {  // mislabeled in clib, it's the character bumper material
								if (!Settings::bDisplayCharacterBumper) {
									continue;
								}
								color = 0x004087FF;
							}
						}
						trueHUD->DrawCapsule(pointA, pointB, collisionCapsuleRadius, 0.f, color);
					}
				}
			}
		};

		switch (Settings::uDisplayDebugShapes) {
		case DebugDrawMode::kAdjusted:
			AdjustmentHandler::ForEachController([&](std::shared_ptr<ControllerData> entry) {
				drawCharController(entry->controller, entry->actorHandle);
			});
			break;
		case DebugDrawMode::kAll:
			auto playerHandle = RE::PlayerCharacter::GetSingleton()->GetHandle();
			auto playerController = RE::PlayerCharacter::GetSingleton()->GetCharController();
			drawCharController(playerController, playerHandle);

			for (auto& handle : RE::ProcessLists::GetSingleton()->highActorHandles) {
				if (auto actor = handle.get()) {
					if (auto controller = actor->GetCharController()) {
						drawCharController(controller, handle);
					}
				}
			}
		}
	}
}

void AdjustmentHandler::AddControllerToMap(RE::bhkCharacterController* a_controller, RE::ActorHandle a_actorHandle)
{
	WriteLocker locker(controllersLock);
	
	_controllers.emplace(a_controller, std::make_shared<ControllerData>(a_controller, a_actorHandle));
}

void AdjustmentHandler::RemoveControllerFromMap(RE::bhkCharacterController* a_controller)
{
	WriteLocker locker(controllersLock);
	
	_controllers.erase(a_controller);
}

void AdjustmentHandler::ForEachController(std::function<void(std::shared_ptr<ControllerData>)> a_func)
{
	ReadLocker locker(controllersLock);

	for (auto& entry : _controllers) {
		a_func(entry.second);
	}
}

bool AdjustmentHandler::CheckSkeletonForCollisionShapes(RE::NiAVObject* a_object)
{
	if (!a_object) {
		return false;
	}

	if (auto& collisionObject = a_object->collisionObject) {
		if (auto bhkCollisionObject = collisionObject->AsBhkNiCollisionObject()) {
			if (bhkCollisionObject->body) {
				if (auto& referencedObject = bhkCollisionObject->body->referencedObject) {
					if (auto worldObject = static_cast<RE::hkpWorldObject*>(referencedObject.get())) {
						if (auto collidable = worldObject->GetCollidable()) {
							if (collidable->shape && collidable->shape->userData) {
								auto layer = static_cast<RE::COL_LAYER>(collidable->broadPhaseHandle.collisionFilterInfo & 0x7F);
								if (layer == RE::COL_LAYER::kCharController) {
									if (collidable->shape->userData->materialID != RE::MATERIAL_ID::kDragonSkeleton) {  // not actually dragon skeleton, this is mislabeled in clib
										return true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	if (auto node = a_object->AsNode()) {
		if (node->children.size() > 0) {
			for (auto& child : node->children) {
				if (CheckSkeletonForCollisionShapes(child.get())) {
					return true;
				}
			}
		}
	}

	return false;
}
