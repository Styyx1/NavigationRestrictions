#include "Settings.h"

void Settings::LoadSettings()
{
    logger::info("Loading settings");
    CSimpleIniA ini;
    constexpr auto defaultSettingsPath = L"Data/MCM/Config/NavigationRestrictions/settings.ini";
    constexpr auto mcmPath = L"Data/MCM/Settings/NavigationRestrictions.ini";

    UpdateSettings(ini, defaultSettingsPath);
    UpdateSettings(ini, mcmPath);
}

void Settings::UpdateSettings(CSimpleIniA &ini , std::filesystem::path path)
{     
    ini.SetUnicode();
    ini.LoadFile(path.string().c_str());
    auto file = path.string();
    logger::info("loading ini file from {}", file);

    debug_logging = ini.GetBoolValue("DebugLogging", "bEnableDebugLog", false);
    restrictionMSG = ini.GetValue("Texts", "sMessageText", "You need a map to see your location!");
    compassBreakMSG = ini.GetValue("Texts", "sCompassBreakMessage", "Your compass can not guide you anymore...");
    showCompassBreak = ini.GetBoolValue("Settings", "bShowCompassNotif", true);
    durability_map_damaged = ini.GetLongValue("Settings", "iDamagedMapDurability", (std::int16_t)10);
    durability_map_normal = ini.GetLongValue("Settings", "iMapDurability", (std::int16_t)20);
    bypass_compass_checks = ini.GetBoolValue("Settings", "bBypassCompassCheck", false);
    bypass_map_checks = ini.GetBoolValue("Settings", "bBypassMapCheck", false);
    enable_compass_damage = ini.GetBoolValue("Settings", "bEnableCompassDamage", true);
    compass_duration_days = ini.GetDoubleValue("Settings", "fCompassDurationDays", 3.0f);

    if (debug_logging) {
        spdlog::set_level(spdlog::level::debug);
        logger::debug("Debug logging enabled");
    }

    logger::info("Loaded settings");
    logger::info("");
}

bool Settings::isSkillOfTheWildActive()
{
    auto dh = RE::TESDataHandler::GetSingleton();

    if (auto file = dh->LookupModByName(sotw_mod); file && file->compileIndex != 0xFF) {
        logger::info("Skills of the wild is active");
        skills_of_the_wild_active = true;
    }

    return skills_of_the_wild_active;
}

inline static void PrintMap(RE::TESObjectMISC* item)
{
    logger::debug("lookup successful, item name is {}", item->GetName());
}

void Settings::LoadForms() noexcept
{
    const char* plugin_name = "NavigationRestrictions.esp";
    const int MapID = 0x800;
    const int MapDamagedID = 0x801;
    const int MapIndestructibleID = 0x803;
    const int MapDestroyedID = 0x802;
    const int CompassID = 0x804;
    const int CompassIndestructibleID = 0x81C;
    const int sotw_compass_global_cheat_id = 0x863;
    const int sotw_compass_global_non_cheat_id = 0x958;

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
    compass_indestructible = dataHandler->LookupForm<RE::TESObjectMISC>(CompassIndestructibleID, plugin_name);
    PrintMap(compass_indestructible);

    if (isSkillOfTheWildActive()) {
        skills_of_the_wild_perk = dataHandler->LookupForm(sotw_compass_global_non_cheat_id, sotw_mod)->As<RE::TESGlobal>();
        sotw_cheat_global = dataHandler->LookupForm(sotw_compass_global_cheat_id, sotw_mod)->As<RE::TESGlobal>();
    }

}
