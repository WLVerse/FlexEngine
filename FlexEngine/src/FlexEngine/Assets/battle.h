#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "assetkey.h"

namespace FlexEngine
{
  namespace Asset
  {

    // TODO: use the flxfmt formatter
    // TODO: save any changes to file
    struct __FLX_API Battle
    {
      File& metadata;

      AssetKey drifter_slot_one = "None";
      AssetKey drifter_slot_two = "None";

      AssetKey enemy_slot_one = "None";
      AssetKey enemy_slot_two = "None";
      AssetKey enemy_slot_three = "None";
      AssetKey enemy_slot_four = "None";
      AssetKey enemy_slot_five = "None";

      int boss_battle = 0; // 0 = false, 1-5 = true, pointing out which enemy slot is the boss
      bool is_tutorial = false;

      // helper vector functions
      std::vector<AssetKey*> GetDrifterSlots()
      {
        return { &drifter_slot_one, &drifter_slot_two };
      }
      std::vector<AssetKey*> GetEnemySlots()
      {
        return { &enemy_slot_one, &enemy_slot_two, &enemy_slot_three, &enemy_slot_four, &enemy_slot_five };
      }
      std::vector<AssetKey*> GetAllSlots()
      {
        auto slots = GetDrifterSlots();
        auto enemy_slots = GetEnemySlots();
        slots.insert(slots.end(), enemy_slots.begin(), enemy_slots.end());
        return slots;
      }

      Battle(File& _metadata);
    };

  } // namespace Asset
} // namespace FlexEngine
