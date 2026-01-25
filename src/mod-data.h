#pragma once

namespace MOD {
	inline constexpr std::string_view MOD_NAME = "Navigation Restrictions";
	inline constexpr std::string_view TOML_DEF = "Data/SKSE/Plugins/navigation-restrictions.toml";
	inline constexpr std::string_view TOML_CUS = "Data/SKSE/Plugins/navigation-restrictions_custom.toml";
	inline constexpr std::string_view MAP_SEC = "Settings.Map";
	inline constexpr std::string_view COMP_SEC = "Settings.Compass";
	
	//Forms:
	inline constexpr std::string_view MOD_FILE = "NavigationRestrictions.esp";
	inline constexpr std::string_view SOTW_FILE = "SkillsOfTheWild.esp";
	inline constexpr RE::FormID MAP_GOOD_ID = 0x800;
	inline constexpr RE::FormID MAP_DAMAGED_ID = 0x801;
	inline constexpr RE::FormID MAP_DESTROYED_ID = 0x802;
	inline constexpr RE::FormID MAP_INDESTRUCTIBLE_ID = 0x803;
	inline constexpr RE::FormID COMP_GOOD_ID = 0x804;
	inline constexpr RE::FormID COMP_INDESTRUCTIBLE_ID = 0x81c;
	//Skills of the wild integration:
	inline constexpr RE::FormID SOTW_COMP_CHEAT_GLOBAL_ID = 0x863;
	inline constexpr RE::FormID SOTW_COMP_NON_CHEAT_GLOBAL_ID = 0x958;

	inline constexpr std::string_view kMapIndestructibleKeyword = "MapIndestructible";
	inline constexpr std::string_view kCompassIndestructibleKeyword = "CompassIndestructible";

}
