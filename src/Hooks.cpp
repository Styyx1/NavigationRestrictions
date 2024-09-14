#include "Hooks.h"
#include "Utility.h"
#include "Settings.h"

namespace Hooks
{
    void Install() noexcept
    {
        stl::write_thunk_call<MainUpdate>();
        logger::info("Installed main update hook");
        logger::info("");
    }

    

    i32 MainUpdate::Thunk() noexcept
    {
       
        Utility* util = Utility::GetSingleton();
        Settings* settings = Settings::GetSingleton();
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
        if (!RE::UI::GetSingleton()->IsMenuOpen(RE::MainMenu::MENU_NAME)) {
            if (settings->bypassCompassCheck->value == 0.0) {
                if (destroy) {
                    player->RemoveItem(settings->compass, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                    util->PrintCompass(settings);
                    logger::debug("destroyed compass");
                    destroy = false;
                }
                if (player->GetItemCount(settings->compass) == 0 && !util->GetCompassVisibilityState()) {
                    logger::debug("start to hide compass");
                    //hidden = util->ShowCompass();
                    util->HideCompass();
                    hidden = true;
                }

                if (settings->compassDurationDays->value > 0.0) {                
                    if (RE::Calendar::GetSingleton()->gameDay->value == (settings->storedTime + settings->compassDurationDays->value)) {
                        settings->storedTime = 0;
                        logger::debug("time check for destruction");
                        destroy = true;
                    }
                }
                if (util->GetCompassVisibilityState() && player->GetItemCount(settings->compass) > 0) {
                    //logger::debug("start to show compass");
                    hidden = false;
                    settings->storedTime = RE::Calendar::GetSingleton()->gameDay->value;
                    logger::debug("stored game time");
                    util->ShowCompass();
                }
            }
            else if (util->GetCompassVisibilityState()) {
                logger::debug("Compass check bypassed, enable compass");
                hidden = false;
                util->ShowCompass();
            }      
        }        
        return func();
    }
} // namespace Hooks
