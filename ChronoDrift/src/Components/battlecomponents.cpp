/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      battlecomponents.cpp
\author    [75%] Rocky Sutarius
\par       rocky.sutarius@digipen.edu
\co-author [25%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file is where the classes are registered as
           components to be utilized by the battle system 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "battlecomponents.h"
#ifndef GAME
#include "Editor/componentviewer.h"
#endif 
namespace ChronoDrift 
{
  #pragma region Character Components
  FLX_REFL_REGISTER_START(Action)
    FLX_REFL_REGISTER_PROPERTY(move_to_use)
  FLX_REFL_REGISTER_END;
  #pragma endregion

  #pragma region Status Effect Components
  FLX_REFL_REGISTER_START(Burn)
    FLX_REFL_REGISTER_PROPERTY(remaining_turns)
    FLX_REFL_REGISTER_PROPERTY(damage_value)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Shock)
    FLX_REFL_REGISTER_PROPERTY(remaining_turns)
    FLX_REFL_REGISTER_PROPERTY(damage_value)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Shear)
    FLX_REFL_REGISTER_PROPERTY(remaining_turns)
    FLX_REFL_REGISTER_PROPERTY(damage_value)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Immunity)
      FLX_REFL_REGISTER_PROPERTY(remaining_turns)
      FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Attack_Buff)
      FLX_REFL_REGISTER_PROPERTY(remaining_turns)
      FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Attack_Debuff)
      FLX_REFL_REGISTER_PROPERTY(remaining_turns)
      FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Stun)
    FLX_REFL_REGISTER_PROPERTY(remaining_turns)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Recovery)
    FLX_REFL_REGISTER_PROPERTY(remaining_turns)
    FLX_REFL_REGISTER_PROPERTY(heal_value)
  FLX_REFL_REGISTER_END;
  #pragma endregion

  #pragma region Battle Components
  //FLX_REFL_REGISTER_START(BattleState)
    //FLX_REFL_REGISTER_PROPERTY(phase)
    //FLX_REFL_REGISTER_PROPERTY(active_character)
    //FLX_REFL_REGISTER_PROPERTY(target_one)
    //FLX_REFL_REGISTER_PROPERTY(target_two)
    //FLX_REFL_REGISTER_PROPERTY(target_three)
    //FLX_REFL_REGISTER_PROPERTY(target_four)
    //FLX_REFL_REGISTER_PROPERTY(target_five)
    //FLX_REFL_REGISTER_PROPERTY(current_target_count)
    //FLX_REFL_REGISTER_PROPERTY(selected_targets)
  //FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(BattleSlot)
    FLX_REFL_REGISTER_PROPERTY(character)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(PlayerSlot)
    FLX_REFL_REGISTER_PROPERTY(slot_number)
    FLX_REFL_REGISTER_PROPERTY(character)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(EnemySlot)
    FLX_REFL_REGISTER_PROPERTY(slot_number)
    FLX_REFL_REGISTER_PROPERTY(character)
  FLX_REFL_REGISTER_END;
  
  FLX_REFL_REGISTER_START(TurnOrderDisplay)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(MoveButton)
    FLX_REFL_REGISTER_PROPERTY(move_number)
  FLX_REFL_REGISTER_END;

  #pragma endregion

  FLX_REFL_REGISTER_START(Character)
    FLX_REFL_REGISTER_PROPERTY(is_player)
    FLX_REFL_REGISTER_PROPERTY(character_name)
    FLX_REFL_REGISTER_PROPERTY(character_status) // Character Status Effects Description
    FLX_REFL_REGISTER_PROPERTY(character_class)  // Character Class and Weapon Type needs to be the same
    FLX_REFL_REGISTER_PROPERTY(base_health)
    FLX_REFL_REGISTER_PROPERTY(current_health)
    FLX_REFL_REGISTER_PROPERTY(base_speed)
    FLX_REFL_REGISTER_PROPERTY(current_speed)

    FLX_REFL_REGISTER_PROPERTY(weapon_name)
    FLX_REFL_REGISTER_PROPERTY(weapon_description)
    FLX_REFL_REGISTER_PROPERTY(weapon_type)
    FLX_REFL_REGISTER_PROPERTY(weapon_move_one)
    FLX_REFL_REGISTER_PROPERTY(weapon_move_two)
    FLX_REFL_REGISTER_PROPERTY(weapon_move_three)

    FLX_REFL_REGISTER_PROPERTY(chrono_gear_name)
    FLX_REFL_REGISTER_PROPERTY(chrono_gear_description)
    FLX_REFL_REGISTER_PROPERTY(chrono_gear_cooldown)

    FLX_REFL_REGISTER_PROPERTY(character_ultimate)
    FLX_REFL_REGISTER_PROPERTY(character_ultimate_cooldown)

    FLX_REFL_REGISTER_PROPERTY(character_passive_name)
    FLX_REFL_REGISTER_PROPERTY(character_passive_description)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(CharacterMovementSprites)
    FLX_REFL_REGISTER_PROPERTY(idle_cols)
    FLX_REFL_REGISTER_PROPERTY(idle_movement)
    FLX_REFL_REGISTER_PROPERTY(right_cols)
    FLX_REFL_REGISTER_PROPERTY(right_movement)
    FLX_REFL_REGISTER_PROPERTY(left_cols)
    FLX_REFL_REGISTER_PROPERTY(left_movement)
    FLX_REFL_REGISTER_PROPERTY(up_cols)
    FLX_REFL_REGISTER_PROPERTY(up_movement)
    FLX_REFL_REGISTER_PROPERTY(down_cols)
    FLX_REFL_REGISTER_PROPERTY(down_movement)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Stat)
    FLX_REFL_REGISTER_PROPERTY(stat_name)
    FLX_REFL_REGISTER_PROPERTY(base_stat_value)
    FLX_REFL_REGISTER_PROPERTY(current_stat_value)
  FLX_REFL_REGISTER_END;


  /*
  Editor viewer
  */
  #ifndef GAME
  COMPONENT_VIEWER_START(CharacterMovementSprites)
    COMPONENT_VIEWER_DRAG_INT(idle_cols)
    COMPONENT_VIEWER_TEXTURE_PATH(idle_movement)
    COMPONENT_VIEWER_DRAG_INT(right_cols)
    COMPONENT_VIEWER_TEXTURE_PATH(right_movement)
    COMPONENT_VIEWER_DRAG_INT(left_cols)
    COMPONENT_VIEWER_TEXTURE_PATH(left_movement)
    COMPONENT_VIEWER_DRAG_INT(up_cols)
    COMPONENT_VIEWER_TEXTURE_PATH(up_movement)
    COMPONENT_VIEWER_DRAG_INT(down_cols)
    COMPONENT_VIEWER_TEXTURE_PATH(down_movement)
  COMPONENT_VIEWER_END(CharacterMovementSprites)

  COMPONENT_VIEWER_START(Character)
    COMPONENT_VIEWER_BOOL(is_player)
    COMPONENT_VIEWER_EDITABLE_STRING(character_name)
    COMPONENT_VIEWER_EDITABLE_STRING(character_status)
    COMPONENT_VIEWER_DRAG_INT(character_class)
    COMPONENT_VIEWER_DRAG_INT(base_health)
    COMPONENT_VIEWER_DRAG_INT(current_health)
    COMPONENT_VIEWER_DRAG_INT(base_speed)
    COMPONENT_VIEWER_DRAG_INT(current_speed)

    COMPONENT_VIEWER_EDITABLE_STRING(weapon_name)
    COMPONENT_VIEWER_EDITABLE_STRING(weapon_description)
    COMPONENT_VIEWER_DRAG_INT(weapon_type)
    COMPONENT_VIEWER_EDITABLE_STRING(weapon_move_one)
    COMPONENT_VIEWER_EDITABLE_STRING(weapon_move_two)
    COMPONENT_VIEWER_EDITABLE_STRING(weapon_move_three)

    COMPONENT_VIEWER_EDITABLE_STRING(chrono_gear_name)
    COMPONENT_VIEWER_EDITABLE_STRING(chrono_gear_description)
    COMPONENT_VIEWER_DRAG_INT(chrono_gear_cooldown)

    COMPONENT_VIEWER_EDITABLE_STRING(character_ultimate)
    COMPONENT_VIEWER_EDITABLE_STRING(character_ultimate_cooldown)

    COMPONENT_VIEWER_EDITABLE_STRING(character_passive_name)
    COMPONENT_VIEWER_EDITABLE_STRING(character_passive_description)
  COMPONENT_VIEWER_END(Character)

  void RegisterBattleComponents()
  {
    REGISTER_COMPONENT_VIEWER(Character);
    REGISTER_COMPONENT_VIEWER(CharacterMovementSprites);
  }
  #endif
}