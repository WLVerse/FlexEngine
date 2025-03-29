/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// mainmenureturn.cpp
//
// This script handles the interactions for the key inputs used by the return
// button inside the options screen of the main menu. It returns the player
// from the options screen, back to the main menu.
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class MainMenuReturnScript : public IScript
{
public:
  MainMenuReturnScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "MainMenuReturn"; }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (self.GetComponent<Scale>()->scale.x != self.GetComponent<Slider>()->original_scale.x) {
        self.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        self.GetComponent<Scale>()->scale.x = std::clamp(self.GetComponent<Scale>()->scale.x,
          0.f, self.GetComponent<Slider>()->original_scale.x);
      }

      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active Display Mode", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active Master Volume", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_D) || Input::GetKeyDown(GLFW_KEY_A)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active Quit Button", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_SPACE) || Input::GetKeyDown(GLFW_KEY_ENTER)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Return to Menu", true);
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active
      && !FlexECS::Scene::GetEntityByName("Return Button Sprite").GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Active Return Button", true);
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    {
      Application::MessagingSystem::Send("Return to Menu", true);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MainMenuReturnScript MainMenuReturn;
