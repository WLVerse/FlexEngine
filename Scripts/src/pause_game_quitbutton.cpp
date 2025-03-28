/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// quitbutton.cpp
//
// This script handles the interactions for the key inputs used by the quit
// button inside the pause menu. It exits the game when user selects the button
// 
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class ExitButtonScript : public IScript
{
public:
  ExitButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "ExitButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active How Button", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active Resume Button", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_ENTER) || Input::GetKeyDown(GLFW_KEY_SPACE)) {
        Application::QueueCommand(Application::Command::QuitApplication);
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
        entity.GetComponent<Transform>()->is_active = false;
      }
      Application::MessagingSystem::Send("Active Quit Game Button", true);
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) &&
      FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active) {
      Application::QueueCommand(Application::Command::QuitApplication);
    }
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static ExitButtonScript ExitButton;
