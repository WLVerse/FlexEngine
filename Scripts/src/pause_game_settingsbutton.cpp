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
  std::string active_entity;
  bool is_volume = false;
  bool editing_volume = false;
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
        active_entity = "Master Volume Sprite";
        is_volume = true;
      }
      if (Input::GetKeyDown(GLFW_KEY_W) && !editing_volume) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Transform>()->is_active = false;
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Resume Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_S) && !editing_volume) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Transform>()->is_active = false;
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "How Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
        Input::Cleanup();
        if (editing_volume) {
          FlexECS::Scene::GetEntityByName("ButtonPress").GetComponent<Audio>()->should_play = true;
          editing_volume = false;
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, PauseHoverUI, SettingsUI>()) {
            entity.GetComponent<Transform>()->is_active = false;
          }
        }
        else Application::MessagingSystem::Send("Resume Game", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_SPACE)) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName("ButtonPress").GetComponent<Audio>()->should_play = true;
        Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "Master Volume Sprite", true});
        FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
        editing_volume = true;
      }
    }
    else {
      if (is_volume) is_volume = false;
      if (editing_volume) editing_volume = false;
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Settings Button Sprite", true});
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "Master Volume Sprite", true});
      FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
      editing_volume = true;
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static SettingsButtonScript SettingsButton;
