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
  void TownLayer::OnAttach()
  {
    File& file = File::Open(Path::current("assets/saves/townscene.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

    // Trigger music to start
    //FlexECS::Scene::GetEntityByName("Town BGM").GetComponent<Audio>()->should_play = true;
    //main_character = FlexECS::Scene::GetEntityByName("Walking Renko");
    //area_to_transition = FlexECS::Scene::GetEntityByName("Area To Transition");
  }

  void TownLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void TownLayer::Update()
  {
    /*
#pragma region Camera Follow System

    // move camera to follow main character
    FlexECS::Entity camera = CameraManager::GetMainGameCameraID();
    camera.GetComponent<Position>()->position = main_character.GetComponent<Position>()->position;

#pragma endregion

#pragma region Renko X Barrel z-index resolver

    //// resolve z-index of renko and barrel
    //FlexECS::Entity barrel = FlexECS::Scene::GetEntityByName("Stranded Flaming Barrel");
    //float character_y = main_character.GetComponent<Position>()->position.y - main_character.GetComponent<Scale>()->scale.y / 2;
    //float barrel_y = barrel.GetComponent<Position>()->position.y - barrel.GetComponent<Scale>()->scale.y / 2;
    //main_character.GetComponent<ZIndex>()->z =
    //  (character_y < barrel_y) ? barrel.GetComponent<ZIndex>()->z + 1 : barrel.GetComponent<ZIndex>()->z - 1;

#pragma endregion


    if (area_to_transition.GetComponent<BoundingBox2D>()->is_colliding)
    {
      // transition lorhhhhhhhh
      Application::MessagingSystem::Send("Enter Battle", true);
    }
    */
  }
} // namespace Game
