#include "Hooks.h"
#include "Logging.h"
#include "Settings.h"
#include "Serialisation.h"
#include "papyrus.h"

void Listener(SKSE::MessagingInterface::Message* message) noexcept
{
    if (message->type == SKSE::MessagingInterface::kDataLoaded) {
        Config::Forms::GetSingleton()->LoadForms();
    }

}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    Init(skse, {.trampoline = true});
    Config::Settings::GetSingleton()->UpdateSettings(false);

    if (const auto messaging{ SKSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener)) {
        return false;
    }   

    if (auto serialization = SKSE::GetSerializationInterface()) {
        serialization->SetUniqueID(Serialisation::ID);
        serialization->SetSaveCallback(&Serialisation::SaveCallback);
        serialization->SetLoadCallback(&Serialisation::LoadCallback);
        serialization->SetRevertCallback(&Serialisation::RevertCallback);
    }
    return true;
}
