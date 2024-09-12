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
        bool hidden{};
        Utility* util = Utility::GetSingleton();
        Settings* settings = Settings::GetSingleton();
        RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
        if (util->GetItemCount(player, settings->compass) == 0 && !hidden) {
            //logger::debug("start to hide compass");
            util->HideCompass();
            hidden = true;
        }
        else {
            //logger::debug("start to show compass");
            hidden = false;
            util->ShowCompass();
        }

        return func();
    }
} // namespace Hooks
