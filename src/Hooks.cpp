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
        if (a_message.type == RE::UI_MESSAGE_TYPE::kShow && !Settings::bypass_map_checks)
        {
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            if (!shouldOpenMap(player)) {
                logger::info("restrict map hook");
                showRestrictionMessage();
                return RE::UI_MESSAGE_RESULTS::kIgnore;
            }
            else {
                auto curr_map = GetCurrentMapItem(player);
                if (curr_map && curr_map != Settings::map_indestructible) {
                    damage_map_item(1);
                    logger::debug("damaged {} remaining durability is {}", curr_map->GetName(), total_durability_value_all_maps - current_map_damage);
                }                
            }
        }
        return func(this, a_message);
    }

    bool MapMenuEx::hasAtLeastOneMapItem(RE::PlayerCharacter* player)
    {
        logger::info("has at least one map item {}", player->GetItemCount(Settings::map) || player->GetItemCount(Settings::map_damaged) || player->GetItemCount(Settings::map_indestructible) ? "true" : "false");
        return player->GetItemCount(Settings::map) || player->GetItemCount(Settings::map_damaged) || player->GetItemCount(Settings::map_indestructible);
    }

    bool MapMenuEx::shouldOpenMap(RE::PlayerCharacter* player)
    {
        logger::info("should open map is {}", hasAtLeastOneMapItem(player) || Settings::bypass_map_checks != 0 ? "true" : "false");
        return hasAtLeastOneMapItem(player) || Settings::bypass_map_checks;
    }

    void MapMenuEx::damage_map_item(uint16_t a_damage_amount)
    {
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
        auto map = GetCurrentMapItem(player);
        logger::info("current map damage value is {}", current_map_damage);
        if (map && current_map_damage >= total_durability_value_all_maps || current_map_damage >= ItemAdded::map_durability_map.at(map)) {            
            destroy_map_item(map, player);
            current_map_damage = 0;
            logger::info("reset current map damage");
            return;
        }
        current_map_damage = std::clamp(current_map_damage += a_damage_amount, (std::int16_t)0, total_durability_value_all_maps);
        
        return;        
    }

    void MapMenuEx::destroy_map_item(RE::TESObjectMISC* a_map_item, RE::PlayerCharacter* player)
    {
        player->RemoveItem(a_map_item, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
        if (a_map_item == Settings::map) {
            player->AddObjectToContainer(Settings::map_damaged, nullptr, 1, nullptr);
            //total_durability_value_all_maps += ItemAdded::map_durability_map.at(Settings::map_damaged);
            total_durability_value_all_maps = std::clamp(total_durability_value_all_maps -= ItemAdded::map_durability_map.at(a_map_item), (std::int16_t)0, ItemAdded::map_durability_map.at(a_map_item));
            logger::info("new durability is {}", total_durability_value_all_maps);
            return;
        }
        if (a_map_item == Settings::map_damaged) {
            player->AddObjectToContainer(Settings::map_destroyed, nullptr, 1, nullptr);
            //total_durability_value_all_maps += ItemAdded::map_durability_map.at(Settings::map_destroyed);
            total_durability_value_all_maps = std::clamp((total_durability_value_all_maps -= ItemAdded::map_durability_map.at(a_map_item)), (std::int16_t)0, ItemAdded::map_durability_map.at(a_map_item));
            logger::info("new durability is {}", total_durability_value_all_maps);
            return;
        }
    }

    RE::TESObjectMISC* MapMenuEx::GetCurrentMapItem(RE::PlayerCharacter* player)
    {
        if (player->GetItemCount(Settings::map) > 0) {
            return Settings::map;
        }
        if (player->GetItemCount(Settings::map_damaged) > 0) {
            return Settings::map_damaged;
        }
        if (player->GetItemCount(Settings::map_indestructible) > 0) {
            return Settings::map_indestructible;
        }
        return nullptr;
    }

    bool MapMenuEx::CurrentMapItemIsValid(RE::PlayerCharacter* player)
    {
        if (GetCurrentMapItem(player)) {
            return true;
        }
        return false;
    }

    void MapMenuEx::showRestrictionMessage(){
        RE::DebugNotification(Settings::restrictionMSG.c_str());
        return;
    }

    void ItemAdded::LowerDurability(std::unordered_map<RE::TESObjectMISC*, std::int16_t> a_mapPairs, std::int16_t a_total_durability, RE::TESObjectMISC* used_map)
    {
        auto result = MapMenuEx::total_durability_value_all_maps;
        MapMenuEx::total_durability_value_all_maps = std::clamp((result -= a_mapPairs.at(used_map)), (std::int16_t)0, MapMenuEx::total_durability_value_all_maps);
    }

    void ItemAdded::InstallAddItemHook()
    {
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
        _AddObjectToContainer = PlayerCharacterVtbl.write_vfunc(0x5A, OnItemAdded);
    }

    void ItemAdded::InstallRemoveItemHook()
    {
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
        _RemoveItem = PlayerCharacterVtbl.write_vfunc(0x56, OnItemRemoved);
    }

    void ItemAdded::InstallPickupHook()
    {
        REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
        _PickUpObject = PlayerCharacterVtbl.write_vfunc(0xCC, PickUpObject);
    }

    void ItemAdded::PopulateMap()
    {
        ItemAdded::map_durability_map.try_emplace(Settings::map, Settings::durability_map_normal);
        ItemAdded::map_durability_map.try_emplace(Settings::map_damaged, Settings::durability_map_damaged);
        ItemAdded::map_durability_map.try_emplace(Settings::map_destroyed, 0);

        logger::info("populated map, entries are: 1 with a value of {} \n 2 with a value of {} and \n 3 with a value of {}", map_durability_map.at(Settings::map), map_durability_map.at(Settings::map_damaged), map_durability_map.at(Settings::map_destroyed));
    }

    void ItemAdded::UpdateMap()
    {
        ItemAdded::map_durability_map.insert_or_assign(Settings::map, Settings::durability_map_normal);
        ItemAdded::map_durability_map.insert_or_assign(Settings::map_damaged, Settings::durability_map_damaged);
    }

    void ItemAdded::PickUpObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound)
    {

        {
            _PickUpObject(a_this, a_object, a_count, a_arg3, a_playSound);
            if (a_object->GetBaseObject() == Settings::map || a_object->GetBaseObject() == Settings::map_damaged || a_object->GetBaseObject() == Settings::map_destroyed) {
                if (a_count > 1) {
                    for (int i = 0; i < a_count; i++) {
                        AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->GetBaseObject()->As<RE::TESObjectMISC>());
                        logger::info("{}.) added durability", i);
                    }
                }
                else {
                    AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->GetBaseObject()->As<RE::TESObjectMISC>());
                    logger::info("added durability");
                }
                
            }  
            logger::info("item {} picked up", a_object->GetName());

        }
    }

    void ItemAdded::OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr)
    {
        _AddObjectToContainer(a_this, a_object, a_extraList, a_count, a_fromRefr);
        if (a_object == Settings::map->As<RE::TESBoundObject>() || a_object == Settings::map_damaged->As<RE::TESBoundObject>() || a_object == Settings::map_destroyed->As<RE::TESBoundObject>()) {
            if (a_count > 1) {
                for (int i = 0; i < a_count; i++) {
                    AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->As<RE::TESObjectMISC>());
                    logger::info("{}.) added durability", i);
                }
            }
            else {
                AddDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_object->As<RE::TESObjectMISC>());
                logger::info("added durability");
            }
        }
        logger::info("item {} added", a_object->GetName());
        
    }

    RE::ObjectRefHandle ItemAdded::OnItemRemoved(RE::Actor* a_this, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate)
    {
        
        if (a_item == Settings::map || a_item == Settings::map_damaged || a_item == Settings::map_destroyed) {
            if (a_count > 1) {
                for (int i = 0; i < a_count; i++) {
                    LowerDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_item->As<RE::TESObjectMISC>());
                    logger::info("lowered durability");
                }
            }
            else {
                LowerDurability(map_durability_map, MapMenuEx::total_durability_value_all_maps, a_item->As<RE::TESObjectMISC>());
                logger::info("lowered durability");
            }
        }
        logger::info("item {} removed", a_item->GetName());
        return _RemoveItem(a_this, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
    }

    void ItemAdded::AddDurability(std::unordered_map<RE::TESObjectMISC*, std::int16_t> a_mapPairs, std::int16_t a_total_durability, RE::TESObjectMISC* used_map)
    {
        //auto result = MapMenuEx::total_durability_value_all_maps;
        MapMenuEx::total_durability_value_all_maps += a_mapPairs.at(used_map);
        logger::info("new durability after add dur function is {}", MapMenuEx::total_durability_value_all_maps);
    }

    void MainUpdate::PlayerUpdate(RE::PlayerCharacter* p, float a_delta)
    {
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
        RE::Calendar* cal = RE::Calendar::GetSingleton();

        if (!RE::UI::GetSingleton()->IsMenuOpen(RE::MainMenu::MENU_NAME)) 
        {
            if (Settings::bypass_compass_checks && !compass_visible) {
                ShowCompass();
                logger::debug("bypassed compass check");
                return func(p, a_delta);
            }

            if (!Settings::bypass_compass_checks)
            {                
                if (canDestroyCompass() && compass_visible) {
                    PrintCompass();
                    destroy = false;
                }
                if (player->GetItemCount(Settings::compass) == 0 && compass_visible) {
                    logger::debug("start to hide compass");
                    HideCompass();
                    compass_visible = false;
                    hidden = true;
                }
                if (Settings::compass_duration_days > 0.0 && !hidden && Settings::enable_compass_damage) {
                    if (cal->GetHoursPassed() >= (passed_time + 1.0)) {                        
                        if (damageCompass(std::roundf(cal->GetHoursPassed() - passed_time))) {
                            logger::debug("time check for destruction");
                            destroy = true;
                        }
                        else {
                            passed_time = cal->GetHoursPassed();
                            destroy = false;
                            logger::debug("stored new time, it is {}", passed_time);
                        }
                    }
                }
                if (!compass_visible && shouldShowCompass(player) || !hidden && shouldShowCompass(player) && init) {
                    //logger::debug("start to show compass");
                    hidden = false;
                    passed_time = cal->GetHoursPassed();
                    destroy = false;
                    init = false;
                    logger::debug("stored game time it is {}", passed_time);
                    logger::debug("current durability days is: {}", Settings::compass_duration_days);
                    ShowCompass();
                }
            }
        }
        return func(p, a_delta);
    }

    void MainUpdate::Install()
    {
        REL::Relocation<std::uintptr_t> PlayerVTBL{ RE::VTABLE_PlayerCharacter[0] };
        func = PlayerVTBL.write_vfunc(0xAD, PlayerUpdate);
        logger::info("hook:Player Update");
    }

    void MainUpdate::PrintCompass() {
        if (Settings::showCompassBreak) {
            RE::DebugNotification(Settings::compassBreakMSG.c_str());
        }        
        return;
    }

    bool MainUpdate::CNOShowCompass()
    {
        return ShowHUDElement("_root.HUDMovieBaseInstance.FocusedMarkerInfo._alpha");
    }

    bool MainUpdate::CNOHideCompass()
    {
        return HideHudElement("_root.HUDMovieBaseInstance.FocusedMarkerInfo._alpha");
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

    bool MainUpdate::GetCNOCompassState()
    {
        bool visible = false;
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            visible = uiMovie->GetVariableDouble("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha") < 1.0;
        }
        return visible;
    }

    bool MainUpdate::canDestroyCompass()
    {
        return destroy && Settings::enable_compass_damage;
    }

    bool MainUpdate::damageCompass(std::int16_t a_amount)
    {
        compass_damage_val += a_amount;
        logger::debug("new damage value is {}", compass_damage_val);
        if (compass_damage_val >= (Settings::compass_duration_days * 24.0)) {
            compass_damage_val = 0.0;
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            if (player->GetItemCount(Settings::compass) > 0) {
                player->RemoveItem(Settings::compass, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
            }            
            return true;
        }
        return false;
    }

    bool MainUpdate::HasCompassItem(RE::PlayerCharacter* player)
    {
        return player->GetItemCount(Settings::compass);
    }

    bool MainUpdate::shouldShowCompass(RE::PlayerCharacter* player)
    {
        show_compass_now = false;
        if (HasCompassItem(player)) {
            show_compass_now = true;
        }
        if (Settings::bypass_compass_checks) {
            show_compass_now = true;
        }
        return show_compass_now;
    }

} // namespace Hooks
