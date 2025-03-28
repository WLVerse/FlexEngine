/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// resumebutton.cpp
//
// This script handles the interactions for the key inputs used by the resume
// button inside the pause menu. It returns the user back to the game whenever
// the user clicks on the button.
// 
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class ResumeButtonScript : public IScript
{
public:
  ResumeButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "ResumeButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active Quit Game Button", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
          if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = true;
        }
        FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
        Application::MessagingSystem::Send("Active Settings Button", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE) || Input::GetKeyDown(GLFW_KEY_ENTER) || Input::GetKeyDown(GLFW_KEY_SPACE)) {
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
        //if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>())
        entity.GetComponent<Transform>()->is_active = false;
      }
      Application::MessagingSystem::Send("Active Resume Button", true);
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) &&
      FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Resume Game", true);
    }
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static ResumeButtonScript ResumeButton;
