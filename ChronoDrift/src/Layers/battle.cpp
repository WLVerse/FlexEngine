/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      battle.cpp
\author    [78%] Rocky Sutarius
\par       rocky.sutarius\@digipen.edu
\co-author [22%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file is where the functions to set up and run
           the battle scene of the game, is contained at. The
           functions include:
           - SetupBattle()
           - OnAttach()
           - OnDetach()
           - Update()

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "battle.h"

#include "States.h"
#include "Layers.h"

#include "Components/battlecomponents.h"
#include "Components/physics.h"
#include "Components/rendering.h"

#include "BattleSystems/turnorderdisplay.h"
#include "BattleSystems/targetingsystem.h"
#include "BattleSystems/battlesystem.h"

#include "Renderer/sprite2d.h"
#include <Components/characterinput.h>
#include <BattleSystems/physicssystem.h>
#include <CharacterPrefab/characterprefab.h>

namespace ChronoDrift {

  void BattleLayer::SetupBattle()
  {
    m_battlesystem.AddCharacters(FlexECS::Scene::GetActiveScene()->CachedQuery<Character>());
    m_battlesystem.BeginBattle();
  }

  void BattleLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();
    MoveRegistry::RegisterMoveFunctions();
    MoveRegistry::RegisterStatusFunctions();

    #ifdef GAME
    #pragma region This is copy pasted code that shuld never be like this, pls remove aft m3iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii
    auto scene = FlexECS::Scene::GetActiveScene();
    std::vector<FlexECS::Entity> characters = scene->CachedQuery<Character>();
    // In the scenario where a joker begins battle when they actually wanted to restart
    if (!scene->CachedQuery<BattleSlot>().empty()) {
      ResetCharacters();
      SetupBattle();
    }
    // Ah now this one is to actually set up for the scene without any battle slots
    if (!characters.empty()) {
      int num_of_players = 0, num_of_enemies = 0;
      for (auto& c : characters) {
        if (c.GetComponent<Character>()->is_player) ++num_of_players;
        else ++num_of_enemies;
      }
      if (num_of_enemies > 0 && num_of_players > 0) SetupBattle(); // Set Up Battle
      else Log::Debug("Please load a proper battle scene first. Thank you.");
    }
    else Log::Debug("Please load a proper battle scene first. Thank you.");
    #pragma endregion
    #endif
  }

  void BattleLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void BattleLayer::Update()
  {
    auto scene = FlexECS::Scene::GetActiveScene();
    #ifndef GAME
    if (ImGui::BeginMainMenuBar()) {
      ImGui::SetCursorPosY((ImGui::GetWindowHeight() - ImGui::GetFrameHeight()) / 2.f);
      if (ImGui::BeginMenu("Game Play Options")) {
        if (ImGui::MenuItem("Begin Battle")) {
          std::vector<FlexECS::Entity> characters = scene->CachedQuery<Character>();
          // In the scenario where a joker begins battle when they actually wanted to restart
          if (!scene->CachedQuery<BattleSlot>().empty()) {
            ResetCharacters();
            SetupBattle();
          }
          // Ah now this one is to actually set up for the scene without any battle slots
          if (!characters.empty()) {
            int num_of_players = 0, num_of_enemies = 0;
            for (auto& c : characters) {
              if (c.GetComponent<Character>()->is_player) ++num_of_players;
              else ++num_of_enemies;
            }
            if (num_of_enemies > 0 && num_of_players > 0) SetupBattle(); // Set Up Battle
            else Log::Debug("Please load a proper battle scene first. Thank you.");
          }
          else Log::Debug("Please load a proper battle scene first. Thank you.");
        }
        if (ImGui::MenuItem("Restart Battle", "Ctrl+R")) {
          if (!scene->CachedQuery<BattleSlot>().empty()) {
            ResetCharacters();
            SetupBattle();
          }
          else Log::Debug("Please load a proper battle scene first. Thank you.");
        }
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
    #endif
    // Include a check whether battle system has been activated
    if (!scene->CachedQuery<BattleSlot>().empty()) m_battlesystem.Update();

    //if (!scene->CachedQuery<CharacterInput, Character>().empty()) {
    //  // Get the current camera (SO COOLLLL!)
    //  FlexECS::Entity cam_entity = m_CamM_Instance->GetMainCamera();
    //  auto& curr_cam = cam_entity.GetComponent<Camera>()->camera;
    //  #ifndef GAME // Lock camera during game mode
    //  for (auto& s : scene->CachedQuery<CharacterInput>()) {
    //    // Set Camera Position to Player with an offset lah
    //    curr_cam.position = s.GetComponent<Position>()->position - Vector2(600, 500);
    //    if (Input::GetKey(GLFW_KEY_W)) {
    //      s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->up_cols;
    //      s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->up_movement;
    //    }
    //    else if (Input::GetKey(GLFW_KEY_A)) {
    //      s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->left_cols;
    //      s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->left_movement;
    //    }
    //    else if (Input::GetKey(GLFW_KEY_S)) {
    //      s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->down_cols;
    //      s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->down_movement;
    //    }
    //    else if (Input::GetKey(GLFW_KEY_D)) {
    //      s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->right_cols;
    //      s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->right_movement;
    //    }
    //    else {
    //      s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->idle_cols;
    //      s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->idle_movement;
    //    }
    //  }
    //  // Updating camera position
    //  m_CamM_Instance->UpdateData(cam_entity, curr_cam);
    //  #endif
    //}

    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<CharacterInput>())
    {
      entity.GetComponent<CharacterInput>()->up = Input::GetKey(GLFW_KEY_W);
      entity.GetComponent<CharacterInput>()->down = Input::GetKey(GLFW_KEY_S);
      entity.GetComponent<CharacterInput>()->left = Input::GetKey(GLFW_KEY_A);
      entity.GetComponent<CharacterInput>()->right = Input::GetKey(GLFW_KEY_D);
    }

    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<CharacterInput, Rigidbody>())
    {
      auto& velocity = entity.GetComponent<Rigidbody>()->velocity;
      velocity.x = 0.0f;
      velocity.y = 0.0f;

      if (entity.GetComponent<CharacterInput>()->up)
      {
        velocity.y = -300.0f;
      }

      if (entity.GetComponent<CharacterInput>()->down)
      {
        velocity.y = 300.0f;
      }

      if (entity.GetComponent<CharacterInput>()->left)
      {
        velocity.x = -300.0f;
      }

      if (entity.GetComponent<CharacterInput>()->right)
      {
        velocity.x = 300.0f;
      }
    }
    #if GAME // Restart characters
    if (Input::GetKeyDown(GLFW_KEY_R)) {
      ResetCharacters();
      SetupBattle();
    }
    #endif

    RenderSprite2D();
    UpdatePhysicsSystem();
  }
}