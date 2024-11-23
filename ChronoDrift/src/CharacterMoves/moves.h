/* Start Header
*****************************************************************/
/*!
\file      moves.h
\author    [97%] Rocky Sutarius
\par			 rocky.sutarius@digipen.edu
\co-author [3%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file contains the declaration of the move
           and move registry class to create and store
           moves.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include <FlexEngine.h>

using namespace FlexEngine;
namespace ChronoDrift
{
  using MoveFunction = std::function<void(std::vector<FlexECS::Entity> targets, int value)>;
  using EffectFunction = std::function<void(std::vector<FlexECS::Entity> targets, int value, int duration)>;

  struct MoveExecution {
    int value;
    MoveFunction move_function;
  };

  struct StatusEffectApplication {
    int duration;
    int value;
    EffectFunction effect_function;
  };

  struct Move {
    std::string name; //use to access map of moves
    std::string description;

    int cost = 0;

    bool is_target_enemy = true;
    int target_type = 0;

    std::vector<MoveExecution> move_function_container;
    std::vector<StatusEffectApplication> sea_function_container;
  };

  class MoveRegistry {
  public:
    MoveRegistry(MoveRegistry&) = delete;
    MoveRegistry& operator=(MoveRegistry&) = delete;

    static void RegisterMoveFunctions();
    static void RegisterStatusFunctions();

    static Move GetMove(std::string move_name);

  private:
    MoveRegistry();
    ~MoveRegistry();

    static std::unordered_map<std::string, MoveFunction> s_move_function_registry;
    static std::unordered_map<std::string, EffectFunction> s_status_function_registry;
    static inline std::unordered_map<std::string, int> break_down_numerical_string = {
      {"SINGLE", 1},
      {"DOUBLE", 2},
      {"TRIPLE", 3},
      {"QUARUPLE", 4},
      {"QUINTUPLE", 5},
      {"SEXTUPLE", 6},
      {"ALL", -1},
      {"SELF", -2},
    };
  };
}