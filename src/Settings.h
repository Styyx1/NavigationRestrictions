#pragma once



class Settings : public Singleton<Settings>
{
public:
    
    static void LoadSettings() noexcept;
    void LoadForms() noexcept;
    void CheckGlobals();
    inline static float storedTime{};
    inline static bool debug_logging{};
    inline static bool enableCompassDamage{};
    inline static std::string restrictionMSG{ "" };
    inline static std::string compassBreakMSG{ "" };
    inline static bool showCompassBreak{};
    //new mcm rewrite
    inline static bool bypass_map_checks{ false };
    inline static bool bypass_compass_checks{ false };
    inline static uint16_t map_durability_total{ 50 };
#define armo RE::TESObjectMISC*
#define glob RE::TESGlobal*
    inline static armo map;
    inline static armo map_damaged;
    inline static armo map_indestructible;
    inline static armo map_destroyed;
    inline static armo compass;
    inline static glob durability;
    inline static glob damageValue;
    inline static glob bypassMapCheck;
    inline static glob bypassCompassCheck;
    inline static glob compassDurationDays;
    inline static glob compassDurability;
    inline static glob compassDamage;
    inline static glob timeStorage;

};
