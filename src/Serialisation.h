#pragma once
#include "hooks.h"
// Credits: https://github.com/colinswrath/BladeAndBlunt/blob/main/include/Serialization.h
namespace Serialisation
{
	static constexpr std::uint32_t SerializationVersion = 1;
	static constexpr std::uint32_t ID = 'SNRM';
	static constexpr std::uint32_t SerializationType = 'SMAC';


	inline void SaveCallback(SKSE::SerializationInterface* a_skse)
	{

		if (!a_skse->OpenRecord(SerializationType, SerializationVersion)) {
			logger::error("Failed to open navigation restriction record");
			return;
		} else {
			auto mapDamageCurr = Hooks::MapMenuEx::current_map_damage;
			auto mapTotoalDur = Hooks::MapMenuEx::total_durability_value_all_maps;
			auto compassVisibility = Hooks::MainUpdate::compass_visible;
			auto compassDamageVal = Hooks::MainUpdate::compass_damage_val;
			auto shouldDestroyComp = Hooks::MainUpdate::destroy;
			auto storedTime = Hooks::MainUpdate::passed_time;
			auto isHidden = Hooks::MainUpdate::hidden;
			if (!a_skse->WriteRecordData(mapDamageCurr)) {
				logger::error("Failed to write size of record data");
				return;
			}

			if (!a_skse->WriteRecordData(mapTotoalDur)) {
				logger::error("Failed to write size of record data");
				return;
			}

			if (!a_skse->WriteRecordData(compassVisibility)) {
				logger::error("Failed to write size of record data");
				return;
			}

			if (!a_skse->WriteRecordData(compassDamageVal)) {
				logger::error("Failed to write size of record data");
				return;
			}

			if (!a_skse->WriteRecordData(shouldDestroyComp)) {
				logger::error("Failed to write size of record data");
				return;
			}

			if (!a_skse->WriteRecordData(isHidden)) {
				logger::error("Failed to write size of record data");
				return;
			}

			if (!a_skse->WriteRecordData(storedTime)) {
				logger::error("Failed to write size of record data");
				return;
			}

			else { 
				logger::info("Serialized current map damage: {}", mapDamageCurr);
				logger::info("Serialized total durability: {}", mapTotoalDur);
				logger::info("Serialized compass visibility state: {}", compassVisibility ? "true" : "false");
				logger::info("Serialized compass should destroy: {}", shouldDestroyComp ? "true" : "false");
				logger::info("Serialized compass damage: {}", compassDamageVal);
				logger::info("Serialized is hidden: {}", isHidden ? "true" : "false");
				logger::info("Serialized passed time: {}", storedTime);
			}
		}
	}

	inline void LoadCallback(SKSE::SerializationInterface* a_skse)
	{
		std::uint32_t type;
		std::uint32_t version;
		std::uint32_t length;
		a_skse->GetNextRecordInfo(type, version, length);

		if (type != SerializationType) {
			return;
		}

		if (version != SerializationVersion) {
			logger::error("Unable to load data");
			return;
		}
		std::int16_t deserialisedMapDamageCurr;
		std::int16_t deserialisedMapTotalDur;
		bool deserialisedCompassVisibilityState;
		float deserialisedCompassDamageValue;
		bool deserialisedShouldDestroyCompass;
		bool deserialisedIsHidden;
		float deserialisedPassedTime;

		if (!a_skse->ReadRecordData(deserialisedMapDamageCurr)) {
			logger::error("Failed to load size");
			return;
		} 

		if (!a_skse->ReadRecordData(deserialisedMapTotalDur)) {
			logger::error("Failed to load size");
			return;
		}

		if (!a_skse->ReadRecordData(deserialisedCompassVisibilityState)) {
			logger::error("Failed to load size");
			return;
		}

		if (!a_skse->ReadRecordData(deserialisedCompassDamageValue)) {
			logger::error("Failed to load size");
			return;
		}

		if (!a_skse->ReadRecordData(deserialisedShouldDestroyCompass)) {
			logger::error("Failed to load size");
			return;
		}

		if (!a_skse->ReadRecordData(deserialisedIsHidden)) {
			logger::error("Failed to load size");
			return;
		}

		if (!a_skse->ReadRecordData(deserialisedPassedTime)) {
			logger::error("Failed to load size");
			return;
		}

		else {
			Hooks::MapMenuEx::current_map_damage = deserialisedMapDamageCurr;
			Hooks::MapMenuEx::total_durability_value_all_maps = deserialisedMapTotalDur;
			Hooks::MainUpdate::compass_visible = deserialisedCompassVisibilityState;
			Hooks::MainUpdate::compass_damage_val = deserialisedCompassDamageValue;
			Hooks::MainUpdate::destroy = deserialisedShouldDestroyCompass;
			Hooks::MainUpdate::passed_time = deserialisedPassedTime;

			logger::info("Deserialized current map damage: {}", deserialisedMapDamageCurr);
			logger::info("Deserialized total map durability: {}", deserialisedMapTotalDur);
			logger::info("Deserialized compass visibility state: {}", deserialisedCompassVisibilityState ? "true" : "false");
			logger::info("Deserialized compass damage: {}", deserialisedCompassDamageValue);
			logger::info("Deserialized compass should destroy: {}", deserialisedShouldDestroyCompass ? "true" : "false");
			logger::info("Deserialized is hidden: {}", deserialisedIsHidden ? "true" : "false");
			logger::info("Deserialized passed time: {}", deserialisedPassedTime);
		}
	}

	inline void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse)
	{
		Hooks::MapMenuEx::current_map_damage = 0;
		Hooks::MapMenuEx::total_durability_value_all_maps = 0;
		Hooks::MainUpdate::compass_visible = true;
		Hooks::MainUpdate::compass_damage_val = 0.0f;
		Hooks::MainUpdate::destroy = false;
		Hooks::MainUpdate::hidden = false;
		Hooks::MainUpdate::passed_time = 0.0f;
	}
}