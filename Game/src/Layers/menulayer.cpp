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

  FlexECS::Entity& active_entity = FlexECS::Entity::Null;

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
      active_entity = FlexECS::Scene::GetEntityByName("Master Volume Sprite");
      FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
    }
  }

  void MenuLayer::OnAttach()
  {
    File& file = File::Open(Path::current("assets/saves/mainmenu_v6.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
    
    // Trigger music to start
    FlexECS::Scene::GetEntityByName("Main Menu BGM").GetComponent<Audio>()->should_play = true;

    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Start Game"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Settings"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Quit"));
    
    // find camera
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
    {
      CameraManager::SetMainGameCameraID(entity);
      break;
    }

    FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Hover.png";
    Set_Up_Settings_Menu();
    Application::GetCurrentWindow()->ToggleFullScreen(false);
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
    pause_buttons[0] = Application::MessagingSystem::Receive<bool>("Active Master Volume");
    pause_buttons[1] = Application::MessagingSystem::Receive<bool>("Active BGM Volume");
    pause_buttons[2] = Application::MessagingSystem::Receive<bool>("Active SFX Volume");
    pause_buttons[3] = Application::MessagingSystem::Receive<bool>("Active Display Mode");
    pause_buttons[4] = Application::MessagingSystem::Receive<bool>("Active Return Button");
    pause_buttons[5] = Application::MessagingSystem::Receive<bool>("Active Quit Button");

    // check for escape key
    if (return_to_menu) Settings_Menu_Functionality();

    if (open_settings) {
      if (std::any_of(pause_buttons.begin(), pause_buttons.end(), [](bool state) {
        return state;
      })) {
        active_entity.GetComponent<Transform>()->is_active = false;
        
        if (pause_buttons[0]) active_entity = FlexECS::Scene::GetEntityByName("Master Volume Sprite");
        else if (pause_buttons[1]) active_entity = FlexECS::Scene::GetEntityByName("BGM Volume Sprite");
        else if (pause_buttons[2]) active_entity = FlexECS::Scene::GetEntityByName("SFX Volume Sprite");
        else if (pause_buttons[3]) active_entity = FlexECS::Scene::GetEntityByName("Display Mode Sprite");
        else if (pause_buttons[4]) active_entity = FlexECS::Scene::GetEntityByName("Return Button Sprite");
        else if (pause_buttons[5]) active_entity = FlexECS::Scene::GetEntityByName("Quit Button Sprite");
          
        active_entity.GetComponent<Scale>()->scale.x = 0.f;
        active_entity.GetComponent<Transform>()->is_active = true;
      }
      
      if (active_entity.GetComponent<Scale>()->scale.x != active_entity.GetComponent<Slider>()->original_scale.x) {
        active_entity.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        active_entity.GetComponent<Scale>()->scale.x = std::clamp(active_entity.GetComponent<Scale>()->scale.x,
          0.f, active_entity.GetComponent<Slider>()->original_scale.x);
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
