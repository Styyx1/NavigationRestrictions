#pragma once

namespace Hooks
{
    struct DurabilityTracker : REX::Singleton<DurabilityTracker>
    {
        std::unordered_map<RE::TESBoundObject*, uint32_t> durability_pool;
        std::unordered_map<RE::TESBoundObject*, uint32_t> durability_amounts;
        std::unordered_set<RE::TESBoundObject*> tracked_items;

        void AddItemToPool(RE::TESBoundObject* a_item, uint32_t a_durabiltyAmount, uint32_t a_itemCount = 1);
        void DamageItem(RE::TESBoundObject* a_item, RE::PlayerCharacter* a_player, uint32_t a_damageAmount = 1);
        void RemoveItemFromPool(RE::TESBoundObject* a_item, uint32_t a_durabiltyAmount, uint32_t a_itemCount = 1);
        uint32_t GetRemainingDurability(RE::TESBoundObject* a_item) const;
        bool IsItemBroken(RE::TESBoundObject* a_item) const;
        void PopulateMapFromInventory(RE::PlayerCharacter* player);
        void GenerateDurabilityAmounts();
    };

    struct MapMenuEx
    {
        inline static std::int16_t current_map_damage;
        inline static std::int16_t total_durability_value_all_maps;

    private:
        static RE::UI_MESSAGE_RESULTS MapOpen(RE::MapMenu* a_this, RE::UIMessage& a_message);
        static bool hasAtLeastOneMapItem(RE::PlayerCharacter* player);
        static bool ShouldOpenMap(RE::PlayerCharacter* player);
        static bool HasIndestructibleMap(RE::PlayerCharacter* player);
        static RE::TESObjectMISC* GetCurrentMapItem(RE::PlayerCharacter* player);
        static void showRestrictionMessage();

        inline static REL::HookVFT _MapOpen{ RE::MapMenu::VTABLE[0], 0x4, MapOpen };
    };

    struct ItemManip : RE::PlayerCharacter
    {
        static void InstallAddItemHook();
        static void InstallPickupHook();
        static void InstallDropObjectHook();

    private:
        static void PickUpObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound);
        static void OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr);
        static RE::ObjectRefHandle DropObject(RE::PlayerCharacter* player, const RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, std::int32_t a_count, const RE::NiPoint3* a_dropLoc = 0, const RE::NiPoint3* a_rotate = 0);                       // 0CB

        inline static REL::Relocation<decltype(&OnItemAdded)> _AddObjectToContainer;
        inline static REL::Relocation<decltype(&PickUpObject)> _PickUpObject;
        inline static REL::Relocation<decltype(&DropObject)> _DropObject;
    };

    struct CompassHook : REX::Singleton<CompassHook> {

        static void InstallCompassHook();
        bool state_show_compass{ true };
        bool GetCompassState() const;
        void SetCompassState(bool b_show);
        void UpdateCompassState();
        void ForceShowCompass();

    private:
        static inline void Update(RE::HUDObject* a_this);
        bool HasCompassItem() const;
        bool ShouldShowCompass() const;
        bool HasIndestructibleCompass(RE::PlayerCharacter* a_player) const;
        RE::TESObjectMISC* GetCompassFromInventory(RE::PlayerCharacter* a_player) const;
        void ShowCompassBreakMessage();
        void DoDamageCompass();
        TimerUtil::Timer compass_timer;
        static inline REL::Relocation<decltype(&Update)> _UpdateComp;

    };
} 
