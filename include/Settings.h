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
    //inline static float compassDestryDays{};
    inline static std::string restrictionMSG{ "" };
    inline static std::string compassBreakMSG{ "" };
    inline static bool showCompassBreak{};
#define armo RE::TESObjectMISC*
#define glob RE::TESGlobal*
    armo map;
    armo map_damaged;
    armo map_indestructible;
    armo map_destroyed;
    armo compass;
    glob durability;
    glob damageValue;
    glob bypassMapCheck;
    glob bypassCompassCheck;
    glob compassDurationDays;
    glob compassDurability;
    glob compassDamage;
    glob timeStorage;

};
