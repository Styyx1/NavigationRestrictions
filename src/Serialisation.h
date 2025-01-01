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
			if (!a_skse->WriteRecordData(mapDamageCurr)) {
				logger::error("Failed to write size of record data");
				return;
			}

			if (!a_skse->WriteRecordData(mapTotoalDur)) {
				logger::error("Failed to write size of record data");
				return;
			}
			else { 
				logger::info("Serialized current map damage: {}", mapDamageCurr);
				logger::info("Serialized total durability: {}", mapTotoalDur);
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

		if (!a_skse->ReadRecordData(deserialisedMapDamageCurr)) {
			logger::error("Failed to load size");
			return;
		} 

		if (!a_skse->ReadRecordData(deserialisedMapTotalDur)) {
			logger::error("Failed to load size");
			return;
		} 

		else {
			Hooks::MapMenuEx::current_map_damage = deserialisedMapDamageCurr;
			Hooks::MapMenuEx::total_durability_value_all_maps = deserialisedMapTotalDur;

			logger::info("Deserialized current map damage: {}", deserialisedMapDamageCurr);
			logger::info("Deserialized total map durability: {}", deserialisedMapTotalDur);
		}
	}

	inline void RevertCallback([[maybe_unused]] SKSE::SerializationInterface* a_skse)
	{
		Hooks::MapMenuEx::current_map_damage = 0;
		Hooks::MapMenuEx::total_durability_value_all_maps = 0;
	}
}