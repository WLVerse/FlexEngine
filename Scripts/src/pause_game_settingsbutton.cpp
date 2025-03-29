/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// settingsbutton.cpp
//
// This script handles the interactions for the key inputs used by the settings
// button inside the pause menu. It displays the volume settings of the game
// that the user can edit, after the user has selected the settings button.
// 
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class SettingsButtonScript : public IScript
{
private:
  std::array<bool, 4> volume_buttons = { false, false, false, false };
  FlexECS::Entity& active_entity = FlexECS::Entity::Null;
  bool is_volume = false;
public:
  SettingsButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "SettingsButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (!is_volume) {
        if (Input::GetKeyDown(GLFW_KEY_W)) {
          Input::Cleanup();
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
            if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = false;
          }
          Application::MessagingSystem::Send("Active Resume Button", true);
        }
        if (Input::GetKeyDown(GLFW_KEY_S)) {
          Input::Cleanup();
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
            if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = false;
          }
          Application::MessagingSystem::Send("Active How Button", true);
        }
        if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
          Input::Cleanup();
          Application::MessagingSystem::Send("Resume Game", true);
        }
        if (Input::GetKeyDown(GLFW_KEY_SPACE)) {
          Input::Cleanup();
          active_entity = FlexECS::Scene::GetEntityByName("Master Volume Sprite");
          is_volume = true;
        }
      }
      else
      {
        volume_buttons[0] = Application::MessagingSystem::Receive<bool>("Active Master Volume");
        volume_buttons[1] = Application::MessagingSystem::Receive<bool>("Active BGM Volume");
        volume_buttons[2] = Application::MessagingSystem::Receive<bool>("Active SFX Volume");
        volume_buttons[3] = Application::MessagingSystem::Receive<bool>("Active Display Mode");

        if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
          active_entity.GetComponent<Transform>()->is_active = false;
          active_entity = FlexECS::Entity::Null;
          Application::MessagingSystem::Send("Active Settings Button", true);
          is_volume = false;
        }
        if (std::any_of(volume_buttons.begin(), volume_buttons.end(), [](bool state) {
          return state;
        })) {
          active_entity.GetComponent<Transform>()->is_active = false;

          if (volume_buttons[0]) active_entity = FlexECS::Scene::GetEntityByName("Master Volume Sprite");
          else if (volume_buttons[1]) active_entity = FlexECS::Scene::GetEntityByName("BGM Volume Sprite");
          else if (volume_buttons[2]) active_entity = FlexECS::Scene::GetEntityByName("SFX Volume Sprite");
          else if (volume_buttons[3]) active_entity = FlexECS::Scene::GetEntityByName("Display Mode Sprite");

          active_entity.GetComponent<Scale>()->scale.x = 0.f;
          active_entity.GetComponent<Transform>()->is_active = true;
        }

        if (active_entity.GetComponent<Scale>()->scale.x != active_entity.GetComponent<Slider>()->original_scale.x) {
          active_entity.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
          active_entity.GetComponent<Scale>()->scale.x = std::clamp(active_entity.GetComponent<Scale>()->scale.x,
            0.f, active_entity.GetComponent<Slider>()->original_scale.x);
        }
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
        if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = true;
      }
      Application::MessagingSystem::Send("Active Settings Button", true);
      is_volume = false;
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) &&
      FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active) {
      is_volume = true;
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static SettingsButtonScript SettingsButton;
