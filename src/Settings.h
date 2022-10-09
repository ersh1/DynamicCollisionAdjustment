#pragma once
#include "TrueHUDAPI.h"

enum class DebugDrawMode : std::uint32_t
{
	kNone = 0,
	kAdjusted = 1,
	kAll = 2
};

struct Settings
{
	static void Initialize();
	static void ReadSettings();
	static void OnPostLoadGame();
	static void UpdateGlobals();

	static void RequestAPIs();

	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);
	static void ReadUInt32Setting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting);

	// General
	static inline bool bEnableActorScaleFix = true;
	static inline bool bEnableStateAdjustments = true;

	static inline float fSneakControllerShapeHeightMultiplier = 0.75f;
	static inline float fSwimmingControllerShapeHeightMultiplier = 0.75f;
	static inline float fSwimmingControllerShapeRadiusMultiplier = 2.f;

	// Debug
	static inline DebugDrawMode uDisplayDebugShapes = DebugDrawMode::kNone;
	static inline bool bDisplayCharacterBumper = false;

	// Non-MCM
	static inline TRUEHUD_API::IVTrueHUD4* g_trueHUD = nullptr;
	static inline RE::TESGlobal* glob_trueHUD = nullptr;
	static inline RE::BGSKeyword* kywd_NPC = nullptr;
};
