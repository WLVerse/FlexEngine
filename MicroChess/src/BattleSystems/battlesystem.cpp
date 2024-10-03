/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      battlesystem.cpp
\author    [100%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file is where the functions utilized by the
           battle system to execute the battle scene, is defined
           at.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "battlesystem.h"

#include "States.h"
#include "Layers.h"

#include "Components/battlecomponents.h"
#include "Components/physics.h"
#include "Components/rendering.h"

#include "BattleSystems/turnorderdisplay.h"
#include "BattleSystems/targetingsystem.h"

#include "Physics/box2d.h"
#include "Renderer/sprite2d.h"

// temporary
#include "CharacterPrefab/characterprefab.h"


namespace ChronoShift {


  //Sorting functions
  struct SortLowestSpeed
  {
    bool operator()(FlexECS::Entity e1, FlexECS::Entity e2) {
      return e1.GetComponent<Character>()->current_speed < e2.GetComponent<Character>()->current_speed;
    }
  };


  BattleSystem::BattleSystem()
  {
  }
  BattleSystem::~BattleSystem()
  {
  }
  void BattleSystem::InitializeBattleSlots()
  {
    auto scene = FlexECS::Scene::GetActiveScene();
    const Vector3 color_player_slot = { 0.45f, 0.58f, 0.32f };
    const Vector3 color_enemy_slot = { 0.77f, 0.12f, 0.23f };

    for (int i = 0; i < m_slots.size(); i++)
    {
      bool is_player_slot = (i < 4);
      Vector2 position;
      if (is_player_slot) position = { 200.f + 120.f * i, 600.f };
      else                position = { 700.f + 120.f * (i - 4), 200.f };

      FlexECS::Entity slot = FlexECS::Scene::CreateEntity();
      slot.AddComponent<BattleSlot>({ i, FlexECS::Entity::Null });
      slot.AddComponent<OnHover>({});
      slot.AddComponent<OnClick>({});
      slot.AddComponent<IsActive>({ false });
      slot.AddComponent<Position>({ position });
      slot.AddComponent<Scale>({ { 100,100 } });
      slot.AddComponent<ZIndex>({ 9 });
      slot.AddComponent<Rotation>({});
      slot.AddComponent<Transform>({});
      slot.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"()"),
        is_player_slot ? color_player_slot : color_enemy_slot,
        Vector3::Zero,
        Vector3::One,
        Renderer2DProps::Alignment_Center
       });
      slot.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
      m_slots[i] = slot;
    }

    for (int i = 0; i < 4; i++)
    {
      Vector2 position = { 900.f, 450.f + (80.f * i) };

      FlexECS::Entity move_button = FlexECS::Scene::CreateEntity();
      move_button.AddComponent<MoveButton>({ i });
      move_button.AddComponent<OnHover>({});
      move_button.AddComponent<OnClick>({});
      move_button.AddComponent<IsActive>({ false });
      move_button.AddComponent<Position>({ position });
      move_button.AddComponent<Scale>({ { 350,70 } });
      move_button.AddComponent<ZIndex>({ 9 });
      move_button.AddComponent<Rotation>({});
      move_button.AddComponent<Transform>({});
      move_button.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"()"),
        Vector3::One,
        Vector3::Zero,
        Vector3::One - Vector3(0.10f * i, 0.10f * i , 0.10f * i),
        Renderer2DProps::Alignment_Center
       });
      move_button.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
    }
  }

  void BattleSystem::AddCharacter(FlexECS::Entity character, int position)
  {
    m_characters.push_back(character);
    if (character.GetComponent<Character>()->is_player) m_players.push_back(character);
    else m_enemies.push_back(character);
    m_slots[position].GetComponent<BattleSlot>()->character = character;
    m_slots[position].GetComponent<IsActive>()->is_active = true;
  }

  void BattleSystem::BeginBattle()
  {
    ResetCharacters();
    /*for (FlexECS::Entity entity : m_characters)
    {
      m_speedstack.push_back(entity);
    }*/
    m_characters.sort(SortLowestSpeed());

    auto scene = FlexECS::Scene::GetActiveScene();
    std::cout << "Beginning Stack Order:\nName / HP / Speed \n";
    for (auto& entity : m_characters)
    {
      auto cw = entity.GetComponent<Character>()->weapon_name;
      std::cout << scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name)
        << "  HP: " << entity.GetComponent<Character>()->current_health
        << "  Spd: " << entity.GetComponent<Character>()->current_speed
        << " Weapon: " << scene->Internal_StringStorage_Get(cw);
      if (entity.GetComponent<Character>()->is_player) {
        auto cg = entity.GetComponent<Character>()->chrono_gear_description;
        std::cout << " Chrono Gear: " << scene->Internal_StringStorage_Get(cg);
      }
      std::cout << std::endl;
    }
    std::cout << "\n";

    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
  }

  void BattleSystem::UpdateSpeedStack()
  {
    auto scene = FlexECS::Scene::GetActiveScene();
    int speed_to_decrease = m_characters.front().GetComponent<Character>()->current_speed;
    //if (speed_to_decrease != 0) {
    FlexECS::Scene::StringIndex front_character = m_characters.front().GetComponent<Character>()->character_name;
    std::cout << scene->Internal_StringStorage_Get(front_character) << " turn:" << std::endl;
    for (auto& entity : m_characters)
    {
      entity.GetComponent<Character>()->current_speed -= speed_to_decrease;
      std::cout << scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name)
        << "  HP: " << entity.GetComponent<Character>()->current_health
        << "  Spd: " << entity.GetComponent<Character>()->current_speed << std::endl;
    }
    std::cout << "\n";

    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    battle_state.GetComponent<BattleState>()->active_character = m_characters.front();
    battle_state.GetComponent<BattleState>()->phase = BP_MOVE_SELECTION;
      /*if (m_characters.front().GetComponent<Character>()->is_player) {
        battle_state.GetComponent<BattleState>()->phase = BP_PLAYER_TURN;
      }
      else {
        battle_state.GetComponent<BattleState>()->phase = BP_ENEMY_TURN;
      }*/
    //}
  }

  void BattleSystem::Update()
  {
    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    int battle_phase = battle_state.GetComponent<BattleState>()->phase;
    if (battle_phase == BP_PROCESSING) {
      UpdateSpeedStack();
    }
    else if (battle_phase == BP_MOVE_SELECTION) {
      PlayerMoveSelection();
    }
    else if (battle_phase == BP_MOVE_DEATH_PROCESSION) {
      DeathProcession();
    }
  }

  void BattleSystem::PlayerMoveSelection()
  {
    static int selected_num;
    std::array<FlexECS::Scene::StringIndex, 3> character_moves{
      m_characters.front().GetComponent<Character>()->weapon_move_one,
      m_characters.front().GetComponent<Character>()->weapon_move_two,
      m_characters.front().GetComponent<Character>()->weapon_move_three,
    };
    // Reset all the slots colors
    for (int i = 0; i < 9; i++) {
      m_slots[i].GetComponent<Sprite>()->color = { 0.45f, 0.58f, 0.32f }; // player slot color
      if (i > 3) m_slots[i].GetComponent<Sprite>()->color = { 0.77f, 0.12f, 0.23f }; // enemy slot color
    }

    static int move_decision = -1; // variable storing move selection
    if (!m_characters.front().GetComponent<Character>()->is_player) {
      // Random move selection for enemies
      move_decision = Range(0, 2).Get();
    }
    else {
      for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, MoveButton>()) {
        entity.GetComponent<IsActive>()->is_active = true;
      }
      // move selection system by mouse click for players
      GetMoveSelection();
      for (auto& entity : FlexECS::Scene::GetActiveScene()->View<OnClick, MoveButton>())
      {
        auto& click_status = entity.GetComponent<OnClick>()->is_clicked;
        if (click_status == true) {
          move_decision = entity.GetComponent<MoveButton>()->move_number;
          Move player_move = MoveRegistry::GetMove(
            FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(
              character_moves[move_decision]));
          std::cout << "Move Selected: " << player_move.name << std::endl;
          selected_num = (!player_move.is_target_player * 4);
        }
      }
    }
    if (move_decision == -1) return;
    Move selected_move = MoveRegistry::GetMove(
            FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(
              character_moves[move_decision]));

    // WIP: To include mouse click as well when selecting targets
    /*if (target_count < move.target_count) {
    GetTargetSelection();

    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<OnClick, BattleSlot>())
    {
      auto& click_status = entity.GetComponent<OnClick>()->is_clicked;
      if (click_status == true)
      {
        int selection = entity.GetComponent<BattleSlot>()->slot_number;
        FlexECS::Entity target = entity.GetComponent<BattleSlot>()->character;
        FlexECS::Scene::StringIndex target_name = target.GetComponent<Character>()->character_name;
        std::cout << FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(target_name) << " has been selected.\n";
        target_count += 1;
        switch (target_count) {
        case 1:
          battle_state.GetComponent<BattleState>()->target_one = selection;
          break;
        case 2:
          battle_state.GetComponent<BattleState>()->target_two = selection;
          break;
        case 3:
          battle_state.GetComponent<BattleState>()->target_three = selection;
          break;
        case 4:
          battle_state.GetComponent<BattleState>()->target_four = selection;
          break;
        case 5:
          battle_state.GetComponent<BattleState>()->target_five = selection;
          break;
        }
      }
    }*/
    std::vector<int> result;
    if (m_characters.front().GetComponent<Character>()->is_player) {
      static int adjacent = 1;

      // Identify whether target is enemy or team
      // number of targets
      // selection of adjecent targets should only apply when target count is less than 5
      size_t no_of_targets = 0;
      if (selected_move.is_target_player) {
        // means is player target
        no_of_targets = m_players.size();
      }
      else {
        // means is enemy target
        no_of_targets = m_enemies.size();
      }

      if (selected_move.target_count < no_of_targets) {
        if (Input::GetKeyDown(GLFW_KEY_A) && selected_num > (!selected_move.is_target_player * 4)) {
          selected_num--;
        }
        else if (Input::GetKeyDown(GLFW_KEY_D) && selected_num < ((!selected_move.is_target_player * 4) + no_of_targets - 1)) {
          selected_num++;
        }
        if ((selected_num + 1) > ((!selected_move.is_target_player * 4) + no_of_targets - 1)) {
          adjacent = -1;
        }
        else if ((selected_num - 1) < (!selected_move.is_target_player * 4)) {
          adjacent = 1;
        }
        if (selected_move.target_count == 1) {
          m_slots[selected_num].GetComponent<Sprite>()->color = { 0.35f, 0.68f, 0.12f };
        }
        else if (selected_move.target_count == 2) {
          m_slots[selected_num].GetComponent<Sprite>()->color = { 0.35f, 0.68f, 0.12f };
          m_slots[selected_num + adjacent].GetComponent<Sprite>()->color = { 0.35f, 0.68f, 0.12f };
        }
        else {
          for (int j = (selected_num - 1); j < (selected_num + 1); j++) {
            m_slots[j].GetComponent<Sprite>()->color = { 0.35f, 0.68f, 0.12f };
          }
        }
      }
      else {
        for (int i = selected_num; i < (selected_num + no_of_targets); i++) {
          m_slots[i].GetComponent<Sprite>()->color = { 0.35f, 0.68f, 0.12f };
        }
      }

      if (Input::GetKeyDown(GLFW_KEY_SPACE)) {
        switch (selected_move.target_count) {
        case 3:
          result.push_back(selected_num + 1);
          result.push_back(selected_num - 1);
          break;
        case 2:
          result.push_back(selected_num + adjacent);
          break;
        }
        result.push_back(selected_num);
      }
    }
    else {
      int min = 0, max = 0;
      if (selected_move.is_target_player) {
        min = 0;
        max = static_cast<int>(m_players.size() - 1);
      }
      else {
        min = static_cast<int>(m_players.size() + 1);
        max = static_cast<int>(m_characters.size() - 1);
      }
      for (int i = 0; i < selected_move.target_count; i++) {
        result.push_back(Range(min, max).Get());
      }
    }
    if (result.empty()) return;

    int final_decision = move_decision;
    move_decision = -1;
    std::vector final_result = result;
    result.clear();
    ExecuteMove(character_moves[final_decision], final_result);
  }

  void BattleSystem::ExecuteMove(FlexECS::Scene::StringIndex move_id, std::vector<int> selected_targets)
  {
    //get the move user
    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    FlexECS::Entity user = battle_state.GetComponent<BattleState>()->active_character;
    Move move = MoveRegistry::GetMove(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(move_id));
    std::vector<FlexECS::Entity> targets;

    if (move.target_type == MOVE_TARGET_SINGLE)
    {
      for (int i = 0; i < selected_targets.size(); i++) {
        targets.insert(targets.begin(), static_cast<FlexECS::Entity>(m_slots[selected_targets[i]].GetComponent<BattleSlot>()->character));
      }
    }
    // WIP: To include execution of other moves of different target types
    /*else if (move.target_type == MOVE_TARGET_SELF) {
      targets.push_back(user);
    }
    else if (move.target_type == MOVE_TARGET_ALL_ENEMY) {
      for (FlexECS::Entity target : m_characters)
      {
        if (!target.GetComponent<Character>()->is_player)
          targets.push_back(target);
      }
    }
    else if (move.target_type == MOVE_TARGET_ALL_PLAYER) {
      for (FlexECS::Entity target : m_characters)
      {
        if (target.GetComponent<Character>()->is_player)
          targets.push_back(target);
      }
    }*/

    //execute move
    move.effect(targets);
    user.GetComponent<Character>()->current_speed += move.cost + user.GetComponent<Character>()->base_speed;
    user.GetComponent<Action>()->move_to_use = FlexECS::Entity::Null;
    m_characters.sort(SortLowestSpeed());

    
    auto scene = FlexECS::Scene::GetActiveScene();
    //Display Character Move and target selection
    std::cout << scene->Internal_StringStorage_Get(user.GetComponent<Character>()->character_name)
      << " executed " << move.name
      << " on ";
    for (int i = 0; i < targets.size(); ++i) {
      std::cout << scene->Internal_StringStorage_Get(targets[i].GetComponent<Character>()->character_name);
      if ((i + 1) < targets.size()) std::cout << " and ";
    }
    std::cout << std::endl;
    // Display Status of alive Characters
    for (auto& entity : m_characters)
    {
      if (entity.GetComponent<Character>()->current_health > 0) {
        std::cout << scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name)
          << "  HP: " << entity.GetComponent<Character>()->current_health
          << "  Spd: " << entity.GetComponent<Character>()->current_speed << std::endl;
      }
    }
    // Display Characters that have died
    for (auto& entity : m_characters)
    {
      if (entity.GetComponent<Character>()->current_health <= 0) {
        std::cout << scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name)
          << " has been killed by " << scene->Internal_StringStorage_Get(user.GetComponent<Character>()->character_name)
          << "'s " << move.name << std::endl;
      }
    }
    std::cout << "\n";
    battle_state.GetComponent<BattleState>()->current_target_count = 0;
    battle_state.GetComponent<BattleState>()->active_character = FlexECS::Entity::Null;
    battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
  }
  // WIP: Death Procession kind of works:
  // 1. need to disable IsActive component for the character entity itself so that the sprite won't appear
  // 2. Affects battlesystem (causes crashes)
  // - Need to find a better way to set the starting index when selecting move,
  //   just setting to 0 for player and 4 for enemies is not enough
  // - If enemy killed is not the first one, current targetting system has no
  //   issues. However, when attempting to execute the move, instant crash (issue has not been identified)
  void BattleSystem::DeathProcession() {
    FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->View<BattleState>()[0];
    int to_delete = 0;
    FlexECS::Entity entity_to_delete = FlexECS::Entity::Null;
    for (auto& entity : m_characters) {
      if (entity.GetComponent<Character>()->current_health <= 0) {
        if (entity.GetComponent<Character>()->is_player) {
          // Removing Dead Player from players vector
          for (auto& player : m_players) {
            if (player.GetComponent<Character>()->character_name == entity.GetComponent<Character>()->character_name) {
              m_players.erase(m_players.begin() + to_delete);
            }
            ++to_delete;
          }
        }
        else {
          // Removing Dead Enemy from enemies vector
          for (auto& enemy : m_enemies) {
            if (enemy.GetComponent<Character>()->character_name == entity.GetComponent<Character>()->character_name) {
              m_enemies.erase(m_enemies.begin() + to_delete);
            }
            ++to_delete;
          }
        }
        to_delete = 0;
        // Removing Dead Character from battle system slots
        for (int i = 0; i < m_slots.size(); i++) {
          if (m_slots[i].GetComponent<BattleSlot>()->character == entity) {
            m_slots[i].GetComponent<IsActive>()->is_active = false;
            m_slots[i].GetComponent<BattleSlot>()->character = FlexECS::Entity::Null;
          }
        }
        // Removing Dead Character from character vector
        entity_to_delete = entity;
      }
    }
    if (entity_to_delete != FlexECS::Entity::Null) m_characters.remove(entity_to_delete);
    battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
  }
}