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

  // Pause Buttons
  std::string active_pause_button;
  std::string active_volume_button;
  bool is_paused = false;
  Vector3 original_camera_pos;

  void Set_Up_Town_Pause_Menu() {
    #pragma region Load pause menu data
    std::array<std::string, 9> slider_names = {
      "SFX Slider Background",
      "SFX Slider Fill",
      "SFX Knob",
      "BGM Slider Background",
      "BGM Slider Fill",
      "BGM Knob",
      "Master Slider Background",
      "Master Slider Fill",
      "Master Knob"
    };
    for (int i = 0; i < 9; i += 3) {
      FlexECS::Entity tempEntity = FlexECS::Scene::GetEntityByName(slider_names[i]);

      float min_value = tempEntity.GetComponent<Position>()->position.x - tempEntity.GetComponent<Sprite>()->scale.x - 20.f;
      float max_value = tempEntity.GetComponent<Position>()->position.x + tempEntity.GetComponent<Sprite>()->scale.x + 15.f;

      FlexECS::Entity sliderEntity = FlexECS::Scene::GetEntityByName(slider_names[i + 1]);
      sliderEntity.GetComponent<Slider>()->min_position = min_value;
      sliderEntity.GetComponent<Slider>()->max_position = max_value;
      sliderEntity.GetComponent<Slider>()->slider_length = max_value - min_value;
      sliderEntity.GetComponent<Slider>()->original_value = ((FlexECS::Scene::GetEntityByName(slider_names[i + 2]).GetComponent<Position>()->position.x - min_value)) / (max_value - min_value);

      // Set Up Volume Positions
      if (i == 0) {
        FlexECS::Scene::GetEntityByName("SFX Knob").GetComponent<Position>()->position.x =
          FMODWrapper::Core::GetGroupVolume(FMODWrapper::Core::CHANNELGROUP::SFX) * (max_value - min_value) + min_value;
      }
      else if (i == 3) {
        FlexECS::Scene::GetEntityByName("BGM Knob").GetComponent<Position>()->position.x =
          FMODWrapper::Core::GetGroupVolume(FMODWrapper::Core::CHANNELGROUP::BGM) * (max_value - min_value) + min_value;
      }
      else if (i == 6) {
        FlexECS::Scene::GetEntityByName("Master Knob").GetComponent<Position>()->position.x = max_value;
      }
    }
    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PauseUI, Slider>()) {
      entity.GetComponent<Slider>()->original_scale = entity.GetComponent<Scale>()->scale;
    }
    if (Application::GetCurrentWindow()->GetFullScreen()) {
      FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Display Mode Value").GetComponent<Text>()->text) = "Fullscreen";
    }
    else FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Display Mode Value").GetComponent<Text>()->text) = "Windowed";
    #pragma endregion
  }

  void Town_Pause_Functionality() {
    is_paused ^= true;
    FlexECS::Entity cam = CameraManager::GetMainGameCameraID();

    if (is_paused) {
      cam.GetComponent<Position>()->position = original_camera_pos;
      active_pause_button = "Resume Button Sprite";
      active_volume_button = "Master Volume Sprite";
    }
    else cam.GetComponent<Position>()->position = FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position;

    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI>()) {
      bool& state_to_set = entity.GetComponent<Transform>()->is_active;
      if (entity.HasComponent<PauseHoverUI>() || entity.HasComponent<CreditsUI>() || entity.HasComponent<QuitUI>()) state_to_set = false;
      else state_to_set ^= true;
    }

    if (is_paused) FlexECS::Scene::GetEntityByName("Resume Button Sprite").GetComponent<Transform>()->is_active = true;
  }

  void TownLayer::OnAttach()
  {
    //File& file = File::Open(Path::current("assets/saves/town_v4.flxscene"));
    File& file = File::Open(Path::current(town_version));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

    // Trigger music to start
    FlexECS::Scene::GetEntityByName("Town BGM").GetComponent<Audio>()->should_play = true;

    if (town_version == "assets/saves/town_v7_2.flxscene")
    {
      FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position = Vector3(707.943f, -172.714f, 0);
    }
    FlexECS::Entity camera = CameraManager::GetMainGameCameraID();
    original_camera_pos = camera.GetComponent<Position>()->position;
    camera.GetComponent<Position>()->position = FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position;

    Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 4, 1.2 });
    
    //if (town_version == "assets/saves/town_v7.flxscene")
    Set_Up_Town_Pause_Menu();
  }

  void TownLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceFadeOut(1.f);
  }

  void TownLayer::Update()
  {
    //if (town_version == "assets/saves/town_v7.flxscene") {
    std::pair<std::string, bool> active_pause_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Pause Sprite");
    std::pair<std::string, bool> active_volume_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Volume Sprite");
    
    bool resume_game = Application::MessagingSystem::Receive<bool>("Resume Game");
    // check for escape key
    if ((Input::GetKeyDown(GLFW_KEY_ESCAPE) && !is_paused) || resume_game)
    {
      Town_Pause_Functionality();
    }
    
    FlexECS::Entity cam = CameraManager::GetMainGameCameraID();

    if (is_paused) {
      cam.GetComponent<Camera>()->SetOrthographic(-1920 / 2.f, 1080 / 2.f, -1920 / 2.f, 1080 / 2.f);

      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>()) {
        if (!entity.GetComponent<Transform>()->is_active) break;
        entity.GetComponent<PostProcessingMarker>()->enableGaussianBlur = true;
      }

      if (!FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active &&
            !FlexECS::Scene::GetEntityByName("Quit Game Confirmation Prompt").GetComponent<Transform>()->is_active) {
        if (active_pause_sprite.second) {

          FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Transform>()->is_active = false;
          FlexECS::Scene::GetEntityByName(active_pause_sprite.first).GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName(active_pause_sprite.first).GetComponent<Transform>()->is_active = true;
          active_pause_button = active_pause_sprite.first;

          if (active_pause_button != "Settings Button Sprite") {
            for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, PauseHoverUI, SettingsUI>()) {
              entity.GetComponent<Transform>()->is_active = false;
            }
          }
        }

        if (active_volume_sprite.second) {
          if (active_pause_button != "Settings Button Sprite") {
            FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
            FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active = true;
            active_pause_button = "Settings Button Sprite";
          }
          FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Transform>()->is_active = false;
          FlexECS::Scene::GetEntityByName(active_volume_sprite.first).GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName(active_volume_sprite.first).GetComponent<Transform>()->is_active = true;
          active_volume_button = active_volume_sprite.first;
        }
      }

      if (FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x !=
          FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Slider>()->original_scale.x) {
        FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x +=
          Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x =
          std::clamp(FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x,
          0.f, FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Slider>()->original_scale.x);
      }

      if (FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x !=
          FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Slider>()->original_scale.x) {
        FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x +=
          Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x =
          std::clamp(FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x,
          0.f, FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Slider>()->original_scale.x);
      }
      return;
    }
    else {
      cam.GetComponent<Camera>()->SetOrthographic(-cam.GetComponent<Camera>()->GetOrthoWidth() / 2.f,
      cam.GetComponent<Camera>()->GetOrthoWidth() / 2.f,
      -cam.GetComponent<Camera>()->GetOrthoHeight() / 2.f,
      cam.GetComponent<Camera>()->GetOrthoHeight() / 2.f);
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>()) {
        if (!entity.GetComponent<Transform>()->is_active) break;
        entity.GetComponent<PostProcessingMarker>()->enableGaussianBlur = false;
      }
    }
    
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
    camera.GetComponent<Position>()->position.x = std::clamp(camera.GetComponent<Position>()->position.x, -1080.f, 710.f);
    camera.GetComponent<Position>()->position.y = std::clamp(camera.GetComponent<Position>()->position.y, -275.f, 930.f);

#pragma endregion
#pragma region Scene Transition
   /*if (main_enemy.GetComponent<BoundingBox2D>()->is_colliding)
   {
     // transition lorhhhhhhhh
     Application::MessagingSystem::Send("Enter Boss", true);
   }*/
    static bool startcombat = false;
    if (town_version == "assets/saves/town_v7_2.flxscene")
    {
        if (!FlexECS::Scene::GetEntityByName("Jack").GetComponent<Transform>()->is_active && FlexECS::Scene::GetEntityByName("Jack Collider").GetComponent<BoundingBox2D>()->is_colliding)
        {
            FlexECS::Scene::GetEntityByName("Jack Collider").RemoveComponent<BoundingBox2D>();
            FlexECS::Scene::GetEntityByName("Jack").GetComponent<Transform>()->is_active = true;
        }

        
        if (FlexECS::Scene::GetEntityByName("Jack").GetComponent<BoundingBox2D>()->is_colliding && !startcombat)
        {
            Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 3, 1.2 });
            startcombat = true;
            // Awful code to delete renko's script so he can't move haha...
            FlexECS::Entity renko = FlexECS::Scene::GetEntityByName("Renko");
            renko.RemoveComponent<Script>();
            renko.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
        }

        int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
        if (transitionMSG == 3)
        {
            Application::MessagingSystem::Send("Enter Boss", true);
            startcombat = false;
        }
    }
    else
    {

        if (FlexECS::Scene::GetEntityByName("Encounter1").GetComponent<BoundingBox2D>()->is_colliding && !startcombat)
        {
            Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 3, 1.2 });
            startcombat = true;
                        // Awful code to delete renko's script so he can't move haha...
            FlexECS::Entity renko = FlexECS::Scene::GetEntityByName("Renko");
            renko.RemoveComponent<Script>();
            renko.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
        }


        int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
        if (transitionMSG == 3)
        {
            Application::MessagingSystem::Send("Enter Battle 1", true);
            startcombat = false;
        }
    }
#pragma endregion

    // Randomly toggle light on and off between range 1 to 2 seconds only in the other scene, but of course when added this can be added to the other scene as well
    if (town_version == "assets/saves/town_v7_2.flxscene")
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
