#include "pch.h"

#include "battle.h"

namespace FlexEngine
{

  namespace Asset
  {

    Battle::Battle(File& _metadata)
      : metadata(_metadata)
    {
      // parse the file
      // example
      // DrifterSlotOne: "/data/Renko.flxcharacter"
      // DrifterSlotTwo: "/data/Grace.flxcharacter"
      // EnemySlotOne: "/data/Enemy1.flxcharacter"
      // EnemySlotTwo: "/data/Enemy2.flxcharacter"
      // EnemySlotThree: "/data/Jack.flxcharacter"
      // EnemySlotFour: "/data/Enemy2.flxcharacter"
      // EnemySlotFive: "/data/Enemy1.flxcharacter"
      // BossBattle: 3
      std::stringstream ss(metadata.Read());
      std::string line;
      while (std::getline(ss, line))
      {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "DrifterSlotOne:")
          iss >> drifter_slot_one;
        else if (key == "DrifterSlotTwo:")
          iss >> drifter_slot_two;
        else if (key == "EnemySlotOne:")
          iss >> enemy_slot_one;
        else if (key == "EnemySlotTwo:")
          iss >> enemy_slot_two;
        else if (key == "EnemySlotThree:")
          iss >> enemy_slot_three;
        else if (key == "EnemySlotFour:")
          iss >> enemy_slot_four;
        else if (key == "EnemySlotFive:")
          iss >> enemy_slot_five;
        else if (key == "BossBattle:")
          iss >> boss_battle;
        else if (key == "BattleNum:")
            iss >> battle_num;
      }
    }

  } // namespace Asset

} // namespace FlexEngine
