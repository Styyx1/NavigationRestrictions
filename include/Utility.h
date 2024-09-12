#pragma once

class Utility : public Singleton<Utility>
{
public:

    /* int GetItemCount(RE::Actor* a_actor, RE::TESBoundObject* item);*/
    template <int id, typename x_Function>
    class _generic_foo_;

    template <int id, typename T, typename... Args>
    class _generic_foo_<id, T(Args...)>
    {
    public:
        static T eval(Args... args)
        {
            using func_t = T(Args...);
            REL::Relocation<func_t> func{ REL::ID(id) };
            return func(std::forward<Args>(args)...);
        }
    };

    int GetItemCount(RE::Actor* a_actor, RE::TESBoundObject* item)
    {            
        auto runtime = REL::Module::GetRuntime();
        if (auto changes = a_actor->GetInventoryChanges()) {
            if (runtime == REL::Module::Runtime::SE) {
                return _generic_foo_<15868, int(RE::InventoryChanges*, RE::TESBoundObject*)>::eval(changes, item);
            }
            if (runtime == REL::Module::Runtime::AE) {
                return _generic_foo_<15642, int(RE::InventoryChanges*, RE::TESBoundObject*)>::eval(changes, item);
            }
        }
        return 0;
    }    

    bool ShowCompass() 
    {
        return ShowHUDElement("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha");
    }

    bool HideCompass()
    {
        return HideHudElement("_root.HUDMovieBaseInstance.CompassShoutMeterHolder._alpha");
    }

    bool HideHudElement(const char* a_pathToVar) {
        bool visible = false;
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            uiMovie->SetVariableDouble(a_pathToVar, 0.0);
            visible = uiMovie->GetVariableDouble(a_pathToVar) < 1.0;  // invert visible            
        }
        return visible;
    }

    bool ShowHUDElement(const char* a_pathToVar)
    {
        bool visible = false;
        if (auto uiMovie = RE::UI::GetSingleton()->GetMovieView(RE::HUDMenu::MENU_NAME)) {
            uiMovie->SetVariableDouble(a_pathToVar, 100.0);
            visible = uiMovie->GetVariableDouble(a_pathToVar) < 1.0;  // invert visible            
        }
        return visible;
    }

};
