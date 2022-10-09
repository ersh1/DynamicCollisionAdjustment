#pragma once
#include "Havok.h"

static float* g_worldScale = (float*)RELOCATION_ID(231896, 188105).address();
static float* g_worldScaleInverse = (float*)RELOCATION_ID(230692, 187407).address();
static DWORD* g_dwTlsIndex = (DWORD*)RELOCATION_ID(520865, 407383).address();  // 2F50B74, 2FEB6F4

static void* g_unkCloneValue1 = (void*)RELOCATION_ID(501133, 359452).address();          // 1DB348C, 1E47508
static void* g_unkCloneValue2 = (void*)RELOCATION_ID(501132, 359451).address();          // 1DB3488, 1E47504
static uint32_t* g_unkCloneValue3 = (uint32_t*)RELOCATION_ID(523909, 410490).address();  // 3012500, 30AD080
static char* g_unkCloneValue4 = (char*)RELOCATION_ID(511989, 388581).address(); 

typedef void (*thkpConvexVerticesShape_getOriginalVertices)(const RE::hkpConvexVerticesShape* a_this, RE::hkArray<RE::hkVector4>& a_outVertices);
static REL::Relocation<thkpConvexVerticesShape_getOriginalVertices> hkpConvexVerticesShape_getOriginalVertices{ RELOCATION_ID(64067, 65093) };  // B5E120, B83290

typedef void (*thkpConvexVerticesShape_ctor)(RE::hkpConvexVerticesShape*, const RE::hkStridedVertices& a_vertices, const RE::hkpConvexVerticesShape::BuildConfig& a_buildConfig);
static REL::Relocation<thkpConvexVerticesShape_ctor> hkpConvexVerticesShape_ctor{ RELOCATION_ID(78843, 80831) };  // E43640, E895C0
																												  //static REL::Relocation<thkpConvexVerticesShape_ctor> hkpConvexVerticesShape_ctor{ RELOCATION_ID(64063, 65089) };  // B5DDC0, B82F30
typedef void (*tNiMatrixToNiQuaternion)(RE::NiQuaternion& quatOut, const RE::NiMatrix3& matIn);
static REL::Relocation<tNiMatrixToNiQuaternion> NiMatrixToNiQuaternion{ RELOCATION_ID(69467, 70844) };  // C6E2D0, C967D0

typedef RE::NiObject* (*tNiObject_Clone)(RE::NiObject* a_object, const RE::NiCloningProcess& a_cloningProcess);
static REL::Relocation<tNiObject_Clone> NiObject_Clone{ RELOCATION_ID(68836, 70188) };  // C52820, C79F30

typedef void (*tCleanupCloneMap)(RE::BSTHashMap<RE::NiObject*, RE::NiObject*>& a_cloneMap);
static REL::Relocation<tCleanupCloneMap> CleanupCloneMap{ RELOCATION_ID(15231, 15395) };  // 1B8AD0, 1C4460

typedef void (*tCleanupProcessMap)(RE::BSTHashMap<RE::NiObject*, bool>& a_cloneMap);
static REL::Relocation<tCleanupProcessMap> CleanupProcessMap{ RELOCATION_ID(15232, 15396) };  // 1B8B90, 1C4520

typedef float (*tTESObjectREFR_GetRelevantWaterHeight)(RE::TESObjectREFR* a_refr);
static REL::Relocation<tTESObjectREFR_GetRelevantWaterHeight> TESObjectREFR_GetRelevantWaterHeight{ RELOCATION_ID(19411, 19839) };  // 29B190, 2AD7F0

typedef void (*tbhkCharacterController_ToggleCharacterBumper)(RE::bhkCharacterController* a_controller, bool a_bEnable);
static REL::Relocation<tbhkCharacterController_ToggleCharacterBumper> bhkCharacterController_ToggleCharacterBumper{ RELOCATION_ID(76429, 78268) };  // DBF110, DFF930

typedef void (*tEnableCharacterBumperHandler_Handle)(void* a_enableCharacterBumperHandler, RE::Actor* a_actor);
static REL::Relocation<tEnableCharacterBumperHandler_Handle> EnableCharacterBumperHandler_Handle{ RELOCATION_ID(41805, 42886) };  // 723070, 74E1E0

typedef void (*tDisableCharacterBumperHandler_Handle)(void* a_disableCharacterBumperHandler, RE::Actor* a_actor);
static REL::Relocation<tDisableCharacterBumperHandler_Handle> DisableCharacterBumperHandler_Handle{ RELOCATION_ID(41806, 42887) };  // 723110, 74E280
