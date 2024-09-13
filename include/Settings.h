#pragma once



class Settings : public Singleton<Settings>
{
public:
    
    static void LoadSettings() noexcept;
    void LoadForms() noexcept;
    inline static float storedTime{};
    inline static bool debug_logging{};
    //inline static float compassDestryDays{};
    inline static std::string restrictionMSG{ "" };
    inline static std::string compassBreakMSG{ "" };
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

};
