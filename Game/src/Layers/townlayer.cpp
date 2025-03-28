// WLVERSE [https://wlverse.web.app]
// townlayer.cpp
//
// CPP file for town layer where user can control main character to walk around the town scene
//
// AUTHORS
// [80%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
// [20%] Ng Jia Cheng (n.jiacheng\@digipen.edu)
//   - Co Author
// Copyright (c) 2025 DigiPen, All rights reserved.
#include "Layers.h"
#include "Physics/physicssystem.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  extern std::string town_version;
  void TownLayer::OnAttach()
  {
    //File& file = File::Open(Path::current("assets/saves/town_v4.flxscene"));
      File& file = File::Open(Path::current(town_version));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

    // Trigger music to start
    FlexECS::Scene::GetEntityByName("Town BGM").GetComponent<Audio>()->should_play = true;

    if (town_version == "assets/saves/town_v5_pp.flxscene")
    {
        FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position = Vector3(707.943f, -172.714f, 0);
    }
    FlexECS::Entity camera = CameraManager::GetMainGameCameraID();
    camera.GetComponent<Position>()->position = FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position;
  }

  void TownLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceFadeOut(1.f);
  }

  void TownLayer::Update()
  {
    
#pragma region Camera Follow System

    // move camera to follow main character
    FlexECS::Entity camera = CameraManager::GetMainGameCameraID();

    FlexECS::Entity main_character;
    main_character = FlexECS::Scene::GetEntityByName("Renko");

    

    // Option 1: Damped Spring Effect ("lagging behind" effect like a spring following the player)
    //float damping = 5.0f; // Higher values = snappier response
    //camera.GetComponent<Position>()->position += (main_character.GetComponent<Position>()->position -
    //  camera.GetComponent<Position>()->position) * damping * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

    // Option 2: Lerp (Linear Interpolation) for smooth camera movement over time
    float lerpFactor = 0.1f; // Adjust as needed (higher = faster, lower = smoother)
    camera.GetComponent<Position>()->position.x =
      lerp(camera.GetComponent<Position>()->position.x,
                main_character.GetComponent<Position>()->position.x,
                lerpFactor);
    camera.GetComponent<Position>()->position.y =
      lerp(camera.GetComponent<Position>()->position.y,
                main_character.GetComponent<Position>()->position.y,
                lerpFactor);

    //camera.GetComponent<Position>()->position = main_character.GetComponent<Position>()->position;
    camera.GetComponent<Position>()->position.x = std::clamp(camera.GetComponent<Position>()->position.x, -680.f, 510.f);
    camera.GetComponent<Position>()->position.y = std::clamp(camera.GetComponent<Position>()->position.y, -880.f, 730.f);

#pragma endregion
#pragma region Scene Transition
   /*if (main_enemy.GetComponent<BoundingBox2D>()->is_colliding)
   {
     // transition lorhhhhhhhh
     Application::MessagingSystem::Send("Enter Boss", true);
   }*/
    if (town_version == "assets/saves/town_v5_pp.flxscene")
    {
        if (!FlexECS::Scene::GetEntityByName("Jack").GetComponent<Transform>()->is_active && FlexECS::Scene::GetEntityByName("Jack Collider").GetComponent<BoundingBox2D>()->is_colliding)
        {
            FlexECS::Scene::GetEntityByName("Jack Collider").RemoveComponent<BoundingBox2D>();
            FlexECS::Scene::GetEntityByName("Jack").GetComponent<Transform>()->is_active = true;
        }

        if (FlexECS::Scene::GetEntityByName("Jack").GetComponent<BoundingBox2D>()->is_colliding)
        {
            Application::MessagingSystem::Send("Enter Boss", true);
        }
    }
    else
    {

        if (FlexECS::Scene::GetEntityByName("Encounter1").GetComponent<BoundingBox2D>()->is_colliding)
        {
            Application::MessagingSystem::Send("Enter Battle 1", true);
        }
    }
#pragma endregion

    // Randomly toggle light on and off between range 1 to 2 seconds only in the other scene, but of course when added this can be added to the other scene as well
    if (town_version == "assets/saves/town_v4_2.flxscene")
    {
      static float light_timer = rand() % 1 + 1;
      light_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      if (light_timer <= 0)
      {
        // Lazy toggle, on and off
        FlexECS::Scene::GetEntityByName("Flicker").GetComponent<Transform>()->is_active = 
          !FlexECS::Scene::GetEntityByName("Flicker").GetComponent<Transform>()->is_active;
      
        light_timer = rand() % 1 + 1;
      }
    }
    
  }
  float TownLayer::lerp(float a, float b, float t) {
    return a + t * (b - a);
  }
} // namespace Game
