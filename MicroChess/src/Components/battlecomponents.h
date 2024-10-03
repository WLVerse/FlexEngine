/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      battlecomponents.h
\author    [90%] Rocky Sutarius
\par       rocky.sutarius@digipen.edu
\co-author [10%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file is where the design of the components utilized
           by the battle system, are written in.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoShift
{
  #if 0
  //Think about action as the "intent"
  class Action
  {
    FLX_REFL_SERIALIZABLE
  public:
    int id;   //id of move
    FlexECS::EntityID target; //slot 
  };

  class CharacterSpeed
  { FLX_REFL_SERIALIZABLE
  public:
    int base_speed;
    int current_speed;
  };

  class CharacterHealth
  { FLX_REFL_SERIALIZABLE
  public:
    int base_health;
    int current_health;
  };

  class IsPlayer
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_player = true;
  };

  class Weapon
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex description;
    FlexECS::Scene::StringIndex move_one;
    FlexECS::Scene::StringIndex move_two;
    FlexECS::Scene::StringIndex move_three;
    //int weapon_type;
  };

  class ChronoGear
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex ability_description;
    FlexECS::Scene::StringIndex main_stat;
    FlexECS::Scene::StringIndex sub_stat;
  };

  class StatModifiers
  { FLX_REFL_SERIALIZABLE
  public:
    int strength;
    int speed;
  };

  class Passive
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex description;
  };

  class PlayerSlot
  {
    FLX_REFL_SERIALIZABLE
  public:
    int slot_number;
    FlexECS::EntityID character;
  };

  class EnemySlot 
  { FLX_REFL_SERIALIZABLE
  public:
    int slot_number;
    FlexECS::EntityID character;
  };


  //class MoveSet
  //{
  //  FLX_REFL_SERIALIZABLE
  //public:
  //  FlexECS::Scene::StringIndex move_one;
  //  FlexECS::Scene::StringIndex move_two;
  //  FlexECS::Scene::StringIndex move_three;
  //};
  #endif

  #if 1
  //The Queued action (intent) of the character
  class Action
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex move_to_use;   //ID of move in movelist
  };

  //Attach these components to move entity
  /*class MoveID
  { FLX_REFL_SERIALIZABLE
  public:
    std::string move_name;  //string accessing MoveRegistry
  };
  
  class MoveDuration
  { FLX_REFL_SERIALIZABLE
  public:
    int move_duration;
  };
  
  class MoveCooldown
  { FLX_REFL_SERIALIZABLE
  public:
    int move_cooldown;
  };*/

  /*class MoveList 
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::EntityID move_one;
    FlexECS::EntityID move_two;
    FlexECS::EntityID move_three;
  };*/

  //Stats for battling character

  struct EffectValues
  { FLX_REFL_SERIALIZABLE
    int duration;
    int value;
  };

  class StatusEffects
  { FLX_REFL_SERIALIZABLE
  public:
    EffectValues strength;
    EffectValues speed;
  };

  //Battling components
  
  enum BattlePhase : int {
    BP_PLAYER_TURN,   //waiting for player input
    BP_ENEMY_TURN,
    BP_MOVE_DEATH_PROCESSION,
    BP_MOVE_EXECUTION, //Animation playing
    BP_PROCESSING,    //Speed stack moving around, showing who's next
    BP_MOVE_SELECTION,
  };
  
  class BattleState 
  { FLX_REFL_SERIALIZABLE
  public:
    int phase;
    FlexECS::EntityID active_character;
    int target_one = -1;
    int target_two = -1;
    int target_three = -1;
    int target_four = -1;
    int target_five = -1;

    int current_target_count = 0;
  };

  class BattleSlot
  { FLX_REFL_SERIALIZABLE
  public:
    int slot_number;
    FlexECS::Entity character;
  };

  class PlayerSlot
  { FLX_REFL_SERIALIZABLE
  public:
    int slot_number;
    FlexECS::EntityID character;
  };

  class EnemySlot
  {
    FLX_REFL_SERIALIZABLE
  public:
    int slot_number;
    FlexECS::EntityID character;
  };

  class MoveSelectionDisplay
  { FLX_REFL_SERIALIZABLE
  };

  class TurnOrderDisplay 
  { FLX_REFL_SERIALIZABLE
  };

  class MoveButton
  { FLX_REFL_SERIALIZABLE
  public:
    int move_number;
  };
  #endif

  enum WeaponType : int {
    WT_RANGED,    // Ranged Combat
    WT_MELEE,     // Close Combat
    WT_DEFENSIVE, // Defense
  };

  enum CharacterClass : int {
    CLASS_CROWD_CONTROL, // Crowd Control
    CLASS_TANK,          // Tank
    CLASS_DPS,           // DPS
  };

  enum MoveTargetType : int {
    MOVE_TARGET_SELF = 1,
    MOVE_TARGET_SINGLE = 2,
    MOVE_TARGET_SINGLE_ENEMY = 3,
    MOVE_TARGET_SINGLE_PLAYER = 4,
    MOVE_TARGET_DOUBLE = 5,
    MOVE_TARGET_TRIPLE = 6,
    MOVE_TARGET_ALL_ENEMY = 7,
    MOVE_TARGET_ALL_PLAYER = 8,
  };

  //struct Move {
  //  FLX_REFL_SERIALIZABLE
  //  FlexECS::Scene::StringIndex move_name;
  //  FlexECS::Scene::StringIndex move_description;
  //  int cost;         // Speed cost for each move
  //  int target_type;  // MoveTargetType Enum
  //  int target_count; // how many times you need to target
  //};

  struct Stat {
    FLX_REFL_SERIALIZABLE
    FlexECS::Scene::StringIndex stat_name;
    int base_stat_value;
    int current_stat_value;
  };

  class Character {
    FLX_REFL_SERIALIZABLE
  public:
    bool is_player = true;
    FlexECS::Scene::StringIndex character_name;
    FlexECS::Scene::StringIndex character_status;   // Character Status Effects Description
    int character_class;                            // Character Class and Weapon Type needs to be the same
    int base_health;
    int current_health;
    int base_speed;
    int current_speed;
    
    FlexECS::Scene::StringIndex weapon_name;
    FlexECS::Scene::StringIndex weapon_description;
    int weapon_type;
    FlexECS::Scene::StringIndex weapon_move_one;
    FlexECS::Scene::StringIndex weapon_move_two;
    FlexECS::Scene::StringIndex weapon_move_three;

    FlexECS::Scene::StringIndex chrono_gear_name;
    FlexECS::Scene::StringIndex chrono_gear_description;
    int chrono_gear_cooldown;

    FlexECS::Scene::StringIndex character_ultimate;
    FlexECS::Scene::StringIndex character_ultimate_cooldown;

    FlexECS::Scene::StringIndex character_passive_name;
    FlexECS::Scene::StringIndex character_passive_description;
  };
}