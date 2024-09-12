#pragma once
#include "Utility.h"

namespace Events
{
    class InventoryAddHandler : public EventSingleton<InventoryAddHandler, RE::BGSAddToPlayerInventoryEvent>
    {
    public: RE::BSEventNotifyControl ProcessEvent(const RE::BGSAddToPlayerInventoryEvent* a_event, RE::BSTEventSource<RE::BGSAddToPlayerInventoryEvent>* a_eventSource) noexcept override;

    };

    class EquipEventHandler : public EventSingleton<EquipEventHandler, RE::TESEquipEvent>
    {
    public:
        RE::BSEventNotifyControl ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) noexcept override;
    };

    class MenuEvent : public RE::BSTEventSink<RE::MenuOpenCloseEvent>
    {
        MenuEvent()                            = default;
        MenuEvent(const MenuEvent&)            = delete;
        MenuEvent(MenuEvent&&)                 = delete;
        MenuEvent& operator=(const MenuEvent&) = delete;
        MenuEvent& operator=(MenuEvent&&)      = delete;

    public:
        static MenuEvent* GetSingleton()
        {
            static MenuEvent singleton;
            return &singleton;
        }

        RE::BSEventNotifyControl ProcessEvent(const RE::MenuOpenCloseEvent* event, RE::BSTEventSource<RE::MenuOpenCloseEvent>*) override;



        void RegisterMenuEvents()
        {
            if (const auto scripts = RE::UI::GetSingleton()) {
                scripts->AddEventSink<RE::MenuOpenCloseEvent>(this);
                logger::info("Registered {}"sv, typeid(RE::MenuOpenCloseEvent).name());
            }
        }

    };

    class CraftingHandler : public EventSingleton<CraftingHandler, RE::ItemCrafted::Event>
    {
    public: RE::BSEventNotifyControl ProcessEvent(const RE::ItemCrafted::Event* a_event, RE::BSTEventSource<RE::ItemCrafted::Event>* a_eventSource) noexcept override;
    };

} // namespace Events
