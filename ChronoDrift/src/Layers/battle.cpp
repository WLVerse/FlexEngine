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

#include "Physics/box2d.h"
#include "Renderer/sprite2d.h"
#include <Components/characterinput.h>
#include <BattleSystems/physicssystem.h>
#include <CharacterPrefab/characterprefab.h>

namespace ChronoDrift {

  void BattleLayer::SetupBattle()
  {
    MoveRegistry::RegisterMoveFunctions();
    MoveRegistry::RegisterStatusFunctions();

    m_battlesystem.AddCharacters(FlexECS::Manager::GetInstance().GetActiveScene()->CachedQuery<Character>());
    m_battlesystem.BeginBattle();
  }

  void BattleLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();
  }

  void BattleLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void BattleLayer::Update()
  {
    // Testing
    if (Input::GetKeyDown(GLFW_KEY_F))
    {
      //Log::Debug("Value of the pointer is: " + std::to_string(*tempptr));

      //std::shared_ptr<FlexECS::Scene> test_scene = FlexECS::Manager::GetInstance().GetActiveScene();
      //ScriptingSystem::ecs_functions["RunPhysicsSystem"](test_scene);

      FlexECS::Manager& manager = FlexECS::Manager::GetInstance();
      IScript* fn = ScriptingSystem::ecs_functions["CreateAddPhysicsEntityScript"];
      if (fn != nullptr)
        fn->Update(manager);
      else
        Log::Error("Function not found");
    }

    if (Input::GetKeyDown(GLFW_KEY_4)) SetupBattle(); // Set Up Battle

    // Include a check whether battle system has been activated
    auto scene = FlexECS::Manager::GetInstance().GetActiveScene();
    if (!scene->CachedQuery<BattleSlot>().empty()) m_battlesystem.Update();

    if (!scene->CachedQuery<CharacterInput, Character>().empty()) {
      // Get the current camera (SO COOLLLL!)
      FlexECS::Entity cam_entity = CameraManager::GetMainCamera();
      auto& curr_cam = cam_entity.GetComponent<Camera>()->camera;

      for (auto& s : scene->CachedQuery<CharacterInput>()) {
        // Set Camera Position to Player with an offset lah
        curr_cam.position = s.GetComponent<Position>()->position - Vector2(600, 500);
        if (Input::GetKey(GLFW_KEY_W)) {
          s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->up_cols;
          s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->up_movement;
        }
        else if (Input::GetKey(GLFW_KEY_A)) {
          s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->left_cols;
          s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->left_movement;
        }
        else if (Input::GetKey(GLFW_KEY_S)) {
          s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->down_cols;
          s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->down_movement;
        }
        else if (Input::GetKey(GLFW_KEY_D)) {
          s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->right_cols;
          s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->right_movement;
        }
        else {
          s.GetComponent<Animation>()->cols = s.GetComponent<CharacterMovementSprites>()->idle_cols;
          s.GetComponent<Animation>()->spritesheet = s.GetComponent<CharacterMovementSprites>()->idle_movement;
        }
      }
      // Updating camera position
      CameraManager::UpdateData(cam_entity, curr_cam);
    }

    for (auto& entity : FlexECS::Manager::GetInstance().GetActiveScene()->CachedQuery<CharacterInput>())
    {
      entity.GetComponent<CharacterInput>()->up = Input::GetKey(GLFW_KEY_W);
      entity.GetComponent<CharacterInput>()->down = Input::GetKey(GLFW_KEY_S);
      entity.GetComponent<CharacterInput>()->left = Input::GetKey(GLFW_KEY_A);
      entity.GetComponent<CharacterInput>()->right = Input::GetKey(GLFW_KEY_D);
    }

    for (auto& entity : FlexECS::Manager::GetInstance().GetActiveScene()->CachedQuery<CharacterInput, Rigidbody>())
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
    
    RenderSprite2D();
    UpdatePhysicsSystem();
  }
}