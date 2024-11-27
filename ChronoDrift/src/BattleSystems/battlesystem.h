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

#define DELAY_TIME 2.f
#define SLOT_SUB_TARGET_COLOR Vector3{ 0.45f, 0.58f, 0.32f }
#define SLOT_MAIN_TARGET_COLOR Vector3{ 0.77f, 0.12f, 0.23f }

enum BattlePhase : int {
  BP_MOVE_DEATH_PROCESSION, // Checking whether anyone has died or not, and to remove dead characters from the battle scene
  BP_MOVE_EXECUTION,        // Executing Move Functions here (along with animation play in the future)
  BP_PROCESSING,            // Speed stack moving around, showing who's next
  BP_MOVE_SELECTION,        // Selection of Move and Targets
  BP_BATTLE_FINISH,         // Detect whether entire enemy or player team is dead (Display win or lose screens respectively)
  BP_STATUS_RUN,            // Run through all status effects characters have
  BP_MOVE_ANIMATION,        // Run animation of move selection
};

using namespace FlexEngine;

namespace ChronoDrift
{
  class BattleSystem {
  public:
    BattleSystem();
    ~BattleSystem();

    void AddCharacters(std::vector<FlexECS::Entity> characters);
    void SetUpBattleScene();
    //void SetUpBattleScene(int num_of_enemies, int num_of_players);

    void BeginBattle();
    void Update();

    size_t GetCharacterCount() { return m_characters.size(); };
    std::vector<FlexECS::Entity>& GetTurnOrder() { return m_characters; };
  private:
    int battle_phase = 0;
    static bool is_battle_finished;
    // Currently for my convenience but should be more useful and applicable
    // in the future if we got waves of enemies in one battle scene
    size_t enemies_displayed;
    size_t players_displayed;
    // Used for speed queue
    std::vector<FlexECS::Entity> m_characters;
    std::list<FlexECS::Entity> m_speed_queue_display;
    //std::vector<FlexECS::Entity> m_slots;
    // These 2 vectors are gonna replace m_slots instead cause god bless me
    std::vector<FlexECS::Entity> m_players;
    std::vector<FlexECS::Entity> m_enemies;

    std::pair<FlexECS::Entity, std::vector<FlexECS::Entity>> selected_targets;
    FlexECS::Scene::StringIndex selected_move = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");

    std::list<FlexECS::Entity> move_buttons;

    //BattleState m_battle_state;
    void UpdateSpeedStack();
    void RunCharacterStatus();
    void PlayerMoveSelection();
    void DeathProcession(std::vector<FlexECS::Entity> list_of_deaths);
    //void ExecuteMove(FlexECS::Scene::StringIndex move_id, std::vector<FlexECS::Entity> selected_targets);
    void ExecuteMove();
    void EndBattleScene();
  };
}