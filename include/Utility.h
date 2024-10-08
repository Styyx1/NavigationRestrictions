#pragma once
#include "Settings.h"

class Utility : public Singleton<Utility>
{
public:

    void LogBool(bool bLog) {
        if (bLog) {
            logger::debug("true");
        }
        else
            logger::debug("false");
    }

    void LogItemCount(RE::TESObjectMISC* item, int count) {
        logger::debug("player has {} of {} in the inventory", count, item->GetName());
    }

    bool damageCompassByOne(Settings* settings, RE::PlayerCharacter* player, int damageVal) {
        settings->compassDamage->value = settings->compassDamage->value + damageVal;
        logger::debug("new damage value = {}", settings->compassDamage->value);
        if (settings->compassDamage->value >= settings->compassDurability->value * (settings->compassDurationDays->value * 24.0)) {
            settings->compassDamage->value = 0.0;
            player->RemoveItem(settings->compass, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
            logger::debug("removed compass");
            return true;
        }
        return false;
    }

    bool HasMap(RE::PlayerCharacter* actor) {
        Settings* settings = Settings::GetSingleton();
        bool has_it = false;
        if (actor->GetItemCount(settings->map) > 0)
            
            has_it = true;

        if (actor->GetItemCount(settings->map_damaged) > 0)
            
            has_it = true;

        if (actor->GetItemCount(settings->map_indestructible) > 0)
            
            has_it = true;
 
        if (actor->GetItemCount(settings->map_destroyed) > 0)
            has_it = false;

        LogItemCount(settings->map, actor->GetItemCount(settings->map));
        LogItemCount(settings->map_damaged, actor->GetItemCount(settings->map_damaged));
        LogItemCount(settings->map_indestructible, actor->GetItemCount(settings->map_indestructible));
        LogItemCount(settings->map_destroyed, actor->GetItemCount(settings->map_destroyed));
        LogBool(has_it);
        return has_it;
    }

    static void PrintMessage(Settings* settings) {
        RE::DebugNotification(settings->restrictionMSG.c_str());
        return;
    }

    static void PrintCompass(Settings* settings) {
        if (settings->showCompassBreak == true) {
            RE::DebugNotification(settings->compassBreakMSG.c_str());
        }        
        return;
    }

    bool CNOShowCompass() {

        return ShowHUDElement("_root.HUDMovieBaseInstance.FocusedMarkerInfo._alpha");
    }
    bool CNOHideCompass()
    {
        return HideHudElement("_root.HUDMovieBaseInstance.FocusedMarkerInfo._alpha");
    }


    bool ShowCompass() 
    {
        return ShowHUDElement("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha");
    }

    bool HideCompass()
    {
        return HideHudElement("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha");
    }

    bool HideHudElement(const char* a_pathToVar) {
        bool visible = false;
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            uiMovie->SetVariableDouble(a_pathToVar, 0.0);
            visible = uiMovie->GetVariableDouble(a_pathToVar) < 1.0;  // invert visible            
        }
        return visible;
    }

    bool ShowHUDElement(const char* a_pathToVar)
    {
        bool visible = false;
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            uiMovie->SetVariableDouble(a_pathToVar, 100.0);
            visible = uiMovie->GetVariableDouble(a_pathToVar) < 1.0;  // invert visible            
        }
        return visible;
    }

    bool GetCNOCompassState() {
        bool visible = false;
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            visible = uiMovie->GetVariableDouble("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha") < 1.0;
        }
        return visible;
    }

    bool GetCompassVisibilityState() {
        bool visible = false;
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            visible = uiMovie->GetVariableDouble("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha") < 1.0;
        }
        return visible;
    }

};
