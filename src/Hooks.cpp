#include "Hooks.h"
#include "Settings.h"

namespace Hooks
{
    void Install() noexcept
    {
        logger::info("{:*^30}", "HOOKS"sv);
        MainUpdate::Install();
        MapMenuEx::Install();
        ItemAdded::InstallAddItemHook();
        ItemAdded::InstallRemoveItemHook();
        ItemAdded::InstallPickupHook();
    }

    void MapMenuEx::Install()
    {
        REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_MapMenu[0]);
        func = vTable.write_vfunc(0x4, &OpenMap);
        logger::info("installed map open hook");
    }

    RE::UI_MESSAGE_RESULTS MapMenuEx::OpenMap(RE::UIMessage& a_message)
    {
        if (a_message.type == RE::UI_MESSAGE_TYPE::kShow && !Setting::Values::bypass_map_checks.GetValue())
        {
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            if (!shouldOpenMap(player)) {
                logger::debug("restrict map hook");
                showRestrictionMessage();
                return RE::UI_MESSAGE_RESULTS::kIgnore;
            }
            else {
                auto curr_map = GetCurrentMapItem(player);
                if (curr_map; curr_map != Setting::Forms::map_indestructible || !hasIndestructibleMap(player)) {
                    damage_map_item(1);
                    logger::debug("damaged {} remaining durability is {}", curr_map->GetName(), total_durability_value_all_maps - current_map_damage);
                }                
            }
        }
        return func(this, a_message);
    }

    bool MapMenuEx::hasAtLeastOneMapItem(RE::PlayerCharacter* player)
    {
        logger::debug("has at least one map item {}", player->GetItemCount(Setting::Forms::map) || player->GetItemCount(Setting::Forms::map_damaged) || hasIndestructibleMap(player) ? "true" : "false");
        return player->GetItemCount(Setting::Forms::map) || player->GetItemCount(Setting::Forms::map_damaged) || hasIndestructibleMap(player);
    }

    bool MapMenuEx::shouldOpenMap(RE::PlayerCharacter* player)
    {
        logger::debug("should open map is {}", hasAtLeastOneMapItem(player) || Setting::Values::bypass_map_checks.GetValue() ? "true" : "false");
        return hasAtLeastOneMapItem(player) || Setting::Values::bypass_map_checks.GetValue();
    }

    void MapMenuEx::damage_map_item(uint16_t a_damage_amount)
    {
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
        auto map = GetCurrentMapItem(player);
        logger::debug("current map damage value is {}", current_map_damage);
        if (map && current_map_damage >= total_durability_value_all_maps || current_map_damage >= ItemAdded::map_durability_map.at(map)) {            
            destroy_map_item(map, player);
            current_map_damage = 0;
            logger::debug("reset current map damage");
            return;
        }
        current_map_damage = std::clamp(current_map_damage += a_damage_amount, (std::int16_t)0, total_durability_value_all_maps);
        
        return;        
    }

    void MapMenuEx::destroy_map_item(RE::TESObjectMISC* a_map_item, RE::PlayerCharacter* player)
    {
        player->RemoveItem(a_map_item, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
        if (a_map_item == Setting::Forms::map) {
            player->AddObjectToContainer(Setting::Forms::map_damaged, nullptr, 1, nullptr);
            //total_durability_value_all_maps += ItemAdded::map_durability_map.at(Setting::Forms::map_damaged);
            total_durability_value_all_maps = std::clamp(total_durability_value_all_maps -= ItemAdded::map_durability_map.at(a_map_item), (std::int16_t)0, ItemAdded::map_durability_map.at(a_map_item));
            logger::debug("new durability is {}", total_durability_value_all_maps);
            return;
        }
        if (a_map_item == Setting::Forms::map_damaged) {
            player->AddObjectToContainer(Setting::Forms::map_destroyed, nullptr, 1, nullptr);
            //total_durability_value_all_maps += ItemAdded::map_durability_map.at(Setting::Forms::map_destroyed);
            total_durability_value_all_maps = std::clamp((total_durability_value_all_maps -= ItemAdded::map_durability_map.at(a_map_item)), (std::int16_t)0, ItemAdded::map_durability_map.at(a_map_item));
            logger::debug("new durability is {}", total_durability_value_all_maps);
            return;
        }
    }

    RE::TESObjectMISC* MapMenuEx::GetCurrentMapItem(RE::PlayerCharacter* player)
    {
        if (player->GetItemCount(Setting::Forms::map) > 0) {
            return Setting::Forms::map;
        }
        if (player->GetItemCount(Setting::Forms::map_damaged) > 0) {
            return Setting::Forms::map_damaged;
        }
        if (player->GetItemCount(Setting::Forms::map_indestructible) > 0) {
            return Setting::Forms::map_indestructible;
        }
        return nullptr;
    }

    void MapMenuEx::showRestrictionMessage(){
        RE::DebugNotification(Setting::Values::restriction_message.GetValue().c_str());
        return;
    }

    void ItemAdded::LowerDurability(std::unordered_map<RE::TESObjectMISC*, std::int16_t> a_mapPairs, std::int16_t a_total_durability, RE::TESObjectMISC* used_map)
    {
        auto result = MapMenuEx::total_durability_value_all_maps;
        MapMenuEx::total_durability_value_all_maps = std::clamp((result -= a_mapPairs.at(used_map)), (std::int16_t)0, MapMenuEx::total_durability_value_all_maps);
    }

    bool MapMenuEx::hasIndestructibleMap(RE::PlayerCharacter* player)
    {
        bool result = false;
        if (player->GetItemCount(Setting::Forms::map_indestructible) > 0) {
            result = true;
        }
        auto inv = player->GetInventory();
        for (auto& item : player->GetInventory()) {
            if (item.first->HasKeywordByEditorID("MapIndestructible")) {
                result = true;
            }
        }
        return result;
    }

    void ItemAdded::PopulateMap()
    {
        ItemAdded::map_durability_map.try_emplace(Setting::Forms::map, Setting::Values::durability_map_normal.GetValue());
        ItemAdded::map_durability_map.try_emplace(Setting::Forms::map_damaged, Setting::Values::durability_map_damaged.GetValue());
        ItemAdded::map_durability_map.try_emplace(Setting::Forms::map_destroyed, 0);

        logger::debug("populated map, entries are: 1 with a value of {} \n 2 with a value of {} and \n 3 with a value of {}", map_durability_map.at(Setting::Forms::map), map_durability_map.at(Setting::Forms::map_damaged), map_durability_map.at(Setting::Forms::map_destroyed));
    }

    void ItemAdded::InstallAddItemHook()
    {
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
        _AddObjectToContainer = PlayerCharacterVtbl.write_vfunc(0x5A, OnItemAdded);
        logger::info("Installed OnItemAdded Hook");
    }

    void ItemAdded::InstallRemoveItemHook()
    {
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
        _RemoveItem = PlayerCharacterVtbl.write_vfunc(0x56, OnItemRemoved);
        logger::info("Installed OnItemRemoved Hook");
    }

    void ItemAdded::InstallPickupHook()
    {
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
        _PickUpObject = PlayerCharacterVtbl.write_vfunc(0xCC, PickUpObject);
        logger::info("Installed PickUpObject Hook");
    }

    void ItemAdded::UpdateMap()
    {
        ItemAdded::map_durability_map.insert_or_assign(Setting::Forms::map, Setting::Values::durability_map_normal.GetValue());
        ItemAdded::map_durability_map.insert_or_assign(Setting::Forms::map_damaged, Setting::Values::durability_map_damaged.GetValue());
    }

    void ItemAdded::PickUpObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound)
    {
        {
            _PickUpObject(a_this, a_object, a_count, a_arg3, a_playSound);
            if (a_object->GetBaseObject() == Setting::Forms::map->As<RE::TESBoundObject>() || a_object->GetBaseObject() == Setting::Forms::map_damaged->As<RE::TESBoundObject>() || a_object->GetBaseObject() == Setting::Forms::map_destroyed->As<RE::TESBoundObject>()) {
                if (a_count > 1) {
                    for (int i = 0; i < a_count; i++) {
                        AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->GetBaseObject()->As<RE::TESObjectMISC>());
                        logger::debug("{}.) added durability", i);
                    }
                }
                else {
                    AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->GetBaseObject()->As<RE::TESObjectMISC>());
                    logger::debug("added durability");
                }
                
            }
            if (a_object->GetBaseObject() == Setting::Forms::compass->As<RE::TESBoundObject>() || a_object->GetBaseObject()->HasKeywordByEditorID("CompassIndestructible")) {
                MainUpdate::show_compass_now = true;
            }
            logger::debug("item {} picked up", a_object->GetName());

        }
    }

    void ItemAdded::OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr)
    {
        _AddObjectToContainer(a_this, a_object, a_extraList, a_count, a_fromRefr);
        if (a_object == Setting::Forms::map->As<RE::TESBoundObject>() || a_object == Setting::Forms::map_damaged->As<RE::TESBoundObject>() || a_object == Setting::Forms::map_destroyed->As<RE::TESBoundObject>()) {
            if (a_count > 1) {
                for (int i = 0; i < a_count; i++) {
                    AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->As<RE::TESObjectMISC>());
                    logger::debug("{}.) added durability", i);
                }
            }
            else {
                AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->As<RE::TESObjectMISC>());
                logger::debug("added durability");
            }
        }
        if (a_object == Setting::Forms::compass || a_object->HasKeywordByEditorID("CompassIndestructible")) {
            MainUpdate::show_compass_now = true;
        }
        logger::debug("item {} added", a_object->GetName());
        
    }

    RE::ObjectRefHandle ItemAdded::OnItemRemoved(RE::Actor* a_this, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate)
    {
        
        if (a_item == Setting::Forms::map || a_item == Setting::Forms::map_damaged || a_item == Setting::Forms::map_destroyed) {
            if (a_count > 1) {
                for (int i = 0; i < a_count; i++) {
                    LowerDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_item->As<RE::TESObjectMISC>());
                    logger::debug("lowered durability");
                }
            }
            else {
                LowerDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_item->As<RE::TESObjectMISC>());
                logger::debug("lowered durability");
            }
        }
        auto handle = _RemoveItem(a_this, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
        auto player = RE::PlayerCharacter::GetSingleton();
        logger::debug("before compass item check");
        if (a_item == Setting::Forms::compass || a_item->HasKeywordByEditorID("CompassIndestructible")) {  
            logger::info("is compass item");
            if (player->GetItemCount(Setting::Forms::compass) <= 1) {
                logger::debug("removed {}, player has {} left", a_item->GetName(), player->GetItemCount(Setting::Forms::compass));
                MainUpdate::show_compass_now = false;
            }
            if (!MainUpdate::shouldShowCompass(player)) {
                MainUpdate::show_compass_now = false;
            }
        }
        logger::debug("item {} removed", a_item->GetName());
        
        
        return handle;
    }

    void ItemAdded::AddDurability(std::unordered_map<RE::TESObjectMISC*, std::int16_t> a_mapPairs, std::int16_t a_total_durability, RE::TESObjectMISC* used_map)
    {
        MapMenuEx::total_durability_value_all_maps += a_mapPairs.at(used_map);
        logger::debug("new durability after add dur function is {}", MapMenuEx::total_durability_value_all_maps);
    }

    void MainUpdate::PlayerUpdate(RE::PlayerCharacter* p, float a_delta)
    {
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
        RE::Calendar* cal = RE::Calendar::GetSingleton();

        if (!RE::UI::GetSingleton()->IsMenuOpen(RE::MainMenu::MENU_NAME))
        {
            if (Setting::Values::bypass_compass_checks.GetValue()) {
                if (!compass_visible) {
                    ShowCompass();
                    compass_visible = true;
                    logger::debug("bypassed compass check");                    
                }
                return func(p, a_delta);                
            }

            CompVisUpdate();

            if (Setting::Values::compass_duration_days.GetValue() > 0.0 && compass_visible && Setting::Values::enable_compass_damage.GetValue()) {
                if (cal->GetHoursPassed() >= (passed_time + 1.0)) {
                    if (damageCompass(std::roundf(cal->GetHoursPassed() - passed_time))) {
                        
                    }
                    else {
                        passed_time = cal->GetHoursPassed();
                        destroy = false;
                        logger::debug("stored new time, it is {}", passed_time);
                    }
                }
            }

            if (init) {                
                if(!show_compass_now) {
                    HideCompass();
                    init = false;
                    logger::debug("hide compass on init");
                }
                else if (show_compass_now) {
                    ShowCompass();
                    init = false;
                    logger::debug("hide compass on init");
                }
            }
            return func(p, a_delta);
        }
        return func(p, a_delta);
    }

    void MainUpdate::Install()
    {
        REL::Relocation<std::uintptr_t> PlayerVTBL{ RE::VTABLE_PlayerCharacter[0] };
        func = PlayerVTBL.write_vfunc(0xAD, PlayerUpdate);
        logger::info("hook:Player Update");
    }

    bool MainUpdate::useSkillsOfTheWild()
    {
        bool sotw_show_comp = true;
        if (Setting::Values::skills_of_the_wild_active) {
            if (Setting::Forms::skills_of_the_wild_perk != nullptr && Setting::Forms::skills_of_the_wild_perk->FORMTYPE == RE::FormType::Global) {
                if (Setting::Forms::skills_of_the_wild_perk->value != 0.0f || Setting::Forms::sotw_cheat_global->value != 0.0f) {
                    sotw_show_comp = true;
                }
                else {
                    sotw_show_comp = false;
                }
            }
            
        }
        return sotw_show_comp;
    }

    void MainUpdate::CompVisUpdate()
    {
        if (show_compass_now != compass_visible) {
            if (show_compass_now) {
                ShowCompass();
            } else {
                HideCompass();
            }
            compass_visible = show_compass_now;
        }
    }

    void MainUpdate::PrintCompass() {
        if (Setting::Values::show_compass_break.GetValue()) {
            RE::DebugNotification(Setting::Values::compass_break_message.GetValue().c_str());
        }        
        return;
    }

    bool MainUpdate::ShowCompass()
    {
        return ShowHUDElement("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha");
    }

    bool MainUpdate::HideCompass()
    {
        return HideHudElement("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha");
    }

    bool MainUpdate::HideHudElement(const char* a_pathToVar) {
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            uiMovie->SetVariable(a_pathToVar, 0.0);
            compass_visible = uiMovie->GetVariableDouble(a_pathToVar);
            logger::debug("compass visible in hide hud element is {}", compass_visible ? "true" : "false");
        }
        return compass_visible;
    }

    bool MainUpdate::ShowHUDElement(const char* a_pathToVar)
    {
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            uiMovie->SetVariable(a_pathToVar, 100.0);
            compass_visible = uiMovie->GetVariableDouble(a_pathToVar);  
            logger::debug("compass visible in show hud element is {}", compass_visible ? "true" : "false");
        }
        return compass_visible;
    }

    bool MainUpdate::damageCompass(std::int16_t a_amount)
    {
        compass_damage_val += a_amount;
        logger::debug("new damage value is {}", compass_damage_val);
        if (compass_damage_val >= (Setting::Values::compass_duration_days.GetValue() * 24.0)) {
            destroy = true;
            compass_damage_val = 0.0;
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            if (player->GetItemCount(Setting::Forms::compass) > 0) {
                player->RemoveItem(Setting::Forms::compass, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                PrintCompass();
                if (!shouldShowCompass(player)) {
                    show_compass_now = false;
                }
            }            
            return true;
        }
        return false;
    }

    bool MainUpdate::HasCompassItem(RE::PlayerCharacter* player)
    {
        i32 count = player->GetItemCount(Setting::Forms::compass);
        if (count <= 0) {
            compass_damage_val = 0;
        }
        return count;
    }

    bool MainUpdate::shouldShowCompass(RE::PlayerCharacter* player)
    {
        logger::debug("start shouldShowCompass");
        show_compass_now = false;
        if (HasCompassItem(player)) {
            if (useSkillsOfTheWild()) {
                show_compass_now = true;
            }   
        }
        if (Setting::Values::bypass_compass_checks.GetValue()) {
            show_compass_now = true;
        }
        for (auto& item : player->GetInventory()) {
            if (item.first->HasKeywordByEditorID("CompassIndestructible")) {
                destroy = false;
                if (useSkillsOfTheWild()) {
                    show_compass_now = true;
                }                
            }
        }
        logger::debug("end shouldShowCompass, result is {}", show_compass_now ? "true" : "false");
        return show_compass_now;
    }
} // namespace Hooks
