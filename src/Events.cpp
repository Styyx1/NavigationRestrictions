#include "Events.h"
#include "Settings.h"

namespace Events
{    
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
        if (event->menuName != RE::MapMenu::MENU_NAME && event->menuName != RE::InventoryMenu::MENU_NAME)
            return RE::BSEventNotifyControl::kContinue;        

        if (event->menuName == RE::InventoryMenu::MENU_NAME) {
            Utility* util = Utility::GetSingleton();
            Settings* settings = Settings::GetSingleton();
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            
            if (player->GetItemCount(settings->map_damaged) > 1) {
                player->RemoveItem(settings->map_damaged, player->GetItemCount(settings->map_damaged), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                RE::SendUIMessage::SendInventoryUpdateMessage(player, nullptr);
                logger::debug("updated item count");
                settings->damageValue->value = 0;
                logger::debug("reset damage value");
            }
            if (player->GetItemCount(settings->map_destroyed) > 1) {
                player->RemoveItem(settings->map_destroyed, player->GetItemCount(settings->map_destroyed), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                RE::SendUIMessage::SendInventoryUpdateMessage(player, nullptr);
                logger::debug("updated item count");
            }
            if (player->GetItemCount(settings->map) > 1) {
                settings->damageValue->value = 0;
                player->RemoveItem(settings->map, (player->GetItemCount(settings->map) - 1), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                RE::SendUIMessage::SendInventoryUpdateMessage(player, nullptr);
                logger::debug("updated item count");
                if (player->GetItemCount(settings->map_damaged) > 0) {
                    player->RemoveItem(settings->map_damaged, player->GetItemCount(settings->map_damaged), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                    RE::SendUIMessage::SendInventoryUpdateMessage(player, nullptr);
                    logger::debug("updated item count");
                }
                if (player->GetItemCount(settings->map_destroyed) > 0) {
                    player->RemoveItem(settings->map_destroyed, player->GetItemCount(settings->map_destroyed), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                    RE::SendUIMessage::SendInventoryUpdateMessage(player, nullptr);
                    logger::debug("updated item count");
                }
            }            
        }
        if (event->menuName == RE::MapMenu::MENU_NAME && event->opening) {
            bool damaged = false;
            Utility* util = Utility::GetSingleton();
            Settings* settings = Settings::GetSingleton(); 
            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
            if (util->GetItemCount(player, settings->map) == 0 || util->GetItemCount(player, settings->map_destroyed) >= 1) {
                HideMenu(RE::MapMenu::MENU_NAME);
                util->PrintMessage(settings);
                logger::debug("can't open menu");
            }
            else {
                settings->damageValue->value += 1;
                if (static_cast<int>(settings->damageValue->value) == static_cast<int>(settings->durability->value) && util->GetItemCount(player, settings->map_destroyed) == 0 && util->GetItemCount(player, settings->map_indestructible) == 0) {
                    if (util->GetItemCount(player, settings->map) == 1 && !damaged) {
                        player->RemoveItem(settings->map, (player->GetItemCount(settings->map)), RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);
                        player->AddObjectToContainer(settings->map_damaged, nullptr, 1, nullptr);                        
                        settings->damageValue->value = 0.0;
                        damaged = true;
                    }
                    if (util->GetItemCount(player, settings->map_damaged) == 1 && !damaged) {
                        player->RemoveItem(settings->map_damaged, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr, nullptr);                      
                        player->AddObjectToContainer(settings->map_destroyed, nullptr, 1, nullptr);
                        settings->damageValue->value = 0.0;
                        damaged = true;
                    }
                }
            }
        }
        return RE::BSEventNotifyControl::kContinue;
    }
} // namespace Events
