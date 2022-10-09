#include "Settings.h"
#include <Simpleini.h>

void Settings::Initialize()
{
	logger::info("Initializing...");

	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		kywd_NPC = dataHandler->LookupForm<RE::BGSKeyword>(0x13794, "Skyrim.esm");
		glob_trueHUD = dataHandler->LookupForm<RE::TESGlobal>(0x801, "DynamicCollisionAdjustment.esl");
	}

	logger::info("...success");
}

void Settings::ReadSettings()
{
	constexpr auto path = L"Data/MCM/Settings/DynamicCollisionAdjustment.ini";

	logger::info("Reading MCM .ini...");

	CSimpleIniA mcm;
	mcm.SetUnicode();

	mcm.LoadFile(path);

	// General
	ReadBoolSetting(mcm, "General", "bEnableActorScaleFix", bEnableActorScaleFix);
	ReadBoolSetting(mcm, "General", "bEnableStateAdjustments", bEnableStateAdjustments);
	
	ReadFloatSetting(mcm, "General", "fSneakControllerCapsuleHeightMultiplier", fSneakControllerShapeHeightMultiplier);
	ReadFloatSetting(mcm, "General", "fSwimmingControllerShapeHeightMultiplier", fSwimmingControllerShapeHeightMultiplier);
	ReadFloatSetting(mcm, "General", "fSwimmingControllerShapeRadiusMultiplier", fSwimmingControllerShapeRadiusMultiplier);

	// Debug
	ReadUInt32Setting(mcm, "Debug", "uDisplayDebugShapes", (uint32_t&)uDisplayDebugShapes);
	ReadBoolSetting(mcm, "Debug", "bDisplayCharacterBumper", bDisplayCharacterBumper);

	logger::info("...success");
}

void Settings::OnPostLoadGame()
{
	RequestAPIs();
	UpdateGlobals();
}

void Settings::UpdateGlobals()
{
	if (glob_trueHUD) {
		glob_trueHUD->value = g_trueHUD != nullptr ? 1.f : 0.f;
	}
}

void Settings::RequestAPIs()
{
	if (!g_trueHUD) {
		Settings::g_trueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD4*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V4));
	}
}

void Settings::ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
	}
}

void Settings::ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		a_setting = static_cast<float>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
	}
}

void Settings::ReadUInt32Setting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		a_setting = static_cast<uint32_t>(a_ini.GetLongValue(a_sectionName, a_settingName));
	}
}
