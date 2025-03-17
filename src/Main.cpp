#include "Hooks.h"
#include "Logging.h"
#include "Settings.h"
#include "Serialisation.h"
#include "papyrus.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Hooks::Install();       
        Settings::GetSingleton()->LoadSettings();
        Settings::GetSingleton()->LoadForms();
        Hooks::ItemAdded::PopulateMap();
        Hooks::ItemAdded::UpdateMap();
        Hooks::MainUpdate::init = true;
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
        Hooks::MainUpdate::init = true;
        Hooks::MainUpdate::shouldShowCompass(RE::PlayerCharacter::GetSingleton());
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    InitLogging();

    const auto plugin{ SKSE::PluginDeclaration::GetSingleton() };
    const auto name{ plugin->GetName() };
    const auto version{ plugin->GetVersion() };

    logger::info("{} {} is loading...", name, version);

    Init(skse);
    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener)) {
        return false;
    }
    SKSE::GetPapyrusInterface()->Register(Papyrus::Bind);

    if (auto serialization = SKSE::GetSerializationInterface()) {
        serialization->SetUniqueID(Serialisation::ID);
        serialization->SetSaveCallback(&Serialisation::SaveCallback);
        serialization->SetLoadCallback(&Serialisation::LoadCallback);
        serialization->SetRevertCallback(&Serialisation::RevertCallback);
    }

    logger::info("{} has finished loading.", name);
    logger::info("");

    return true;
}
