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
namespace ChronoShift
{
  
  using MoveEffect = std::function<void(std::vector<FlexECS::Entity> targets)>;
  struct Move {
    std::string name; //use to access map of moves
    std::string description;
    
    int cost;

    bool is_target_player;
    int target_type;
    int target_count; //how many times you need to target
    //std::pair<MOVE_TARGET_TYPE, int>
    MoveEffect effect;
  };

  class MoveRegistry {
  public:
    MoveRegistry(MoveRegistry&) = delete;
    MoveRegistry& operator=(MoveRegistry &) = delete;

    static void RegisterMoves();

    static Move GetMove(std::string move_name);

  private:
    MoveRegistry();
    ~MoveRegistry();

    static std::unordered_map<std::string, Move> s_move_registry;  //this one used for the moves in the specific battle only
  };
}