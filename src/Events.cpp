#include "Events.h"
#include "Settings.h"

namespace Events
{
    RE::BSEventNotifyControl EquipEventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) noexcept
    {
        if (!a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }

        // Do stuff

        return RE::BSEventNotifyControl::kContinue;
    }

   

    const char* plugin_name = "Helps To Have A Map.esp";
    const int MapItem = 0x862;

    inline void HideMenu(RE::BSFixedString a_menuName)
    {
        if (const auto UIMsgQueue = RE::UIMessageQueue::GetSingleton(); UIMsgQueue) {
            UIMsgQueue->AddMessage(a_menuName, RE::UI_MESSAGE_TYPE::kHide, nullptr);
        }
    }

    RE::BSEventNotifyControl MenuEvent::ProcessEvent(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*)
    {
        if (!event)
            return RE::BSEventNotifyControl::kContinue;
        if (event->menuName != RE::MapMenu::MENU_NAME)
            return RE::BSEventNotifyControl::kContinue;
        if (event->menuName == RE::MapMenu::MENU_NAME) {
            Utility* util = Utility::GetSingleton();
            Settings* settings = Settings::GetSingleton(); 
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            if (util->GetItemCount(player, settings->map) == 0 || util->GetItemCount(player, settings->map_destroyed) >= 1) {
                HideMenu(RE::MapMenu::MENU_NAME);                
            }
            else {
                int damageVal = static_cast<int>(settings->damageValue->value);
                int durabilityVal = static_cast<int>(settings->durability->value);
                ++damageVal;
                if (damageVal == durabilityVal && util->GetItemCount(player, settings->map_destroyed) == 0 && util->GetItemCount(player, settings->map_indestructible) == 0) {
                    if (util->GetItemCount(player, settings->map) == 1) {
                        player->RemoveItem(settings->map, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                        player->AddObjectToContainer(settings->map_damaged, nullptr, 1, nullptr);                        
                        settings->damageValue->value = 0.0;
                    }
                    if (util->GetItemCount(player, settings->map_damaged) == 1) {
                        player->RemoveItem(settings->map_damaged, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);                      
                        player->AddObjectToContainer(settings->map_destroyed, nullptr, 1, nullptr);
                        settings->damageValue->value = 0.0;
                    }
                }
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl InventoryAddHandler::ProcessEvent(const RE::BGSAddToPlayerInventoryEvent* a_event, RE::BSTEventSource<RE::BGSAddToPlayerInventoryEvent>* a_eventSource) noexcept
    {
        if (!a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }
        Settings* settings = Settings::GetSingleton();
        if (a_event->itemBase == settings->map) {
            logger::debug("added {} to players inventory", a_event->itemBase->GetName());
            auto const player = RE::PlayerCharacter::GetSingleton();
            if (player->GetItemCount(settings->map_damaged) > 0) {
                player->RemoveItem(settings->map_damaged, player->GetItemCount(settings->map_damaged), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                settings->damageValue->value = 0;
            }
            if (player->GetItemCount(settings->map_destroyed) > 0) {
                player->RemoveItem(settings->map_destroyed, player->GetItemCount(settings->map_destroyed), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
            }
            if (player->GetItemCount(settings->map) > 1) {
                settings->damageValue->value = 0;
                auto gotItemContainer = a_event->containerRef.get().get();
                player->RemoveItem(settings->map, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, gotItemContainer, nullptr);
                
            }
        }
        // Do stuff

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl CraftingHandler::ProcessEvent(const RE::ItemCrafted::Event* a_event, RE::BSTEventSource<RE::ItemCrafted::Event>* a_eventSource) noexcept
    {
        if (!a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }
        Settings* settings = Settings::GetSingleton();
        if (a_event->item == settings->map) {
            settings->damageValue->value = 0;
        }

        return RE::BSEventNotifyControl::kContinue;
    }

} // namespace Events
