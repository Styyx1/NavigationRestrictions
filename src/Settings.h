#pragma once

namespace Setting {
    namespace Values {

        static float stored_time{};
        static bool skills_of_the_wild_active{};
        static uint16_t map_durability_total{ 50 };

        static REX::INI::Bool debug_logging{ "DebugLogging", "bEnableDebugLog", false };
        static REX::INI::Bool enable_compass_damage{ "Settings", "bEnableCompassDamage", true };
        static REX::INI::Bool bypass_map_checks{ "Settings", "bBypassMapCheck", false };
        static REX::INI::Bool bypass_compass_checks{ "Settings", "bBypassCompassCheck", false  };
        static REX::INI::Bool show_compass_break{ "Settings", "bShowCompassNotif",true };

        static REX::INI::I32 durability_map_normal{ "Settings", "iMapDurability", (i32)20 };
        static REX::INI::I32 durability_map_damaged{ "Settings", "iDamagedMapDurability", (i32)10 };

        static REX::INI::F32 compass_duration_days{ "Settings", "fCompassDurationDays", 3.0f };

        static REX::INI::Str restriction_message{ "Texts", "sMessageText", (std::string)"You need a map to see your location!"};
        static REX::INI::Str compass_break_message{ "Texts", "sCompassBreakMessage", (std::string)"Your compass can not guide you anymore..." };
        
        static void Update()
        {
            logger::info("Loading settings...");
            const auto ini = REX::INI::SettingStore::GetSingleton();
            ini->Init("Data/MCM/Config/NavigationRestrictions/settings.ini", "Data/MCM/Settings/NavigationRestrictions.ini");
            ini->Load();

            if (debug_logging.GetValue()) {
                spdlog::set_level(spdlog::level::debug);
                logger::debug("Debug logging enabled");
            }

            logger::info("...Settings loaded");
        }
    }
    namespace Functions {

        static const char* sotw_mod{ "SkillsOfTheWild.esp" };

        static bool isSkillOfTheWildActive() 
        {
        
            auto dh = RE::TESDataHandler::GetSingleton();

            if (auto file = dh->LookupModByName(sotw_mod); file && file->compileIndex != 0xFF) {
                logger::info("Skills of the wild is active");
                Values::skills_of_the_wild_active = true;
            }
            return Values::skills_of_the_wild_active;
        };
    }

    struct Forms {

        inline static RE::TESObjectMISC* map;
        inline static RE::TESObjectMISC* map_damaged;
        inline static RE::TESObjectMISC* map_indestructible;
        inline static RE::TESObjectMISC* map_destroyed;
        inline static RE::TESObjectMISC* compass;
        inline static RE::TESObjectMISC* compass_indestructible;
        inline static RE::TESGlobal* skills_of_the_wild_perk;
        inline static RE::TESGlobal* sotw_cheat_global;

        static void PrintMap(RE::TESObjectMISC* item)
        {
            logger::debug("lookup successful, item name is {}", item->GetName());
        }

        static void LoadForms() noexcept
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

            if (Functions::isSkillOfTheWildActive()) {
                skills_of_the_wild_perk = dataHandler->LookupForm(sotw_compass_global_non_cheat_id, Functions::sotw_mod)->As<RE::TESGlobal>();
                sotw_cheat_global = dataHandler->LookupForm(sotw_compass_global_cheat_id, Functions::sotw_mod)->As<RE::TESGlobal>();
            }
        };
    };

}