#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\NavigationRestrictions.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    // Load settings

    logger::info("Loaded settings");
    logger::info("");
}

void Settings::LoadForms() noexcept
{
    //hard coded forms for now. will change to ini or json later
    const char* plugin_name = "NavigationRestrictions.esp";
    const int MapID = 0x800;
    const int MapDamagedID = 0x801;
    const int MapIndestructibleID = 0x803;
    const int MapDestroyedID = 0x802;
    const int CompassID = 0x804;
    const int DurabilityID = 0x809;
    const int DamageValueID = 0x80A;

    auto dataHandler = RE::TESDataHandler::GetSingleton();

    map = dataHandler->LookupForm<RE::TESObjectMISC>(MapID, plugin_name);
    map_damaged = dataHandler->LookupForm<RE::TESObjectMISC>(MapDamagedID, plugin_name);
    map_indestructible = dataHandler->LookupForm<RE::TESObjectMISC>(MapIndestructibleID, plugin_name);
    map_destroyed = dataHandler->LookupForm<RE::TESObjectMISC>(MapDestroyedID, plugin_name);
    compass = dataHandler->LookupForm<RE::TESObjectMISC>(CompassID, plugin_name);
    durability = dataHandler->LookupForm<RE::TESGlobal>(DurabilityID, plugin_name);
    damageValue = dataHandler->LookupForm<RE::TESGlobal>(DamageValueID, plugin_name);
}
