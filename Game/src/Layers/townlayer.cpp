// WLVERSE [https://wlverse.web.app]
// townlayer.cpp
// 
// CPP file for town layer where user can control main character to walk around the town scene
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.
#include "Layers.h"
#include "Physics/physicssystem.h"

namespace Game {
  void TownLayer::OnAttach() {
    #pragma region Create the entities
    #if 1
    {
      auto scene = FlexECS::Scene::CreateScene();
      FlexECS::Scene::SetActiveScene(scene);
      {
        FlexECS::Entity entity = scene->CreateEntity("Base Town Image");
        entity.AddComponent<Position>({ { 750, 400, 0 } });
        entity.AddComponent<Scale>({ { 5000, 4200, 0 } });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Transform>({});
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/Env_Town_Build_M4_02.png)") });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Overlay Town Image");
        entity.AddComponent<Position>({ { 750, 516.5f, 0 } });
        entity.AddComponent<Scale>({ { 5000, 3976.5f, 0 } });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Transform>({});
        entity.AddComponent<ZIndex>({ 2 });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/Env_Town_Build_M4_02_+DE.png)") });
      }
      {
        main_character = scene->CreateEntity("Walking Renko");
        main_character.AddComponent<Position>({ {800, 350, 0} });
        main_character.AddComponent<Scale>({ {80, 186, 0} });
        main_character.AddComponent<Rotation>({ {180, 0, 0} });
        main_character.AddComponent<Transform>({});
        main_character.AddComponent<ZIndex>({ 1 });
        main_character.AddComponent<Rigidbody>({});
        main_character.AddComponent<BoundingBox2D>({});
        main_character.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_renko.png)") });
        main_character.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)") });
        main_character.AddComponent<Script>({ FLX_STRING_NEW("MovePlayer") });
      }
      {
        area_to_transition = scene->CreateEntity("Area To Transition");
        area_to_transition.AddComponent<Position>({ {3000, 0, 0} });
        area_to_transition.AddComponent<Scale>({ {1350, 950, 0} });
        area_to_transition.AddComponent<Rotation>({});
        area_to_transition.AddComponent<Transform>({});
        area_to_transition.AddComponent<Rigidbody>({});
        area_to_transition.AddComponent<BoundingBox2D>({});
      }
      // Camera Test
      {
        FlexECS::Entity cam = scene->CreateEntity("Test Cam");
        cam.AddComponent<Position>({});
        cam.AddComponent<Rotation>({});
        cam.AddComponent<Scale>({});
        cam.AddComponent<Transform>({});
        // There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
        // each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        //  Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera({ 850.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam.AddComponent<Camera>(gameTestCamera);
      }
    }
    #endif
  }
  void TownLayer::OnDetach() {

  }
  void TownLayer::Update() {
    CameraManager::GetMainGameCamera()->m_data.position = main_character.GetComponent<Position>()->position;
    PhysicsSystem::UpdatePhysicsSystem();

    if (area_to_transition.GetComponent<BoundingBox2D>()->is_colliding) {
      // transition lorhhhhhhhh
      Application::MessagingSystem::Send("Enter Battle", true);
    }
  }
}