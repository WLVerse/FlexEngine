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
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
          entity.GetComponent<Transform>()->is_active = false;
        }
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Transform>()->is_active = false;
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Resume Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_S) && !editing_volume) {
        Input::Cleanup();
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
          entity.GetComponent<Transform>()->is_active = false;
        }
        FlexECS::Scene::GetEntityByName(active_entity).GetComponent<Transform>()->is_active = false;
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "How Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
        Input::Cleanup();
        if (editing_volume) {
          editing_volume = false;
          FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = false;
        }
        else Application::MessagingSystem::Send("Resume Game", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_SPACE)) {
        Input::Cleanup();
        active_entity = "Master Volume Sprite";
        FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
        editing_volume = true;
      }
      std::pair<std::string, bool> active_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Volume Sprite");
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
    }
    else {
      if (is_volume) is_volume = false;
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
        if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = true;
      }
      Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Settings Button Sprite", true});
    }
  }

  void OnMouseStay() override
  {

  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static SettingsButtonScript SettingsButton;
