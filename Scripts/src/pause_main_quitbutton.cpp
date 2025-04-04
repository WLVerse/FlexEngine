/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// mainmenuquit.cpp
//
// This script handles the interactions for the key inputs used by the quit
// button inside the options screen of the main menu. It exits the game
// when player selects this button.
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class MainMenuQuitScript : public IScript
{
public:
  MainMenuQuitScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "MainMenuQuit"; }
  
  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> {"Display Mode Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Master Volume Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_D) || Input::GetKeyDown(GLFW_KEY_A)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Return Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_SPACE) || Input::GetKeyDown(GLFW_KEY_ENTER)) {
        Application::QueueCommand(Application::Command::QuitApplication);
      }
    }
  }
  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active
      && !FlexECS::Scene::GetEntityByName("Quit Button Sprite").GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Quit Button Sprite", true});
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("Quit Confirmation Start", true);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MainMenuQuitScript MainMenuQuit;
