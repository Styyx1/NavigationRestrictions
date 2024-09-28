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
        RE::Calendar* cal = RE::Calendar::GetSingleton();

        if (!RE::UI::GetSingleton()->IsMenuOpen(RE::MainMenu::MENU_NAME)) {
            if (settings->bypassCompassCheck->value == 0.0) {
                if (destroy && settings->enableCompassDamage) {
                    util->PrintCompass(settings);
                    logger::debug("destroyed compass");
                    destroy = false;
                }
                if (player->GetItemCount(settings->compass) == 0 && !util->GetCompassVisibilityState()) {
                    logger::debug("start to hide compass");
                    //hidden = util->ShowCompass();
                    util->CNOHideCompass();
                    util->HideCompass();
                    hidden = true;
                }
                if (settings->compassDurationDays->value > 0.0 && !hidden && settings->enableCompassDamage) {
                    if (cal->GetHoursPassed() >= (settings->timeStorage->value + 1.0)) {                        
                        if (util->damageCompassByOne(settings, player, std::roundf(cal->GetHoursPassed() - settings->timeStorage->value))) {
                            logger::debug("time check for destruction");
                            destroy = true;
                        }
                        else {
                            settings->timeStorage->value = cal->GetHoursPassed();
                            logger::debug("stored new time, it is {}", settings->timeStorage->value);
                        }
                    }
                }
                if (util->GetCompassVisibilityState() && player->GetItemCount(settings->compass) > 0) {
                    //logger::debug("start to show compass");
                    hidden = false;
                    settings->timeStorage->value = cal->GetHoursPassed();
                    logger::debug("stored game time it is {}", settings->timeStorage->value);
                    logger::debug("current durability days is: {}", settings->compassDurationDays->value);
                    util->CNOShowCompass();
                    util->ShowCompass();
                }
            }
            else if (util->GetCompassVisibilityState() || util->GetCNOCompassState()) {
                logger::debug("Compass check bypassed, enable compass");
                hidden = false;
                util->ShowCompass();
            }      
        }        
        return func();
    }
} // namespace Hooks
