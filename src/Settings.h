#pragma once



class Settings : public Singleton<Settings>
{
public:
    
    void LoadSettings();
    void LoadForms() noexcept;
    void UpdateSettings(CSimpleIniA &ini, std::filesystem::path path);
    inline static float storedTime{};
    inline static bool debug_logging{};
    inline static bool enableCompassDamage{};
    inline static std::string restrictionMSG{ "" };
    inline static std::string compassBreakMSG{ "" };
    inline static bool showCompassBreak{};
    inline static std::int16_t durability_map_normal;
    inline static std::int16_t durability_map_damaged;
    inline static bool bypass_map_checks{ false };
    inline static bool bypass_compass_checks{ false };
    inline static bool enable_compass_damage{ true };
    inline static float compass_duration_days{3.0f};
    inline static uint16_t map_durability_total{ 50 };
#define armo RE::TESObjectMISC*
#define glob RE::TESGlobal*
    inline static armo map;
    inline static armo map_damaged;
    inline static armo map_indestructible;
    inline static armo map_destroyed;
    inline static armo compass;

};
