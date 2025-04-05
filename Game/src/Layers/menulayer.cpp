/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// menulayer.h / .cpp
//
// Declares the MenuLayer class, which is responsible for managing the
// main menu interface, including UI initialization, input handling,
// and transition between menu states.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include "Layers.h"
#include "FlexEngine.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  std::vector<FlexECS::Entity> menu_buttons;
  int selected_button = 0;
  bool open_settings = false;

  std::array<bool, 6> pause_buttons;

  std::string active_entity;

  void Set_Up_Settings_Menu() {
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
  }

  void Settings_Menu_Functionality() {
    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI>()) {
      bool& state_to_set = entity.GetComponent<Transform>()->is_active;
      if (entity.HasComponent<PauseHoverUI>() || entity.HasComponent<SettingsUI>()) state_to_set = false;
      else state_to_set ^= true;
    }

    open_settings ^= true;
    if (open_settings) {
      active_entity = "Master Volume Sprite";
      FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
    }
  }

  void MenuLayer::OnAttach()
  {
    File& file = File::Open(Path::current("assets/saves/mainmenu_v9.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
    
    // Trigger music to start
    FlexECS::Scene::GetEntityByName("Main Menu BGM").GetComponent<Audio>()->should_play = true;

    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Start Game"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Settings"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Quit Game"));
    
    // find camera
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
    {
      CameraManager::SetMainGameCameraID(entity);
      break;
    }

    FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Hover.png";
    Set_Up_Settings_Menu();

    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<VideoPlayer>())
    {
      auto& video = FLX_ASSET_GET(VideoDecoder, FLX_STRING_GET(element.GetComponent<VideoPlayer>()->video_file));
      video.RestartVideo();
    }
    if (Application::GetCurrentWindow()->GetFullScreen()) {
      FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Display Mode Value").GetComponent<Text>()->text) = "Fullscreen";
    }
    else FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Display Mode Value").GetComponent<Text>()->text) = "Windowed";

    Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 1, 1.0 });
  }

  void MenuLayer::OnDetach()
  {
    // No longer need to stop sound as the cutscene is using it...
    // Make sure nothing carries over in the way of sound
    // FMODWrapper::Core::ForceStop();
  }

  void MenuLayer::Update()
  {
    bool return_to_menu = Application::MessagingSystem::Receive<bool>("Return to Menu");
    bool start_options = Application::MessagingSystem::Receive<bool>("OptionStart");
    std::pair<std::string, bool> active_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Pause Sprite");

    // check for escape key
    if (return_to_menu || start_options) Settings_Menu_Functionality();

    if (open_settings) {
      if (active_sprite.second) {
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName(active_sprite.first).GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName(active_sprite.first).GetComponent<Transform>()->is_active = true;
        active_entity = active_sprite.first;
      }
      
      if (FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Scale>()->scale.x !=
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Slider>()->original_scale.x) {
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Scale>()->scale.x +=
          Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Scale>()->scale.x =
          std::clamp(FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Scale>()->scale.x,
          0.f, FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Slider>()->original_scale.x);
      }

      return;
    }

    if (Input::GetKeyDown(GLFW_KEY_S))
    {
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Normal.png";
      ++selected_button;
      if (selected_button > 2)
        selected_button = 0;
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Hover.png";
    }
    else if (Input::GetKeyDown(GLFW_KEY_W))
    {
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Normal.png";
      --selected_button;
      if (selected_button < 0)
        selected_button = 2;
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Hover.png";
    }

    if (Input::GetKeyDown(GLFW_KEY_SPACE))
    {
      // shit copy paste
      if (selected_button == 0)
      {
        Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 2, 0.5 });
      }
      else if (selected_button == 1) Settings_Menu_Functionality();
      else if (selected_button == 2)
      {
        Application::QueueCommand(Application::Command::QuitApplication);
      }
    }

    int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
    if (transitionMSG == 2)
    {
        Application::MessagingSystem::Send("Start Cutscene", true);
    }
  }
}
