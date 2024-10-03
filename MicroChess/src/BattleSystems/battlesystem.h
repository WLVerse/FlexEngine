/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      battlesystem.h
\author    [100%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file is where the functions utilized by the
           battle system to execute the battle scene, is declared
           at.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <FlexEngine.h>

#include "CharacterMoves/moves.h"
using namespace FlexEngine;

namespace ChronoShift
{
  class BattleSystem {
  public:
    BattleSystem();
    ~BattleSystem();

    void InitializeBattleSlots();
    void AddCharacter(FlexECS::Entity character, int position);

    void BeginBattle();
    void Update();

    size_t GetCharacterCount() { return m_characters.size(); };
    std::list<FlexECS::Entity>& GetTurnOrder() { return m_characters; };
  private:
    std::list<FlexECS::Entity> m_characters;
    std::vector<FlexECS::Entity> m_enemies;
    std::vector<FlexECS::Entity> m_players;
    //std::list<FlexECS::Entity> m_speedstack;
    std::array<FlexECS::Entity, 9> m_slots; //player takes up first 4, enemy last 5

    //BattleState m_battle_state;
    void UpdateSpeedStack();
    void PlayerMoveSelection();
    void DeathProcession();
    void ExecuteMove(FlexECS::Scene::StringIndex move_id, std::vector<int> selected_targets);
  };

}
