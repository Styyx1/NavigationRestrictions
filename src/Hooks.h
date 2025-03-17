#pragma once

namespace Hooks
{
    void Install() noexcept;

    class MainUpdate
    {
    public:
        static void PlayerUpdate(RE::PlayerCharacter* p, float a_delta);
        static void Install();
        static bool useSkillsOfTheWild();
        static bool shouldShowCompass(RE::PlayerCharacter* player);
        inline static bool _bCompassAlphaSaved = false;
        static RE::GFxValue _savedCompassAlpha;

        inline static bool destroy;
        inline static bool init;
        inline static bool show_compass_now;
        inline static bool compass_visible;
        inline static float passed_time = 0.0f;
        inline static float compass_damage_val;
        inline static float compass_durability = 3.0f;

    private:
        static void CompVisUpdate();
        static void PrintCompass();

        static bool ShowCompass();
        static bool HideCompass();
        static bool HideHudElement(const char* a_pathToVar);
        static bool ShowHUDElement(const char* a_pathToVar);
        static bool damageCompass(std::int16_t a_amount);
        static bool HasCompassItem(RE::PlayerCharacter* player);
        static inline REL::Relocation<decltype(&PlayerUpdate)> func;
    };
    struct MapMenuEx : public RE::MapMenu
    {
        static void Install();
        inline static std::int16_t current_map_damage;
        inline static std::int16_t total_durability_value_all_maps;
    private:
        RE::UI_MESSAGE_RESULTS OpenMap(RE::UIMessage& a_message);
        bool hasAtLeastOneMapItem(RE::PlayerCharacter* player);
        bool shouldOpenMap(RE::PlayerCharacter* player);
        void damage_map_item(uint16_t a_damage_amount);
        void destroy_map_item(RE::TESObjectMISC* a_map_item, RE::PlayerCharacter* player);
        RE::TESObjectMISC* GetCurrentMapItem(RE::PlayerCharacter* player);
        void showRestrictionMessage();
        bool hasIndestructibleMap(RE::PlayerCharacter* player);
        inline static REL::Relocation<decltype(&RE::MapMenu::ProcessMessage)> func;
    };
    struct ItemAdded : public RE::PlayerCharacter
    {
        static void InstallAddItemHook();
        static void InstallRemoveItemHook();
        static void InstallPickupHook();
        inline static std::unordered_map<RE::TESObjectMISC*, std::int16_t> map_durability_map;
        static void PopulateMap();
        static void UpdateMap();

    private:
        static void PickUpObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound);
        static void OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr);
        static RE::ObjectRefHandle OnItemRemoved(RE::Actor* a_this, RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate);
        static void AddDurability(std::unordered_map<RE::TESObjectMISC*, std::int16_t> a_mapPairs, std::int16_t a_total_durability, RE::TESObjectMISC* used_map);
        static void LowerDurability(std::unordered_map<RE::TESObjectMISC*, std::int16_t> a_mapPairs, std::int16_t a_total_durability, RE::TESObjectMISC* used_map);
        
        inline static REL::Relocation<decltype(&OnItemAdded)> _AddObjectToContainer;
        inline static REL::Relocation<decltype(&PickUpObject)> _PickUpObject;
        inline static REL::Relocation<decltype(&OnItemRemoved)> _RemoveItem;

    };
} // namespace Hooks
