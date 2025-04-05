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
  bool open_controls = false;
  bool start_credits = false;
  bool confirm_quit = false;
  bool yes_or_no = false;

  Audio* button_hover_audio;
  Audio* button_click_audio;

  std::array<bool, 6> pause_buttons;

  std::string active_entity;
  std::pair<FlexECS::Entity, bool> active_credit;
  #pragma region Settings 
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
    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Slider>()) {
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
  #pragma endregion
  #pragma region Quit Confirmation

  void Quit_Menu_No() {
    FlexECS::Scene::GetEntityByName("Quit Yes Sprite").GetComponent<Transform>()->is_active = false;
    FlexECS::Scene::GetEntityByName("Quit No Sprite").GetComponent<Scale>()->scale.x = 0.f;
    FlexECS::Scene::GetEntityByName("Quit No Sprite").GetComponent<Transform>()->is_active = true;
  }

  void Quit_Menu_Yes() {
    FlexECS::Scene::GetEntityByName("Quit No Sprite").GetComponent<Transform>()->is_active = false;
    FlexECS::Scene::GetEntityByName("Quit Yes Sprite").GetComponent<Scale>()->scale.x = 0.f;
    FlexECS::Scene::GetEntityByName("Quit Yes Sprite").GetComponent<Transform>()->is_active = true;
  }

  void Quit_Menu_Functionality() {
    if (Input::GetKeyDown(GLFW_KEY_ESCAPE) || Application::MessagingSystem::Receive<bool>("Cancel Quit")) {
      for (auto& ui : FlexECS::Scene::GetActiveScene()->CachedQuery<QuitUI>()) {
        ui.GetComponent<Transform>()->is_active = false;
      }
      confirm_quit = false;
    }
    if (Input::GetKeyDown(GLFW_KEY_SPACE) || Input::GetKeyDown(GLFW_KEY_ENTER)) {
      Input::Cleanup();
      if (yes_or_no == true) {
        Application::QueueCommand(Application::Command::QuitApplication);
      }
      else {
        for (auto& ui : FlexECS::Scene::GetActiveScene()->CachedQuery<QuitUI>()) {
          ui.GetComponent<Transform>()->is_active = false;
        }
      }
    }
    if (Input::GetKeyDown(GLFW_KEY_A) || Input::GetKeyDown(GLFW_KEY_D)) {
      Input::Cleanup();
      yes_or_no ^= true;
      if (yes_or_no) Quit_Menu_Yes();
      else Quit_Menu_No();
    }
    if (Application::MessagingSystem::Receive<bool>("Quit Yes") &&
        FlexECS::Scene::GetEntityByName("Quit No Sprite").GetComponent<Transform>()->is_active) {
      Quit_Menu_Yes();
    }
    if (Application::MessagingSystem::Receive<bool>("Quit No") &&
      FlexECS::Scene::GetEntityByName("Quit Yes Sprite").GetComponent<Transform>()->is_active) {
      Quit_Menu_No();
    }
  }

  void Quit_Menu_Set_Up() {
    for (auto& ui : FlexECS::Scene::GetActiveScene()->CachedQuery<QuitUI>()) {
      if (!ui.HasComponent<Slider>()) ui.GetComponent<Transform>()->is_active = true;
    }
    FlexECS::Scene::GetEntityByName("Quit Yes Sprite").GetComponent<Transform>()->is_active = true;
    confirm_quit = true;
    yes_or_no = true;
  }
  #pragma endregion
  #pragma region How To Play
  void How_To_Play_Set_Up() {
    FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active = true;
    FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Transform>()->is_active = true;
    FlexECS::Scene::GetEntityByName("Tutorial P1").GetComponent<Transform>()->is_active = true;
    open_controls = true;
    selected_button = 1;
  }

  void How_To_Play_Functionality() {
    bool return_page = Application::MessagingSystem::Receive<bool>("HTPReturnPage");
    bool next_page = Application::MessagingSystem::Receive<bool>("HTPNextPage");

    if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
      Input::Cleanup();
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<CreditsUI>()) {
        entity.GetComponent<Transform>()->is_active = false;
      }
      open_controls = false;
      selected_button = 1;
    }
    if (Input::GetKeyDown(GLFW_KEY_A) || return_page) {
      Input::Cleanup();
      if (selected_button != 1) {
        FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(selected_button)).GetComponent<Transform>()->is_active = false;
        --selected_button;
        FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(selected_button)).GetComponent<Transform>()->is_active = true;
      }
    }
    if (Input::GetKeyDown(GLFW_KEY_D) || next_page) {
      Input::Cleanup();
      if (selected_button != 5) {
        FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(selected_button)).GetComponent<Transform>()->is_active = false;
        ++selected_button;
        FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(selected_button)).GetComponent<Transform>()->is_active = true;
      }
    }
  }
  #pragma endregion
  #pragma region Credits
  void Credits_Set_Up() {
    FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Transform>()->is_active = true;
    FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Sprite>()->opacity = 1.f;
    FlexECS::Scene::GetEntityByName("Credits Page 1").GetComponent<Transform>()->is_active = true;
    active_credit = std::make_pair(FlexECS::Scene::GetEntityByName("Credits Page 1"), false);
    selected_button = 1;
    start_credits = true;
  }
  void Credits_Functionality() {
    if (active_credit.first.GetComponent<Sprite>()->opacity >= 1.f && !active_credit.second) {
      if (Input::AnyKeyDown() || Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        Input::Cleanup();
        active_credit.second = true;
      }
    }
    if (active_credit.first.GetComponent<Sprite>()->opacity <= 0.f && active_credit.second) {
      if (selected_button != 3) ++selected_button;
      else {
        FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Sprite>()->opacity = 0.8f;
        selected_button = 3;
        start_credits = false;
      }
      active_credit = std::make_pair(FlexECS::Scene::GetEntityByName("Credits Page " + std::to_string(selected_button)), false);
    }
    if (Input::AnyKeyDown() || Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
        for (int i = 1; i < 4; i++) {
          FlexECS::Scene::GetEntityByName("Credits Page " + std::to_string(i)).GetComponent<Sprite>()->opacity = 0.f;
        }
        FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Sprite>()->opacity = 0.8f;
        selected_button = 3;
        start_credits = false;
      }
      Input::Cleanup();
      if (start_credits) {
        if (active_credit.second) active_credit.first.GetComponent<Sprite>()->opacity = 0.f;
        else active_credit.first.GetComponent<Sprite>()->opacity = 1.f;
      }
    }

    if (active_credit.second) {
      if (active_credit.first.GetComponent<Sprite>()->opacity > 0.f) {
        active_credit.first.GetComponent<Sprite>()->opacity -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 0.5f;
      }
    }
    else {
      if (active_credit.first.GetComponent<Sprite>()->opacity < 1.f) {
        active_credit.first.GetComponent<Sprite>()->opacity += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 0.5f;
      }
    }
    active_credit.first.GetComponent<Sprite>()->opacity = std::clamp(active_credit.first.GetComponent<Sprite>()->opacity, 0.f, 1.f);
  }
  #pragma endregion
  void MenuLayer::OnAttach()
  {
    File& file = File::Open(Path::current("assets/saves/mainmenu_v10.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
    
    // Trigger music to start
    FlexECS::Scene::GetEntityByName("Main Menu BGM").GetComponent<Audio>()->should_play = true;
    FlexECS::Scene::GetEntityByName("BlowWindBlow").GetComponent<Audio>()->should_play = true;

    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Start Game Sprite"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("How To Play Sprite"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Options Button Sprite"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Credits Button Sprite"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Quit Game Sprite"));

    // Set up button hover and click audio
    button_hover_audio = FlexECS::Scene::GetEntityByName("ButtonHover").GetComponent<Audio>();
    button_click_audio = FlexECS::Scene::GetEntityByName("ButtonPress").GetComponent<Audio>();
    
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
    std::pair<int, bool> mouse_input = Application::MessagingSystem::Receive<std::pair<int, bool>>("Menu Buttons");

    // Enable the quit menu
    if (Application::MessagingSystem::Receive<bool>("Quit Confirmation Start")) {
      Quit_Menu_Set_Up();
    }
    
    // Quit Menu Functionality
    if (confirm_quit) {
      Quit_Menu_Functionality();
      return;
    }

    // Enable the How To Play
    if (Application::MessagingSystem::Receive<bool>("How To Play Start")) {
      How_To_Play_Set_Up();
    }

    // How To Play Menu Functionality
    if (open_controls) {
      How_To_Play_Functionality();
      return;
    }

    // Enable the Credits
    if (Application::MessagingSystem::Receive<bool>("Start Credits")) {
      Credits_Set_Up();
    }

    // Credits Functionality
    if (start_credits) {
      Credits_Functionality();
      return;
    }

    // Enable & Disable the settings menu
    if (return_to_menu || start_options) Settings_Menu_Functionality();

    // Settings Menu Functionality
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

    if (mouse_input.second) {
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Normal.png";
      selected_button = mouse_input.first;
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Hover.png";
    }

    if (Input::GetKeyDown(GLFW_KEY_S))
    {
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Normal.png";
      ++selected_button;
      if (selected_button > 4)
        selected_button = 0;
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Hover.png";

      button_hover_audio->should_play = true;
    }
    else if (Input::GetKeyDown(GLFW_KEY_W))
    {
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Normal.png";
      --selected_button;
      if (selected_button < 0)
        selected_button = 4;
      FLX_STRING_GET(menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle) = "/images/MainMenu/UI_Main_Menu_Button_Hover.png";

      button_hover_audio->should_play = true;
    }

    if (Application::MessagingSystem::Receive<bool>("StartTransitionToGame")) {
      Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 2, 0.5 });
    }

    if (Input::GetKeyDown(GLFW_KEY_SPACE))
    {
      button_click_audio->should_play = true;

      // shit copy paste
      if (selected_button == 0) {
        Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 2, 0.5 });
      }
      else if (selected_button == 1) {
        How_To_Play_Set_Up();
      }
      else if (selected_button == 2) {
        Settings_Menu_Functionality();
      }
      else if (selected_button == 3) {
        Credits_Set_Up();
      }
      else if (selected_button == 4) {
        Quit_Menu_Set_Up();
      }
    }

    int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
    if (transitionMSG == 2) {
        Application::MessagingSystem::Send("Start Cutscene", true);
    }
  }
}
