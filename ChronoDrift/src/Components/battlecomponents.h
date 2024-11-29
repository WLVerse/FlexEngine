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

namespace ChronoDrift
{
  #if 1
  //The Queued action (intent) of the character
  class Action
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex move_to_use;   //ID of move in movelist
  };

  #pragma region Status Effects Components
  class Burn
  { FLX_REFL_SERIALIZABLE
  public:
    int remaining_turns = 0;
    int damage_value = 0;
  };

  class Shock
  {
    FLX_REFL_SERIALIZABLE
  public:
    int remaining_turns = 0;
    int damage_value = 0;
  };

  class Shear
  {
    FLX_REFL_SERIALIZABLE
  public:
    int remaining_turns = 0;
    int damage_value = 0;
  };

  class Immunity
  {
    FLX_REFL_SERIALIZABLE
  public:
    int remaining_turns = 0;
  };

  class Attack_Buff
  {
      FLX_REFL_SERIALIZABLE
  public:
      int remaining_turns = 0;
  };

  class Attack_Debuff
  {
      FLX_REFL_SERIALIZABLE
  public:
      int remaining_turns = 0;
  };

  class Recovery
  {
    FLX_REFL_SERIALIZABLE
  public:
    int remaining_turns = 0;
    int heal_value = 0;
  };

  class Stun
  {
    FLX_REFL_SERIALIZABLE
  public:
    int remaining_turns = 0;
  };
  #pragma endregion

  //Battling components
  
  //enum BattlePhase : int {
  //  BP_MOVE_DEATH_PROCESSION, // Checking whether anyone has died or not, and to remove dead characters from the battle scene
  //  BP_MOVE_EXECUTION,        // Executing Move Functions here (along with animation play in the future)
  //  BP_PROCESSING,            // Speed stack moving around, showing who's next
  //  BP_MOVE_SELECTION,        // Selection of Move and Targets
  //  BP_BATTLE_FINISH,         // Detect whether entire enemy or player team is dead (Display win or lose screens respectively)
  //  BP_STATUS_RUN,            // Run through all status effects characters have
  //  BP_MOVE_ANIMATION,        // Run animation of move selection
  //};
  
  /*class BattleState 
  { FLX_REFL_SERIALIZABLE
  public:
    int phase;
  };*/

  class BattleSlot
  { FLX_REFL_SERIALIZABLE
  public:
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
    MOVE_TARGET_SELF = -2,
    MOVE_TARGET_ALL = -1,
    MOVE_TARGET_SINGLE = 1,
    MOVE_TARGET_DOUBLE = 2,
    MOVE_TARGET_TRIPLE = 3,
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
    FlexECS::Scene::StringIndex character_name = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    // Character Status Effects Description
    FlexECS::Scene::StringIndex character_status = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");   
    // Character Class and Weapon Type needs to be the same
    int character_class = 0;
    int base_health = 0;
    int current_health = 0;
    int base_speed = 0;
    int current_speed = 0;
    
    FlexECS::Scene::StringIndex weapon_name = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    FlexECS::Scene::StringIndex weapon_description = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    int weapon_type = 0;
    FlexECS::Scene::StringIndex weapon_move_one = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    FlexECS::Scene::StringIndex weapon_move_two = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    FlexECS::Scene::StringIndex weapon_move_three = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");

    FlexECS::Scene::StringIndex chrono_gear_name = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    FlexECS::Scene::StringIndex chrono_gear_description = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    int chrono_gear_cooldown = 0;

    FlexECS::Scene::StringIndex character_ultimate = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    FlexECS::Scene::StringIndex character_ultimate_cooldown = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");

    FlexECS::Scene::StringIndex character_passive_name = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    FlexECS::Scene::StringIndex character_passive_description = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
  };

  class CharacterMovementSprites {
    FLX_REFL_SERIALIZABLE
  public:
    int idle_cols = 1;
    FlexECS::Scene::StringIndex idle_movement = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    int right_cols = 1;
    FlexECS::Scene::StringIndex right_movement = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    int left_cols = 1;
    FlexECS::Scene::StringIndex left_movement = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    int up_cols = 1;
    FlexECS::Scene::StringIndex up_movement = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    int down_cols = 1;
    FlexECS::Scene::StringIndex down_movement = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");

  };

  void RegisterBattleComponents();
}