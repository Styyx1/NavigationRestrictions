#pragma once
#include "settings.h"
#include "Hooks.h"


namespace Papyrus
{
	using VM = RE::BSScript::Internal::VirtualMachine;
	using StackID = RE::VMStackID;

	bool Bind(VM* a_vm);

	namespace Functions
	{

		enum
		{
			kVersion = 1
		};

		std::int32_t GetVersion(VM*, StackID, RE::StaticFunctionTag*);
		void UpdateNRSettings(VM*, StackID, RE::StaticFunctionTag*);
		void testMessageBox(VM*, StackID, RE::StaticFunctionTag*);

		void Bind(VM& a_vm);
	}
}