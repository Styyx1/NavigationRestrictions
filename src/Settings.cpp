#include "Settings.h"

void Settings::LoadSettings() noexcept
{
    logger::info("Loading settings");

    CSimpleIniA ini;

    ini.SetUnicode();
    ini.LoadFile(R"(.\Data\SKSE\Plugins\NavigationRestrictions.ini)");

    debug_logging = ini.GetBoolValue("Log", "Debug");
    restrictionMSG = ini.GetValue("Settings", "MessageText", "");
    compassBreakMSG = ini.GetValue("Settings", "CompassBreakMessage", "");
    //compassDestryDays = static_cast<float>(ini.GetDoubleValue("Settings", "fCompassDestroyDays", 3.00000));

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    // Load settings

    logger::info("Loaded settings");
    logger::info("");
}

inline static void PrintMap(RE::TESObjectMISC* item)
{
    logger::debug("lookup successful, item name is {}", item->GetName());
}

inline static void LogGlobal(RE::TESGlobal* global)
{
    logger::debug("lookup successfull global {} with the value {} found", global->GetFormEditorID(), global->value);

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
    const int MapBypass = 0x810;
    const int CompassBypass = 0x811;
    const int CompassDurationDays = 0x812;

    auto dataHandler = RE::TESDataHandler::GetSingleton();

    map = dataHandler->LookupForm<RE::TESObjectMISC>(MapID, plugin_name);
    PrintMap(map);
    map_damaged = dataHandler->LookupForm<RE::TESObjectMISC>(MapDamagedID, plugin_name);
    PrintMap(map_damaged);
    map_indestructible = dataHandler->LookupForm<RE::TESObjectMISC>(MapIndestructibleID, plugin_name);
    PrintMap(map_indestructible);
    map_destroyed = dataHandler->LookupForm<RE::TESObjectMISC>(MapDestroyedID, plugin_name);
    PrintMap(map_destroyed);
    compass = dataHandler->LookupForm<RE::TESObjectMISC>(CompassID, plugin_name);
    PrintMap(compass);
    durability = dataHandler->LookupForm<RE::TESGlobal>(DurabilityID, plugin_name);
    LogGlobal(durability);
    damageValue = dataHandler->LookupForm<RE::TESGlobal>(DamageValueID, plugin_name);
    LogGlobal(damageValue);
    bypassMapCheck = dataHandler->LookupForm<RE::TESGlobal>(MapBypass, plugin_name);
    LogGlobal(bypassMapCheck);
    bypassCompassCheck = dataHandler->LookupForm<RE::TESGlobal>(CompassBypass, plugin_name);
    LogGlobal(bypassCompassCheck);
    compassDurationDays = dataHandler->LookupForm<RE::TESGlobal>(CompassDurationDays, plugin_name);
    LogGlobal(compassDurationDays);


}
