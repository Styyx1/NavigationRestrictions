#include "Events.h"
#include "Hooks.h"
#include "Logging.h"
#include "Settings.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Hooks::Install();
        auto menuevent = Events::MenuEvent::GetSingleton();
        menuevent->RegisterMenuEvents();        
        Settings::LoadSettings();
        Settings::GetSingleton()->LoadForms();
    }
    if (message->type == SKSE::MessagingInterface::kPostLoadGame) {
        Settings::GetSingleton()->CheckGlobals();
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

    logger::info("{} has finished loading.", name);
    logger::info("");

    return true;
}
