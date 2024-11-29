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

// Library effective with Windows
#include <CharacterPrefab/characterprefab.h>

namespace ChronoDrift {

  static int move_decision = -1; // variable storing move selection
  static float delay_timer = 0.f;

  static int boss_move = 0;
  static bool godMode = false;
  static std::string currentPrint;
  //Sorting functions
  struct SortLowestSpeed
  {
    bool operator()(FlexECS::Entity e1, FlexECS::Entity e2) {
      return e1.GetComponent<Character>()->current_speed < e2.GetComponent<Character>()->current_speed;
    }
  };

  struct SortLowestPositions
  {
    bool operator()(FlexECS::Entity& e1, FlexECS::Entity& e2) {
      std::pair<bool, bool> comparison_results = std::make_pair(false, false);

      Vector2 e1_pos = e1.GetComponent<BattleSlot>()->character.GetComponent<Position>()->position;
      Vector2 e2_pos = e2.GetComponent<BattleSlot>()->character.GetComponent<Position>()->position;
      
      comparison_results.first = (e1_pos.x < e2_pos.x) ? true : false;
      comparison_results.second = (e1_pos.y < e2_pos.y) ? true : false;

      if (comparison_results.first) return true;
      else {
        if (comparison_results.second) return true;
        else return false;
      }
    }
  };

  BattleSystem::BattleSystem() : players_displayed(0), enemies_displayed(0) {

  }
  BattleSystem::~BattleSystem()
  {
  }
  void BattleSystem::SetUpBattleScene() {
    auto scene = FlexECS::Scene::GetActiveScene();

    for (int i = 0; i < (players_displayed + enemies_displayed); i++) {
      FlexECS::Entity slot; //FlexECS::Scene::CreateEntity("Slot" + std::to_string(i));
      if (i < players_displayed) slot = m_players[i];
      else slot = m_enemies[i - players_displayed];
      slot.AddComponent<OnClick>({});
      slot.AddComponent<IsActive>({ false });
      slot.AddComponent<Position>({slot.GetComponent<BattleSlot>()->character.GetComponent<Position>()->position});
      slot.AddComponent<Scale>({ { 100,100 } });
      slot.AddComponent<ZIndex>({ 9 });
      slot.AddComponent<Rotation>({});
      slot.AddComponent<Transform>({});
      slot.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"()"),
        SLOT_SUB_TARGET_COLOR,
        Vector3::One,
        Renderer2DProps::Alignment_Center,
        Renderer2DProps::VBO_BasicInverted,
        false
      });
      slot.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

      /*if (i < players_displayed) {
        slot.GetComponent<Position>()->position = { SLOT_POS_HOR_PLAYER + 120.f * i, SLOT_POS_VERT_PLAYER };
      }
      else {
        slot.GetComponent<Position>()->position = { SLOT_POS_HOR_ENEMY + 120.f * (i - players_displayed), SLOT_POS_VERT_ENEMY };
      }*/
      //m_slots.push_back(slot);
    }
    // Move Buttons for Mouse Click Selection of Moves
    //if (!move_buttons.empty()) return;
    /*for (int i = 0; i < 3; i++)
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
      move_buttons.push_back(move_button);
    }*/
  }

  void BattleSystem::AddCharacters(std::vector<FlexECS::Entity> characters) {
    auto scene = FlexECS::Scene::GetActiveScene();
    // Destroy any battle slot entites exisiting in loaded scene
    if (!scene->CachedQuery<BattleSlot>().empty()) {
      for (auto& bs : scene->CachedQuery<BattleSlot>()) {
        scene->DestroyEntity(bs);
      }
    }
    // Destroy any turn order display entities exisiting in loaded scene
    if (!scene->CachedQuery<TurnOrderDisplay>().empty()) {
      for (auto& td : scene->CachedQuery<TurnOrderDisplay>()) {
        scene->DestroyEntity(td);
      }
    }
    // Clear characters container
    if (!m_characters.empty()) m_characters.clear();
    // Clear enemy slot container
    if (!m_enemies.empty()) m_enemies.clear();
    // Clear player slot containeer
    if (!m_players.empty()) m_players.clear();
    
    for (size_t i = 0; i < characters.size(); i++) {
      // adding character into m_characters container that will be sorted according to player speed
      m_characters.push_back(characters[i]);
      // Creating a slot entity
      FlexECS::Entity temp_character = scene->CreateEntity("Target Slot " + std::to_string(i));
      temp_character.AddComponent<BattleSlot>({ characters[i] });
      // Creating a slot that will be used by the speed queue display (logic of it will change in the future)
      FlexECS::Entity display_slot = scene->CreateEntity("Speed Queue Slot " + std::to_string(i));
      display_slot.AddComponent<TurnOrderDisplay>({});
      display_slot.AddComponent<IsActive>({ false });
      display_slot.AddComponent<Position>({ });
      display_slot.AddComponent<Scale>({ { 60,60 } });
      display_slot.AddComponent<ZIndex>({ 10 });
      display_slot.AddComponent<Sprite>({});
      display_slot.AddComponent<Rotation>({});
      display_slot.AddComponent<Transform>({});
      display_slot.AddComponent<Shader>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\shaders\texture)") });
      // Now this is where we add player slots to m_players & enemy slots to m_enemies
      if (characters[i].GetComponent<Character>()->is_player) {
        m_players.push_back(temp_character);
      }
      else {
        m_enemies.push_back(temp_character);
      }
    }

    // One extra step: we need to sort the vectors according to their positions
    std::sort(m_players.begin(), m_players.end(), SortLowestPositions());
    std::sort(m_enemies.begin(), m_enemies.end(), SortLowestPositions());

    players_displayed = m_players.size();
    enemies_displayed = m_enemies.size();
    SetUpBattleScene();
  }

  void BattleSystem::BeginBattle()
  {
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
    {
        if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "win")
        {
            entity.GetComponent<IsActive>()->is_active = false;
        }
        else if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "lose")
        {
            entity.GetComponent<IsActive>()->is_active = false;
        }
        else if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "RenkoInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "GraceInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "JackInfo")
            || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "Info"))
        {
            entity.GetComponent<IsActive>()->is_active = true;
        }
    }
    boss_move = 0;
    currentPrint = "";
    toPrint.clear();
    /*for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Audio>())
    {
        if (!element.GetComponent<IsActive>()->is_active) continue; // skip non active entities

        ChronoDrift::Audio* audio = element.GetComponent<Audio>();
        if (audio->should_play)
        {
            if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(audio->audio_file) == "")
            {
                Log::Warning("Audio not attached to entity: " +
                  FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()));

                audio->should_play = false;
                continue;
            }

            if (audio->is_looping)
            {
                FMODWrapper::Core::PlayLoopingSound(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()),
                                                    FLX_ASSET_GET(Asset::Sound, AssetKey{ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(element.GetComponent<Audio>()->audio_file) }));
            }
            else
            {
                FMODWrapper::Core::PlaySound(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()),
                                             FLX_ASSET_GET(Asset::Sound, AssetKey{ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(element.GetComponent<Audio>()->audio_file) }));
            }

            element.GetComponent<Audio>()->should_play = false;
        }
    }*/

    std::sort(m_characters.begin(), m_characters.end(), SortLowestSpeed());

    auto scene = FlexECS::Scene::GetActiveScene();

    currentPrint = "Begin Battle!";
    for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
    {
        if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
        {
            scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
        }
    }
    toPrint.push_back(currentPrint);
    std::cout << currentPrint << "\n";

    for (auto& entity : m_characters)
    {
      //auto cw = entity.GetComponent<Character>()->weapon_name;
      currentPrint = scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name)
          + " / HP: " + std::to_string(entity.GetComponent<Character>()->current_health)
          + " / SPD: " + std::to_string(entity.GetComponent<Character>()->current_speed);
          //+ " / Weap: " + scene->Internal_StringStorage_Get(cw);
      //if (entity.GetComponent<Character>()->is_player) {
        //auto cg = entity.GetComponent<Character>()->chrono_gear_description;
        //currentPrint += " / Chrono Gear: " + scene->Internal_StringStorage_Get(cg);
      //}
      for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
      {
          if ((scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Renko")
              || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Grace")
              || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Jack"))
          {
              scene->Internal_StringStorage_Get(e.GetComponent<Text>()->text) = currentPrint;
          }
      }
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";
    }

    //FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->Query<BattleState>()[0];
    //battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
    battle_phase = BP_PROCESSING;
  }

  void BattleSystem::UpdateSpeedStack() {
    auto scene = FlexECS::Scene::GetActiveScene();
    int speed_to_decrease = m_characters.front().GetComponent<Character>()->current_speed;
    FlexECS::Scene::StringIndex front_character = m_characters.front().GetComponent<Character>()->character_name;

    currentPrint = scene->Internal_StringStorage_Get(front_character) + "'s turn!";
    for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
    {
        if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
        {
            scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
        }
    }
    toPrint.push_back(currentPrint);
    std::cout << currentPrint << "\n";

    for (auto& entity : m_characters)
    {
      entity.GetComponent<Character>()->current_speed -= speed_to_decrease;
      currentPrint = scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name)
          + " / HP: " + std::to_string(entity.GetComponent<Character>()->current_health)
          + " / SPD: " + std::to_string(entity.GetComponent<Character>()->current_speed);
      for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
      {
          if ((scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Renko")
              || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Grace")
              || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Jack"))
          {
              scene->Internal_StringStorage_Get(e.GetComponent<Text>()->text) = currentPrint;
          }
      }
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";
    }
    if (!m_characters.front().GetComponent<Character>()->is_player) delay_timer = DELAY_TIME;

    //FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->Query<BattleState>()[0];
    //battle_state.GetComponent<BattleState>()->active_character = m_characters.front();
    //battle_state.GetComponent<BattleState>()->phase = BP_STATUS_RUN;
    battle_phase = BP_STATUS_RUN;
  }

  void BattleSystem::RunCharacterStatus() {
      auto scene = FlexECS::Scene::GetActiveScene();
    std::vector<FlexECS::Entity> dead_character;
    dead_character.clear();
    FlexECS::Entity current_active = m_characters.front();
    std::string name = scene->Internal_StringStorage_Get(current_active.GetComponent<Character>()->character_name);
    if (current_active.HasComponent<Shock>()) {
      current_active.GetComponent<Character>()->current_health -= current_active.GetComponent<Shock>()->damage_value;

      currentPrint = name + " lost " + std::to_string(current_active.GetComponent<Shock>()->damage_value) + " HP from shock DOT.";
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      currentPrint = name + "'s current HP: " + std::to_string(current_active.GetComponent<Character>()->current_health);
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      if (--(current_active.GetComponent<Shock>()->remaining_turns) == 0) {
          currentPrint = name + "'s shock effect has ended.";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
        toPrint.push_back(currentPrint);
        std::cout << currentPrint << "\n" << "\n";
        current_active.RemoveComponent<Shock>();
      }
      else {
          currentPrint = name + "'s shock effect still has " + std::to_string(current_active.GetComponent<Shock>()->remaining_turns) + " turn(s).";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
        toPrint.push_back(currentPrint);
        std::cout << currentPrint << "\n" << "\n";
      }
      delay_timer = DELAY_TIME;
    }

    if (current_active.HasComponent<Burn>()) {
      current_active.GetComponent<Character>()->current_health -= current_active.GetComponent<Burn>()->damage_value;

      currentPrint = name + " lost " + std::to_string(current_active.GetComponent<Burn>()->damage_value) + " HP from burn DOT.";
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      currentPrint = name + "'s current HP: " + std::to_string(current_active.GetComponent<Character>()->current_health);
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      if (--(current_active.GetComponent<Burn>()->remaining_turns) == 0) {
          currentPrint = name + "'s burn effect has ended.";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
        toPrint.push_back(currentPrint);
        std::cout << currentPrint << "\n" << "\n";
        current_active.RemoveComponent<Burn>();
      }
      else {
          currentPrint = name + "'s burn effect still has " + std::to_string(current_active.GetComponent<Burn>()->remaining_turns) + " turn(s).";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
        toPrint.push_back(currentPrint);
        std::cout << currentPrint << "\n" << "\n";
      }
      delay_timer = DELAY_TIME;
    }

    if (current_active.HasComponent<Shear>()) {
      current_active.GetComponent<Character>()->current_health -= current_active.GetComponent<Shear>()->damage_value;

      currentPrint = name + " lost " + std::to_string(current_active.GetComponent<Shear>()->damage_value) + " HP from shear DOT.";
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      currentPrint = name + "'s current HP: " + std::to_string(current_active.GetComponent<Character>()->current_health);
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      if (--(current_active.GetComponent<Shear>()->remaining_turns) == 0) {
          currentPrint = name + "'s shear effect has ended.";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
        current_active.RemoveComponent<Shear>();
      }
      else {
          currentPrint = name + "'s shear effect still has " + std::to_string(current_active.GetComponent<Shear>()->remaining_turns) + " turn(s).";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
          std::cout << name << "'s shear effect still has " << std::to_string(current_active.GetComponent<Shear>()->remaining_turns) << " turn(s).";
      }
      delay_timer = DELAY_TIME;
    }

    if (current_active.HasComponent<Recovery>()) {
      current_active.GetComponent<Character>()->current_health += current_active.GetComponent<Recovery>()->heal_value;
      if (current_active.GetComponent<Character>()->current_health > current_active.GetComponent<Character>()->base_health)
      {
          current_active.GetComponent<Character>()->current_health = current_active.GetComponent<Character>()->base_health;
      }
      currentPrint = name + " recovered " + std::to_string(current_active.GetComponent<Recovery>()->heal_value) + " HP.";
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      currentPrint = name + "'s current HP: " + std::to_string(current_active.GetComponent<Character>()->current_health);
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";

      if (--(current_active.GetComponent<Recovery>()->remaining_turns) == 0) {
          currentPrint = name + "'s healing effect has ended.";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
        current_active.RemoveComponent<Recovery>();
      }
      else {
          currentPrint = name + "'s healing effect still has " + std::to_string(current_active.GetComponent<Recovery>()->remaining_turns) + " turn(s).";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
      }
      delay_timer = DELAY_TIME;
    }

    if (current_active.HasComponent<Immunity>()) {
      if (--(current_active.GetComponent<Immunity>()->remaining_turns) <= 0) {
          currentPrint = name + "'s immunity has ended.";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
        current_active.RemoveComponent<Immunity>();
      }
      else {
          currentPrint = name + " is immune to all damage for another " + std::to_string(current_active.GetComponent<Immunity>()->remaining_turns) + " turn(s).";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
      }
      delay_timer = DELAY_TIME;
    }

    if (current_active.HasComponent<Attack_Buff>()) {
        if (--(current_active.GetComponent<Attack_Buff>()->remaining_turns) <= 0) {
            currentPrint = name + "'s attack buff has ended.";
            for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
            {
                if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
                {
                    scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
                }
            }
            toPrint.push_back(currentPrint);
            std::cout << currentPrint << "\n" << "\n";
            current_active.RemoveComponent<Attack_Buff>();
        }
        else {
            currentPrint = name + "'s attack buff still has " + std::to_string(current_active.GetComponent<Attack_Buff>()->remaining_turns) + " turn(s).";
            for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
            {
                if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
                {
                    scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
                }
            }
            toPrint.push_back(currentPrint);
            std::cout << currentPrint << "\n" << "\n";
        }
        delay_timer = DELAY_TIME;
    }

    if (current_active.HasComponent<Attack_Debuff>()) {
        if (--(current_active.GetComponent<Attack_Debuff>()->remaining_turns) <= 0) {
            currentPrint = name + "'s attack debuff has ended.";
            for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
            {
                if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
                {
                    scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
                }
            }
            toPrint.push_back(currentPrint);
            std::cout << currentPrint << "\n" << "\n";
            current_active.RemoveComponent<Attack_Debuff>();
        }
        else {
            currentPrint = name + "'s attack debuff still has " + std::to_string(current_active.GetComponent<Attack_Debuff>()->remaining_turns) + " turn(s).";
            for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
            {
                if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
                {
                    scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
                }
            }
            toPrint.push_back(currentPrint);
            std::cout << currentPrint << "\n" << "\n";
        }
        delay_timer = DELAY_TIME;
    }
    // Just a note that the stun should be the last check for now cause if stun then i would rather just go
    // to next character and skip his turn
    //FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->Query<BattleState>()[0];
    if (current_active.HasComponent<Stun>()) {
      current_active.GetComponent<Character>()->current_speed += current_active.GetComponent<Character>()->base_speed + 10;

      currentPrint = name + " is stunned.";
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n" << "\n";

      if (--(current_active.GetComponent<Stun>()->remaining_turns) <= 0) {
          currentPrint = name + " stun status has ended.";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
        current_active.RemoveComponent<Stun>();
      }
      else {
          currentPrint = name + " stun status still has " + std::to_string(current_active.GetComponent<Stun>()->remaining_turns) + " turn(s).";
          for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
              {
                  scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n" << "\n";
      }

      std::sort(m_characters.begin(), m_characters.end(), SortLowestSpeed());
      //battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
      battle_phase = BP_PROCESSING;
      delay_timer = DELAY_TIME;
    }
    else { //battle_state.GetComponent<BattleState>()->phase = BP_MOVE_SELECTION;
      battle_phase = BP_MOVE_SELECTION;
    }

    if (current_active.GetComponent<Character>()->current_health <= 0) {
      dead_character.push_back(current_active);
      currentPrint = name + "  has been killed.";
      toPrint.push_back(currentPrint);
      std::cout << currentPrint << "\n";
      DeathProcession(dead_character);
    }
  }

  void BattleSystem::Update()
  {
    if (delay_timer > 0.f) {
      delay_timer -= Application::GetCurrentWindow()->GetDeltaTime();
    }
    //auto query = FlexECS::Scene::GetActiveScene()->Query<BattleState>();
    //if (query.empty()) return; // Guard for resetted battle scene

    //FlexECS::Entity battle_state = query[0];
    //int battle_phase = battle_state.GetComponent<BattleState>()->phase;
    if (delay_timer <= 0.f) {
      switch (battle_phase) {
      case BP_PROCESSING:
        move_decision = -1;
        UpdateSpeedStack();
        break;
      case BP_STATUS_RUN:
        RunCharacterStatus();
        break;
      case BP_MOVE_SELECTION:
        PlayerMoveSelection();
        break;
      case BP_MOVE_EXECUTION:
        ExecuteMove();
        break;
      case BP_MOVE_ANIMATION:
        break;
      }
    }
    if (battle_phase != BP_BATTLE_FINISH) EndBattleScene();

    for (auto& entity : m_characters)
    {
        if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "Renko")
        {
            if (entity.HasComponent<Attack_Buff>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoAU")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoAU")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
            if (entity.HasComponent<Attack_Debuff>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoAD")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoAD")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
            if (entity.HasComponent<Immunity>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoIM")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoIM")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
        }
        else if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "Grace")
        {
            if (entity.HasComponent<Attack_Buff>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceAU")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceAU")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
            if (entity.HasComponent<Attack_Debuff>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceAD")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceAD")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
            if (entity.HasComponent<Immunity>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceIM")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceIM")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
        }
        else if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "Jack")
        {
            if (entity.HasComponent<Attack_Buff>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackAU")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackAU")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
            if (entity.HasComponent<Attack_Debuff>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackAD")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackAD")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
            if (entity.HasComponent<Immunity>())
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackIM")
                    {
                        e.GetComponent<IsActive>()->is_active = true;
                    }
                }
            }
            else
            {
                for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
                {
                    if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackIM")
                    {
                        e.GetComponent<IsActive>()->is_active = false;
                    }
                }
            }
        }
    }

    while (toPrint.size() > LINE_LIMIT)
    {
        toPrint.erase(toPrint.begin());
    }
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
    {
        if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "yap")
        {
            currentPrint = "";
            for (auto stringElements : toPrint)
            {
                currentPrint += stringElements;
            }
            FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Text>()->text) = currentPrint;
        }
    }
    if (battle_phase != BP_BATTLE_FINISH) {

        if (Input::GetKeyDown(GLFW_KEY_Z))
        {
            currentPrint = "You win!";
            for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
            {
                if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
                {
                    FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
                }
            }
            toPrint.push_back(currentPrint);
            std::cout << currentPrint;
            battle_phase = BP_BATTLE_FINISH;
            for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
            {
                if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "win")
                {
                    entity.GetComponent<IsActive>()->is_active = true;
                }
            }
            for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
            {
                if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "yap")
                {
                    entity.GetComponent<IsActive>()->is_active = false;
                }
                if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "RenkoInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "GraceInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "JackInfo"))
                {
                    entity.GetComponent<IsActive>()->is_active = false;
                }
            }
            //toPrint.clear();
        }
        else if (Input::GetKeyDown(GLFW_KEY_X))
        {
            if (!godMode)
            {
                currentPrint = "GODMODE!!! Invulnerable to all damage and x2 damage and increases duration on moves!";
                for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
                {
                    if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
                    {
                        FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
                    }
                }
                toPrint.push_back(currentPrint);
                std::cout << currentPrint << "\n";
                godMode = true;
            }
            else
            {
                currentPrint = "Godmode off...";
                for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
                {
                    if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
                    {
                        FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
                    }
                }
                toPrint.push_back(currentPrint);
                std::cout << currentPrint << "\n";
                godMode = false;
            }
        }
    }

    DisplayTurnOrder(GetTurnOrder());
  }

  void BattleSystem::PlayerMoveSelection()
  {
    static auto selected_num = m_enemies.begin();
    std::array<FlexECS::Scene::StringIndex, 3> character_moves{
      m_characters.front().GetComponent<Character>()->weapon_move_one,
      m_characters.front().GetComponent<Character>()->weapon_move_two,
      m_characters.front().GetComponent<Character>()->weapon_move_three,
    };

    for (auto& slot : m_enemies) {
      slot.GetComponent<IsActive>()->is_active = false;
    }
    for (auto& slot : m_players) {
      slot.GetComponent<IsActive>()->is_active = false;
    }
    
    if (!m_characters.front().GetComponent<Character>()->is_player) {
      // Random move selection for enemies
      //move_decision = Range(0, 2).Get();
        move_decision = boss_move;
        if (move_decision >= 2)
        {
            move_decision = 2;
        }
        boss_move++;
    }
    else {
      if (Input::GetKeyDown(GLFW_KEY_1)) move_decision = 0;
      else if (Input::GetKeyDown(GLFW_KEY_2)) move_decision = 1;
      else if (Input::GetKeyDown(GLFW_KEY_3)) move_decision = 2;

      if (Input::GetKeyDown(GLFW_KEY_1) || Input::GetKeyDown(GLFW_KEY_2) || Input::GetKeyDown(GLFW_KEY_3)) {
        Move player_move = MoveRegistry::GetMove(
        FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(
          character_moves[move_decision]));

        currentPrint = "Move Selected: " + player_move.name;
        for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
        {
            if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
            {
                FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
            }
        }
        toPrint.push_back(currentPrint);
        std::cout << currentPrint << "\n";

        if (player_move.is_target_enemy) selected_num = m_enemies.begin();
        else selected_num = m_players.begin();
      }
      /*for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, MoveButton>()) {
        entity.GetComponent<IsActive>()->is_active = true;
      }*/
      // move selection system by mouse click for players
      //GetMoveSelection();
      //for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<OnClick, MoveButton>())
      //{
      //  auto& click_status = entity.GetComponent<OnClick>()->is_clicked;
      //  if (click_status == true) {
      //    move_decision = entity.GetComponent<MoveButton>()->move_number;
      //    Move player_move = MoveRegistry::GetMove(
      //      FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(
      //        character_moves[move_decision]));
      //    std::cout << "Move Selected: " << player_move.name << std::endl;
      //    if (player_move.is_target_enemy) selected_num = m_enemies.begin();
      //    else selected_num = m_players.begin();
      //    //selected_num = (!player_move.is_target_player * players_displayed);
      //    //selected_num += (!player_move.is_target_player * players_displayed);
      //  }
      //}
    }
    if (move_decision == -1) return;
    Move locked_in_move = MoveRegistry::GetMove(
            FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(
              character_moves[move_decision]));
    std::vector<FlexECS::Entity> result;
    auto max_targets = m_enemies.end();
    auto min_targets = m_enemies.begin();

    // Identify whether target is enemy or team
    // number of targets
    // selection of adjecent targets should only apply
    // when target count is less than 5
    size_t no_of_targets = 0;
    if (!locked_in_move.is_target_enemy) {
      // means is player target
      no_of_targets = players_displayed;
      min_targets = m_players.begin();
      max_targets = m_players.end();
    }
    else {
      // means is enemy target
      no_of_targets = enemies_displayed;
    }

    int num_of_adjacent_targets = 0;
    bool is_adjacent = false;
    if (locked_in_move.target_type % 2) {
      // Means Odd
      num_of_adjacent_targets = (locked_in_move.target_type - 1) / 2;
    }
    else {
      // Means Even (Only works for 2 targets)
      is_adjacent = true;
      num_of_adjacent_targets = locked_in_move.target_type / 2;
    }

    static int adjacent = 1;
    if (m_characters.front().GetComponent<Character>()->is_player) {
      if (locked_in_move.target_type < no_of_targets) {
        // targetting system
        if (Input::GetKeyDown(GLFW_KEY_D)) {
          if (selected_num != (max_targets - 1)) selected_num++;
        }
        if (Input::GetKeyDown(GLFW_KEY_A)) {
          if (selected_num != min_targets) selected_num--;
        }
      }
    }
    else {
      if (locked_in_move.target_type < no_of_targets) {
        int max = 0;
        if (!locked_in_move.is_target_enemy) max = static_cast<int>(players_displayed) - 1;
        else max = static_cast<int>(enemies_displayed) - 1;
        int random_selection = Range(0, max).Get();
        selected_num = min_targets + random_selection;
      }
    }

    if (locked_in_move.target_type < no_of_targets) {
      // Setting the adjacent value
      if (selected_num == (max_targets - 1)) adjacent = -1;
      else if (selected_num == min_targets) adjacent = 1;
      // This is for target types greater than MOVE_TARGET_DOUBLE
      std::pair<bool, bool> out_of_bounds_flag = std::make_pair(false, false);
      if (num_of_adjacent_targets > 0 && locked_in_move.target_type != MOVE_TARGET_DOUBLE) {
        for (int i = 1; i <= num_of_adjacent_targets; i++) {
          if (selected_num != min_targets) {
            if (!out_of_bounds_flag.first) {
              (*(selected_num - i)).GetComponent<IsActive>()->is_active = true;
              if ((selected_num - i) == min_targets) {
                out_of_bounds_flag.first = true;
              }
            }
          }
          if (selected_num != (max_targets - 1)) {
            if (!out_of_bounds_flag.second) {
              (*(selected_num + i)).GetComponent<IsActive>()->is_active = true;
              if ((selected_num + i) == (max_targets - 1)) {
                out_of_bounds_flag.second = true;
              }
            }
          }
        }
      }
      // For Special Case Adjacent
      if (is_adjacent && locked_in_move.target_type > 1) {
        // Means even number of targets and that ain't good
        if (locked_in_move.target_type == MOVE_TARGET_DOUBLE) num_of_adjacent_targets = 0;
        // If the adjacent target issa on the right
        if (adjacent == 1) {
          if (!out_of_bounds_flag.second) {
            if ((selected_num + num_of_adjacent_targets) != (max_targets - 1)) {
              (*(selected_num + num_of_adjacent_targets + adjacent)).GetComponent<IsActive>()->is_active = true;
            }
          }
        }
        if (adjacent == -1) {
          if (!out_of_bounds_flag.first) {
            if ((selected_num - num_of_adjacent_targets) != min_targets) {
              (*(selected_num - num_of_adjacent_targets + adjacent)).GetComponent<IsActive>()->is_active = true;
            }
          }
        }
      }
      // I mean this should be quite obvious rite?
      (*selected_num).GetComponent<IsActive>()->is_active = true;
    }
    else {
      // all the slots are to be targetted
      for (auto i = min_targets; i != max_targets; i++) {
        (*i).GetComponent<IsActive>()->is_active = true;
      }
    }

    // all the targets selected man
    if (!m_characters.front().GetComponent<Character>()->is_player ||
      (Input::GetKeyDown(GLFW_KEY_SPACE) && m_characters.front().GetComponent<Character>()->is_player)) {
      for (auto i = min_targets; i != max_targets; i++) {
        if ((*i).GetComponent<IsActive>()->is_active) result.push_back((*i).GetComponent<BattleSlot>()->character);
      }
    }

    if (result.empty()) return;

    selected_move = character_moves[move_decision];
    move_decision = -1;
    selected_targets = std::make_pair(*(selected_num), result);
    result.clear();
    //FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->Query<BattleState>()[0];
    //battle_state.GetComponent<BattleState>()
    battle_phase = BP_MOVE_EXECUTION;
    //ExecuteMove(character_moves[final_decision], final_result);
    for (auto& slot : m_enemies) {
      slot.GetComponent<IsActive>()->is_active = false;
    }
    for (auto& slot : m_players) {
      slot.GetComponent<IsActive>()->is_active = false;
    }
  }

  //void BattleSystem::ExecuteMove(FlexECS::Scene::StringIndex move_id, std::vector<FlexECS::Entity> selected_targets)
  void BattleSystem::ExecuteMove()
  {
    //get the move user
    //FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->Query<BattleState>()[0];
    FlexECS::Entity user = m_characters.front();//battle_state.GetComponent<BattleState>()->active_character;
    //Move move = MoveRegistry::GetMove(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(move_id));
    Move move = MoveRegistry::GetMove(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(selected_move));
    std::vector<FlexECS::Entity> targets;
    targets.insert(targets.begin(), selected_targets.second.begin(), selected_targets.second.end());

    auto scene = FlexECS::Scene::GetActiveScene();
    //execute move

    /*for (auto& e : scene->CachedQuery<Animation>())
    {
        if ((scene->Internal_StringStorage_Get(*user.GetComponent<EntityName>())) == (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>())))
        {
            e.GetComponent<Animation>()->rows = 14;
            e.GetComponent<Animation>()->max_sprites = 14;
            (scene->Internal_StringStorage_Get(e.GetComponent<Animation>()->spritesheet)) = "images/Char_Grace_Attack_Anim_Sheet.png";
            e.GetComponent<Animation>()->m_animation_speed = 0.5f;
        }
    }*/

    bool isFirst = true;
    for (auto& m : move.move_function_container) {
        int actualValue = m.value;
        if (isFirst)
        {
            if (user.HasComponent<Attack_Buff>())
            {
                actualValue *= 2;
            }
            if (user.HasComponent<Attack_Debuff>())
            {
                actualValue /= 2;
            }
            isFirst = false;
        }
        if (godMode)
        {
            if (user.GetComponent<Character>()->is_player)
            {
                actualValue *= 2;
            }
            else actualValue = 0;
        }
      //m.move_function(targets, m.value);
        m.move_function(targets, actualValue);
    }

    for (auto& s : move.sea_function_container) {
        s.effect_function(targets, s.value, s.duration);
    }

    user.GetComponent<Character>()->current_speed += move.cost + user.GetComponent<Character>()->base_speed;
    //user.GetComponent<Action>()->move_to_use = FlexECS::Entity::Null;
    std::sort(m_characters.begin(), m_characters.end(), SortLowestSpeed());

    //Display Character Move and target selection

    currentPrint = scene->Internal_StringStorage_Get(user.GetComponent<Character>()->character_name)
        + " executed " + move.name
        + " on ";
    for (int i = 0; i < targets.size(); ++i) {
        currentPrint += scene->Internal_StringStorage_Get(targets[i].GetComponent<Character>()->character_name);
      if ((i + 1) < targets.size()) currentPrint += " and ";
    }
    for (auto& et : scene->CachedQuery<Text>())
    {
        if ((scene->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
        {
            scene->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
        }
    }
    toPrint.push_back(currentPrint);
    std::cout << currentPrint << "\n";
    // Display Status of alive Characters
    targets.clear();
    for (auto& entity : m_characters)
    {
      if (entity.GetComponent<Character>()->current_health > 0) {
          currentPrint = scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name)
              + " / HP: " + std::to_string(entity.GetComponent<Character>()->current_health)
              + " / SPD: " + std::to_string(entity.GetComponent<Character>()->current_speed);
          for (auto& e : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
          {
              if ((scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Renko")
                  || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Grace")
                  || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackInfo") && (scene->Internal_StringStorage_Get(entity.GetComponent<Character>()->character_name) == "Jack"))
              {
                  scene->Internal_StringStorage_Get(e.GetComponent<Text>()->text) = currentPrint;
              }
          }
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n";
      }
      else targets.push_back(entity);
    }
    // Display Characters that have died
    for (auto& dead_character : targets)
    {
        currentPrint = scene->Internal_StringStorage_Get(dead_character.GetComponent<Character>()->character_name)
            + " has been killed by " + scene->Internal_StringStorage_Get(user.GetComponent<Character>()->character_name)
            + "'s " + move.name;
        for (auto& e : scene->CachedQuery<Text>())
        {
            if ((scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "RenkoInfo") && (scene->Internal_StringStorage_Get(dead_character.GetComponent<Character>()->character_name) == "Renko")
                || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "GraceInfo") && (scene->Internal_StringStorage_Get(dead_character.GetComponent<Character>()->character_name) == "Grace")
                || (scene->Internal_StringStorage_Get(*e.GetComponent<EntityName>()) == "JackInfo") && (scene->Internal_StringStorage_Get(dead_character.GetComponent<Character>()->character_name) == "Jack"))
            {
               e.GetComponent<IsActive>()->is_active = false;
            }
        }
        toPrint.push_back(currentPrint);
        std::cout << currentPrint << "\n";
    }
    //battle_state.GetComponent<BattleState>()->current_target_count = 0;
    //battle_state.GetComponent<BattleState>()->active_character = FlexECS::Entity::Null;
    DeathProcession(targets);
  }

  void BattleSystem::DeathProcession(std::vector<FlexECS::Entity> list_of_deaths) {
    //FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->Query<BattleState>()[0];
    for (auto it = list_of_deaths.begin(); it != list_of_deaths.end(); it++) {
      for (auto c = m_characters.begin(); c != m_characters.end(); c++) {
        if (*c == *it) {
          m_characters.erase(c);
          (*it).GetComponent<IsActive>()->is_active = false;
          currentPrint = (*it).GetComponent<Character>()->character_name + " has been removed from character list";
          toPrint.push_back(currentPrint);
          std::cout << currentPrint << "\n";
          break;
        }
      }
      if ((*it).GetComponent<Character>()->is_player) {
        for (auto p = m_players.begin(); p != m_players.end(); p++) {
          if ((*p).GetComponent<BattleSlot>()->character == *it) {
            FlexECS::Scene::DestroyEntity(*p);
            m_players.erase(p);
            players_displayed--;
            currentPrint = (*it).GetComponent<Character>()->character_name + " has been removed from player list";
            toPrint.push_back(currentPrint);
            std::cout << currentPrint << "\n";
            break;
          }
        }
      }
      else {
        for (auto e = m_enemies.begin(); e != m_enemies.end(); e++) {
          if ((*e).GetComponent<BattleSlot>()->character == *it) {
            FlexECS::Scene::DestroyEntity(*e);
            m_enemies.erase(e);
            enemies_displayed--;
            currentPrint = (*it).GetComponent<Character>()->character_name + " has been removed from enemy list";
            toPrint.push_back(currentPrint);
            std::cout << currentPrint << "\n";
            break;
          }
        }
      }
    }
    //battle_state.GetComponent<BattleState>()->phase = BP_PROCESSING;
    battle_phase = BP_PROCESSING;
    delay_timer = DELAY_TIME;
  }
  
  void BattleSystem::EndBattleScene() {
    //FlexECS::Entity battle_state = FlexECS::Scene::GetActiveScene()->Query<BattleState>()[0];
    if (m_enemies.empty()) {
        currentPrint = "You win!";
        for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
        {
            if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
            {
                FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
            }
        }
      toPrint.push_back(currentPrint);
      std::cout << currentPrint;
      //battle_state.GetComponent<BattleState>()->phase = BP_BATTLE_FINISH;
      battle_phase = BP_BATTLE_FINISH;
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
      {
          if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "win")
          {
              entity.GetComponent<IsActive>()->is_active = true;
          }
      }
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
      {
          if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "yap")
          {
              entity.GetComponent<IsActive>()->is_active = false;
          }
          if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "RenkoInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "GraceInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "JackInfo"))
          {
              entity.GetComponent<IsActive>()->is_active = false;
          }
      }
    }
    if (m_players.empty()) {
        currentPrint = "You lose...";
        for (auto& et : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
        {
            if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*et.GetComponent<EntityName>()) == "Info"))
            {
                FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(et.GetComponent<Text>()->text) = currentPrint;
            }
        }
      toPrint.push_back(currentPrint);
      std::cout << currentPrint;
      //battle_state.GetComponent<BattleState>()->phase = BP_BATTLE_FINISH;
      battle_phase = BP_BATTLE_FINISH;
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
      {
          if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "lose")
          {
              entity.GetComponent<IsActive>()->is_active = true;
          }
      }
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive>())
      {
          if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "yap")
          {
              entity.GetComponent<IsActive>()->is_active = false;
          }
          if ((FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "RenkoInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "GraceInfo")
              || (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) == "JackInfo"))
          {
              entity.GetComponent<IsActive>()->is_active = false;
          }
      }
    }
  }
}