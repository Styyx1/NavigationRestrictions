#include "hooks.h"
#include "Settings.h"
namespace Hooks {
	using set = Config::Settings;
	using fitem = Config::Forms;
#pragma region MapHook


	RE::UI_MESSAGE_RESULTS MapMenuEx::MapOpen(RE::MapMenu* a_this, RE::UIMessage& a_message)
	{
		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		if (a_message.type == RE::UI_MESSAGE_TYPE::kShow && set::toggle_compass_check.GetValue()) {
			if (!ShouldOpenMap(player)) {
				REX::DEBUG("restrict map hook");
				showRestrictionMessage();
				return RE::UI_MESSAGE_RESULTS::kIgnore;
			}
			else {
				RE::TESObjectMISC* curr_map = GetCurrentMapItem(player);
				const auto& forms = Config::Forms::GetSingleton();
				if (set::toggle_damage_map.GetValue()) {
					if (curr_map != fitem::map_indestructible && !HasIndestructibleMap(player)) {
						DurabilityTracker::GetSingleton()->DamageItem(curr_map, player, 1);
					}
				}
			}
		}
		return _MapOpen(a_this, a_message);
	}

	bool MapMenuEx::hasAtLeastOneMapItem(RE::PlayerCharacter* player)
	{
		if (player->GetItemCount(fitem::map_new) > 0 ||
			player->GetItemCount(fitem::map_damaged) > 0 ||
			player->GetItemCount(fitem::map_indestructible) > 0) {
			return true;
		}

		const auto& inv = player->GetInventory();
		for (const auto& [object, pair] : inv) {
			if (object && object->HasKeywordByEditorID(MOD::kMapIndestructibleKeyword)) {
				return true;
			}
		}
		return false;
	}

	bool MapMenuEx::ShouldOpenMap(RE::PlayerCharacter* player)
	{
		return hasAtLeastOneMapItem(player) || !set::toggle_map_check.GetValue();
	}

	bool MapMenuEx::HasIndestructibleMap(RE::PlayerCharacter* player)
	{
		if (player->GetItemCount(fitem::map_indestructible) > 0) {
			return true;
		}
		const auto& inv = player->GetInventory();
		for (const auto& [object, pair] : inv) {
			if (object && object->HasKeywordByEditorID(MOD::kMapIndestructibleKeyword)) {
				return true;
			}
		}
		return false;
	}

	RE::TESObjectMISC* MapMenuEx::GetCurrentMapItem(RE::PlayerCharacter* player)
	{
		if (player->GetItemCount(fitem::map_new) > 0) {
			return fitem::map_new;
		}
		if (player->GetItemCount(fitem::map_damaged) > 0) {
			return fitem::map_damaged;
		}
		if (player->GetItemCount(fitem::map_indestructible) > 0) {
			return fitem::map_indestructible;
		}
		return nullptr;
	}

	void MapMenuEx::showRestrictionMessage()
	{
		std::string text = set::map_restrict_notification.GetValue();
		RE::SendHUDMessage::ShowHUDMessage(text.c_str(), nullptr, true);
	}

#pragma endregion MapHook
#pragma region DurabiltyTracker

	void DurabilityTracker::AddItemToPool(RE::TESBoundObject* a_item, uint32_t a_durabilityAmount, uint32_t a_itemCount)
	{
		if (!a_item)
			return;
		durability_pool[a_item] += a_durabilityAmount * a_itemCount;
		REX::DEBUG("added {} of {} to the durability map. total durability is now: {}", a_itemCount, a_item->GetName(), durability_pool[a_item]);
	}

	void DurabilityTracker::DamageItem(RE::TESBoundObject* a_item, RE::PlayerCharacter* a_player, uint32_t a_damageAmount)
	{
		if (!a_item)
			return;

		uint32_t item_dur = durability_amounts.contains(a_item) ? durability_amounts[a_item] : 0;
		REX::DEBUG("item durability is {}", item_dur);

		if (durability_pool.contains(a_item)) {
			durability_pool[a_item] -= a_damageAmount;

			if (durability_pool[a_item] <= 0) {
				a_player->RemoveItem(a_item, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
				durability_pool.erase(a_item);
				if (a_item == fitem::compass_new) {
					CompassHook::GetSingleton()->UpdateCompassState();
				}
			}
			else {
				uint32_t itemCount = a_player->GetItemCount(a_item);
				uint32_t max_possible_durability = itemCount * item_dur;
				if (durability_pool[a_item] < (max_possible_durability - item_dur)) {
					a_player->RemoveItem(a_item, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
					if (a_item == fitem::compass_new) {
						CompassHook::GetSingleton()->UpdateCompassState();
					}
				}
			}
			REX::DEBUG("Damaged {} by {}, new durability: {}", a_item->GetName(), a_damageAmount, durability_pool[a_item]);
		}
	}

	void DurabilityTracker::RemoveItemFromPool(RE::TESBoundObject* a_item, uint32_t a_durabilityAmount, uint32_t a_itemCount)
	{
		if (!a_item)
			return;
		if (durability_pool.contains(a_item)) {
			uint32_t removal_amount = a_durabilityAmount * a_itemCount;
			if (removal_amount >= durability_pool[a_item]) {
				durability_pool.erase(a_item);
			}
			else {
				durability_pool[a_item] -= removal_amount;
			}
		}
	}

	uint32_t DurabilityTracker::GetRemainingDurability(RE::TESBoundObject* a_item) const
	{
		return durability_pool.contains(a_item) ? durability_pool.at(a_item) : 0;
	}

	bool DurabilityTracker::IsItemBroken(RE::TESBoundObject* a_item) const
	{
		return durability_pool.contains(a_item) ? durability_pool.at(a_item) <= 0 : true;
	}

	void DurabilityTracker::PopulateMapFromInventory(RE::PlayerCharacter* player)
	{
		const auto& inv = player->GetInventory();
		for (auto& item : inv) {
			if (tracked_items.contains(item.first)) {
				if (!durability_pool.contains(item.first)) {
					AddItemToPool(item.first, durability_amounts[item.first], (uint32_t)item.second.first);
				}
			}
		}
	}

	void DurabilityTracker::GenerateDurabilityAmounts()
	{
		durability_amounts = {
			{fitem::map_new, set::map_good_durabilty},
			{fitem::map_damaged, set::map_damaged_durabilty},
			{fitem::compass_new, set::compass_durability}
		};
		tracked_items = {
			fitem::map_new, fitem::map_damaged, fitem::compass_new
		};
	}

#pragma endregion DurabilityTracker
#pragma region ItemManip
	void ItemManip::InstallAddItemHook()
	{
		REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
		_AddObjectToContainer = PlayerCharacterVtbl.write_vfunc(0x5A, OnItemAdded);
		REX::INFO("Installed OnItemAdded Hook");
	}

	void ItemManip::InstallPickupHook()
	{
		REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };
		_PickUpObject = PlayerCharacterVtbl.write_vfunc(0xCC, PickUpObject);
		REX::INFO("Installed PickUpObject Hook");
	}

	void ItemManip::InstallDropObjectHook()
	{
		REL::Relocation<std::uintptr_t> PlayerVtbl{ RE::VTABLE_PlayerCharacter[0] };
		_DropObject = PlayerVtbl.write_vfunc(0x0CB, DropObject);
		REX::INFO("Installed DropObject Hook");
	}

	void ItemManip::PickUpObject(RE::Actor* a_this, RE::TESObjectREFR* a_object, uint32_t a_count, bool a_arg3, bool a_playSound)
	{
		_PickUpObject(a_this, a_object, a_count, a_arg3, a_playSound);
		const auto& tracker = DurabilityTracker::GetSingleton();
		if (tracker->tracked_items.contains(a_object->GetBaseObject())) {
			REX::INFO("picked up {} of {}", a_count, a_object->GetBaseObject()->GetName());
			tracker->AddItemToPool(a_object->GetBaseObject(), tracker->durability_amounts[a_object->GetBaseObject()], a_count);
			CompassHook::GetSingleton()->UpdateCompassState();
		}
	}

	void ItemManip::OnItemAdded(RE::Actor* a_this, RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, int32_t a_count, RE::TESObjectREFR* a_fromRefr)
	{
		_AddObjectToContainer(a_this, a_object, a_extraList, a_count, a_fromRefr);
		const auto& tracker = DurabilityTracker::GetSingleton();
		if (tracker->tracked_items.contains(a_object)) {
			REX::INFO("added {} of {}", a_count, a_object->GetName());
			tracker->AddItemToPool(a_object, tracker->durability_amounts[a_object], a_count);
			CompassHook::GetSingleton()->UpdateCompassState();
		}
	}

	RE::ObjectRefHandle ItemManip::DropObject(RE::PlayerCharacter* player, const RE::TESBoundObject* a_object, RE::ExtraDataList* a_extraList, std::int32_t a_count, const RE::NiPoint3* a_dropLoc, const RE::NiPoint3* a_rotate)
	{
		auto handle = _DropObject(player, a_object, a_extraList, a_count, a_dropLoc, a_rotate);
		if (a_object) {
			const auto& tracker = DurabilityTracker::GetSingleton();
			auto obj = const_cast<RE::TESBoundObject*>(a_object);
			if (tracker->tracked_items.contains(obj)) {
				REX::INFO("removed {} of {}", a_count, a_object->GetName());
				tracker->RemoveItemFromPool(obj, tracker->durability_amounts[obj], a_count);
				CompassHook::GetSingleton()->UpdateCompassState();
			}
		}
		return handle;
	}
#pragma endregion ItemManip
#pragma region CompassHook
	void CompassHook::InstallCompassHook()
	{
		REL::Relocation<std::uintptr_t> vTable(RE::VTABLE_Compass[0]);
		_UpdateComp = vTable.write_vfunc(0x1, &Update);
		REX::INFO("Compass Update installed");
	}

	bool CompassHook::GetCompassState() const
	{
		return state_show_compass;
	}

	void CompassHook::SetCompassState(bool b_show)
	{
		state_show_compass = b_show;
	}

	void CompassHook::UpdateCompassState()
	{
		state_show_compass = ShouldShowCompass();
		bool shouldShow = ShouldShowCompass();
		SetCompassState(shouldShow);
	}

	void CompassHook::ForceShowCompass()
	{
		SetCompassState(true);
	}

	void CompassHook::Update(RE::HUDObject* a_this)
	{
		TimerUtil::Timer timer;
		if (a_this) {
			auto MovieView = a_this->view.get();
			if (MovieView) {
				RE::GFxValue compassHolder;
				a_this->root.GetMember("CompassShoutMeterHolder", &compassHolder);
				if (compassHolder.IsDisplayObject()) {
					RE::GFxValue actual_compass;
					compassHolder.GetMember("Compass", &actual_compass);
					RE::GFxValue::DisplayInfo displayInfo;
					actual_compass.GetDisplayInfo(std::addressof(displayInfo));
					if (!CompassHook::GetSingleton()->GetCompassState()) {
						displayInfo.SetAlpha(0.0f);
						actual_compass.SetDisplayInfo(displayInfo);
					}
					else {
						displayInfo.SetAlpha(100.0f);
						actual_compass.SetDisplayInfo(displayInfo);
						CompassHook::GetSingleton()->DoDamageCompass();
					}
				}
			}
			return _UpdateComp(a_this);
		}
	}

	bool CompassHook::HasCompassItem() const
	{

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
		if (player->GetItemCount(fitem::compass_new) > 0 || player->GetItemCount(fitem::compass_indestructible) > 0) {
			REX::INFO("has either compass or indestructible compass");
			return true;
		}

		const auto& inv = player->GetInventory();
		for (const auto& [object, pair] : inv) {
			if (object && object->HasKeywordByEditorID(MOD::kCompassIndestructibleKeyword)) {
				REX::INFO("has item with compass keyword, it is: {}", object->GetName());
				return true;
			}
		}
		return false;
	}

	bool CompassHook::ShouldShowCompass() const
	{
		REX::INFO("should show compass called");
		bool should_show = HasCompassItem() || !set::toggle_compass_check.GetValue();
		if (fitem::is_sotw_active) {
			if (fitem::sotw_perk->value == 0 || fitem::sotw_cheat->value == 0) {
				should_show = false;
			}
		}
		CompassHook::GetSingleton()->SetCompassState(should_show);
		return should_show;
	}

	bool CompassHook::HasIndestructibleCompass(RE::PlayerCharacter* a_player) const
	{
		if (a_player->GetItemCount(fitem::compass_indestructible) > 0) {
			return true;
		}

		const auto& inv = a_player->GetInventory();
		for (const auto& [object, pair] : inv) {
			if (object && object->HasKeywordByEditorID(MOD::kCompassIndestructibleKeyword)) {
				return true;
			}
		}

		return false;
	}

	RE::TESObjectMISC* CompassHook::GetCompassFromInventory(RE::PlayerCharacter* a_player) const
	{
		if (a_player->GetItemCount(fitem::compass_new) > 0) {
			return fitem::compass_new;
		}
		if (a_player->GetItemCount(fitem::compass_indestructible) > 0) {
			return fitem::compass_indestructible;
		}
		return nullptr;
	}

	void CompassHook::ShowCompassBreakMessage()
	{
		if (set::toggle_compass_notification.GetValue()) {
			std::string text = set::compass_break_message.GetValue();
			RE::SendHUDMessage::ShowHUDMessage(text.c_str(), nullptr, true);
		}
	}

	void CompassHook::DoDamageCompass()
	{

		RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

		if (!set::toggle_damage_compass.GetValue()) {
			compass_timer.Stop();
			return;
		}

		if (set::toggle_compass_check.GetValue()) {
			if(!compass_timer.IsRunning())
				compass_timer.Start();
		}

		if (!GetCompassState()) {
			compass_timer.Stop();
		}

		if (compass_timer.ElapsedSeconds() >= set::compass_damage_tick_time.GetValue()) {

			DurabilityTracker::GetSingleton()->DamageItem(fitem::compass_new, player, 1);
			compass_timer.Reset();
		}
	}

#pragma endregion CompassHook

}