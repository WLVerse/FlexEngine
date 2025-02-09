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
#pragma region Create the entities
#if 1
    {
      auto scene = FlexECS::Scene::CreateScene();
      FlexECS::Scene::SetActiveScene(scene);
      {
        FlexECS::Entity entity = scene->CreateEntity("Base Town Image");
        entity.AddComponent<Position>({
          { 750, 400, 0 }
        });
        entity.AddComponent<Scale>({
          { 3200, 2688, 0 }
        });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Transform>({});
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/env/Env_Town_Build_M4_03.png)") });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Overlay Town Image");
        entity.AddComponent<Position>({
          { 750, 400, 0 }
        });
        entity.AddComponent<Scale>({
          { 3200, 2688, 0 }
        });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Transform>({});
        entity.AddComponent<ZIndex>({ 2 });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/env/Env_Town_Build_M4_03_+DE.png)") });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Stranded Flaming Barrel");
        entity.AddComponent<Position>({
          { -325, 350, 0 }
        });
        entity.AddComponent<Scale>({
          { 128, 192, 0 }
        });
        entity.AddComponent<Rotation>({
          { 180, 0, 0 }
        });
        entity.AddComponent<Transform>({});
        entity.AddComponent<ZIndex>({ 3 });
        entity.AddComponent<Sprite>({});
        entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Prop_Flaming_Barrel.flxspritesheet)") });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Stranded Flaming Barrel Shadow");
        entity.AddComponent<Position>({
          { -300, 325, 0 }
        });
        entity.AddComponent<Scale>({
          { 128, 192, 0 }
        });
        entity.AddComponent<Rotation>({
          { 0, 0, -45 }
        });
        entity.AddComponent<Transform>({});
        entity.AddComponent<ZIndex>({ 2 });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/vfx/VFX_Char_Shadow.png)") });
      }
      {
        main_character = scene->CreateEntity("Walking Renko");
        main_character.AddComponent<Position>({
          { 800, 350, 0 }
        });
        main_character.AddComponent<Scale>({});
        main_character.AddComponent<Rotation>({});
        main_character.AddComponent<Transform>({});
        main_character.AddComponent<ZIndex>({ 1 });
        main_character.AddComponent<Rigidbody>({});
        main_character.AddComponent<BoundingBox2D>({});
        main_character.AddComponent<Sprite>({});

        main_character.AddComponent<Animator>({});
        main_character.GetComponent<Animator>()->spritesheet_handle =
          FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
        main_character.GetComponent<Animator>()->default_spritesheet_handle =
          FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
        main_character.GetComponent<Animator>()->should_play = true;
        main_character.GetComponent<Animator>()->is_looping = true;
        main_character.GetComponent<Animator>()->return_to_default = false;

        main_character.AddComponent<Script>({ FLX_STRING_NEW("MovePlayer") });
      }
      {
        area_to_transition = scene->CreateEntity("Area To Transition");
        area_to_transition.AddComponent<Position>({
          { 1500, 350, 0 }
        });
        area_to_transition.AddComponent<Scale>({
          { 72, 108, 0 }
        });
        area_to_transition.GetComponent<Scale>()->scale *= 1.5;
        area_to_transition.AddComponent<Rotation>({});
        area_to_transition.AddComponent<Transform>({});
        area_to_transition.AddComponent<Rigidbody>({});
        area_to_transition.AddComponent<BoundingBox2D>({});
      }
      {
        FlexECS::Entity e = scene->CreateEntity("Jack");
        e.AddComponent<Transform>({});
        e.AddComponent<Position>({ area_to_transition.GetComponent<Position>()->position });
        e.AddComponent<Scale>({
          { 72, 108, 0 }
        });
        e.GetComponent<Scale>()->scale *= 1.5;
        e.AddComponent<Rotation>({});
        e.AddComponent<Sprite>({});
        e.AddComponent<Animator>({});
        e.GetComponent<Animator>()->spritesheet_handle =
          FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
        e.GetComponent<Animator>()->default_spritesheet_handle =
          FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
        e.GetComponent<Animator>()->should_play = true;
        e.GetComponent<Animator>()->is_looping = true;
        e.GetComponent<Animator>()->return_to_default = false;

        e.AddComponent<ZIndex>({ 1 });
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

        CameraManager::SetCamera(cam, cam.GetComponent<Camera>()); // set the camera as the main game camera
      }
      {
        FlexECS::Entity bgm = scene->CreateEntity("Town BGM");
        bgm.AddComponent<Position>({});
        bgm.AddComponent<Rotation>({});
        bgm.AddComponent<Scale>({});
        bgm.AddComponent<Transform>({});
        bgm.AddComponent<Audio>({ true, false, true, false, FLX_STRING_NEW("/audio/bgm/town (I already clocked out an hour ago).mp3") });
      }
    }
#endif
#pragma endregion
  }

  void TownLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void TownLayer::Update()
  {
    CameraManager::GetMainGameCamera()->m_data.position = main_character.GetComponent<Position>()->position;
    // PhysicsSystem::UpdatePhysicsSystem();

    if (area_to_transition.GetComponent<BoundingBox2D>()->is_colliding)
    {
      // transition lorhhhhhhhh
      Application::MessagingSystem::Send("Enter Battle", true);
    }
  }
} // namespace Game
