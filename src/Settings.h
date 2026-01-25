#pragma once
#include "mod-data.h"

namespace Config {
    using namespace MOD;
    struct Settings : REX::Singleton<Settings> {

        static inline REX::TOML::Bool toggle_damage_compass{ COMP_SEC, "bToggleCompassDamage", true };
        static inline REX::TOML::Bool toggle_compass_check{ COMP_SEC, "bToggleCompassCheck", true };
        static inline REX::TOML::Bool toggle_compass_notification{ COMP_SEC, "bToggleCompassNotification", true };
        static inline REX::TOML::I32 compass_durability{ COMP_SEC, "iCompassDurability", 30 };
        static inline REX::TOML::F32 compass_damage_tick_time{ COMP_SEC, "fCompassDamageTime", 30.0f };
        static inline REX::TOML::Str compass_break_message{ COMP_SEC, "sCompassBreakNotification", std::string("Your compass can not guide you anymore...") };

        static inline REX::TOML::Bool toggle_damage_map{ MAP_SEC, "bToggleMapDamage", true };
        static inline REX::TOML::Bool toggle_map_check{ MAP_SEC, "bToggleMapCheck", true };
        static inline REX::TOML::I32 map_good_durabilty{ MAP_SEC, "iMapGoodDurability", 20 };
        static inline REX::TOML::I32 map_damaged_durabilty{ MAP_SEC, "iMapDamagedDurability", 10 };
        static inline REX::TOML::Str map_restrict_notification{ MAP_SEC, "sMapRestrictNotification", std::string("You need a map in your inventory") };

        

        void UpdateSettings(bool a_save) {
            const auto toml = REX::TOML::SettingStore::GetSingleton();
            toml->Init(TOML_DEF.data(), TOML_CUS.data());
            if (!a_save)
                toml->Load();
            else
                toml->Save();
        }
    };

    struct Forms : REX::Singleton<Forms> {

        static inline RE::TESObjectMISC* map_new{};
        static inline RE::TESObjectMISC* map_damaged{};
        static inline RE::TESObjectMISC* map_indestructible{};
        static inline RE::TESObjectMISC* map_destroyed{};
        static inline RE::TESObjectMISC* compass_new{};
        static inline RE::TESObjectMISC* compass_indestructible{};
        static inline RE::TESGlobal* sotw_perk{};
        static inline RE::TESGlobal* sotw_cheat{};

        //set this on form lookup once and use it exclusively for sotw integration
        //previous version of the mod had issues with an old version of sotw, this hopefully fixes it
        static inline bool is_sotw_active{ false };

        RE::TESObjectMISC* LoadMiscItem(RE::TESDataHandler* a_dataHandler, RE::FormID a_id, std::string_view a_modFile) {
            auto* form = a_dataHandler->LookupForm<RE::TESObjectMISC>(a_id, a_modFile);
            if (form)
                REX::DEBUG("Loaded {}", form->GetName());
            else
                REX::ERROR("can not load {:8x}", a_id);
            return form;
        }
        RE::TESGlobal* LoadGlobal(RE::TESDataHandler* a_dataHandler, RE::FormID a_id, std::string_view a_modFile) {
            auto* form = a_dataHandler->LookupForm<RE::TESGlobal>(a_id, a_modFile);
            if(form)
                REX::DEBUG("Loaded {}", form->GetFormEditorID());
            else
                REX::ERROR("can not load {:8x}", a_id);
            return form;
        }

        void LoadForms() {

            const auto dh = RE::TESDataHandler::GetSingleton();

            if (!MiscUtil::IsModLoaded(MOD_FILE)) {
                REX::FAIL("Required plugin '{}' is missing.\n"
                    "{} cannot function without it.", MOD_FILE, MOD_NAME);
            }
            map_new = LoadMiscItem(dh, MAP_GOOD_ID, MOD_FILE);
            map_damaged = LoadMiscItem(dh, MAP_DAMAGED_ID, MOD_FILE);
            map_indestructible = LoadMiscItem(dh, MAP_INDESTRUCTIBLE_ID, MOD_FILE);
            map_destroyed = LoadMiscItem(dh, MAP_DESTROYED_ID, MOD_FILE);
            compass_new = LoadMiscItem(dh, COMP_GOOD_ID, MOD_FILE);
            compass_indestructible = LoadMiscItem(dh, COMP_INDESTRUCTIBLE_ID, MOD_FILE);

            if (MiscUtil::IsModLoaded(SOTW_FILE)) {
                sotw_cheat = LoadGlobal(dh, SOTW_COMP_CHEAT_GLOBAL_ID, SOTW_FILE);
                sotw_perk = LoadGlobal(dh, SOTW_COMP_NON_CHEAT_GLOBAL_ID, SOTW_FILE);
                if ( sotw_cheat && sotw_perk ) {
                    is_sotw_active = true;
                }                
            }
            else {
                REX::INFO("{} not found, integration is not active", SOTW_FILE);
            }
        }
    };
}