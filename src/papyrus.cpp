#include "Papyrus.h"
#include "Hooks.h"

namespace Papyrus
{
	bool Bind(VM* a_vm)
	{
		if (!a_vm) {
			logger::critical("couldn't get VM State"sv);
			return false;
		}

		logger::info("{:*^30}", "FUNCTIONS"sv);

		Functions::Bind(*a_vm);

		return true;
	}

	namespace Functions
	{
		std::int32_t GetVersion(VM*, StackID, RE::StaticFunctionTag*)
		{
			return kVersion;
		}

		void UpdateNRSettings(VM*, StackID, RE::StaticFunctionTag*)
		{
			Setting::Values::Update();
			Hooks::ItemAdded::UpdateMap();
			logger::debug("called papyrus function");
		}

		void testMessageBox(VM*, StackID, RE::StaticFunctionTag*)
		{
			RE::DebugMessageBox("test if the script even does shit");
		}

		void Bind(VM& a_vm)
		{
			constexpr auto script = "NavigationRestrictionUtil"sv;

			a_vm.RegisterFunction("GetVersion", script, GetVersion, true);
			a_vm.RegisterFunction("UpdateNRSettings", script, UpdateNRSettings);
			logger::info("Registered navigation restriction functions"sv);
		}
	}	
}