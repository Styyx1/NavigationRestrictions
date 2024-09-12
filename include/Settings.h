#pragma once



class Settings : public Singleton<Settings>
{
public:
    
    static void LoadSettings() noexcept;
    void LoadForms() noexcept;

    inline static bool debug_logging{};
#define armo RE::TESObjectMISC*
#define glob RE::TESGlobal*
    armo map;
    armo map_damaged;
    armo map_indestructible;
    armo map_destroyed;
    armo compass;
    glob durability;
    glob damageValue;

};
