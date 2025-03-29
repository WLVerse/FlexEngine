/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// creditsbutton.cpp
//
// This script handles the interactions for the key inputs used by the credits
// button inside the pause menu. It displays the credits when it has been
// selected by the user
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class HowButtonScript : public IScript
{
public:
  HowButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "HowButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
          if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = true;
        }
        FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
        Application::MessagingSystem::Send("Active Settings Button", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active Quit Game Button", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Resume Game", true);
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
        // if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()
        entity.GetComponent<Transform>()->is_active = false;
      }
      Application::MessagingSystem::Send("Active How Button", true);
    }
  }

  void OnMouseStay() override
  {
    //if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    //{
    //  // TODO: Send message to display different assets (Either settings or credits)
    //  for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
    //    entity.GetComponent<Transform>()->is_active = false;
    //  }
    //}
  }

  void OnMouseExit() override
  {
    /*self.GetComponent<Transform>()->is_active = false;*/
  }
};

// Static instance to ensure registration
static HowButtonScript HowButton;
