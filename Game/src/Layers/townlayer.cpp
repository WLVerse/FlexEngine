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

    if (town_version == "assets/saves/town_v4_2.flxscene")
    {
        FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position = Vector3(707.943, -172.714, 0);
    }
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

    camera.GetComponent<Position>()->position = main_character.GetComponent<Position>()->position;
    camera.GetComponent<Position>()->position.x = std::clamp(camera.GetComponent<Position>()->position.x, -880.f, 710.f);
    camera.GetComponent<Position>()->position.y = std::clamp(camera.GetComponent<Position>()->position.y, -880.f, 880.f);

#pragma endregion
#pragma region Scene Transition
   /*if (main_enemy.GetComponent<BoundingBox2D>()->is_colliding)
   {
     // transition lorhhhhhhhh
     Application::MessagingSystem::Send("Enter Boss", true);
   }*/
    if (town_version == "assets/saves/town_v4_2.flxscene")
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
    /*
#pragma region Renko X Barrel z-index resolver

    //// resolve z-index of renko and barrel
    //FlexECS::Entity barrel = FlexECS::Scene::GetEntityByName("Stranded Flaming Barrel");
    //float character_y = main_character.GetComponent<Position>()->position.y - main_character.GetComponent<Scale>()->scale.y / 2;
    //float barrel_y = barrel.GetComponent<Position>()->position.y - barrel.GetComponent<Scale>()->scale.y / 2;
    //main_character.GetComponent<ZIndex>()->z =
    //  (character_y < barrel_y) ? barrel.GetComponent<ZIndex>()->z + 1 : barrel.GetComponent<ZIndex>()->z - 1;

#pragma endregion


    
    */
  }
} // namespace Game
