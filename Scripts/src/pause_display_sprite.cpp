/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// displaymodebutton.cpp
//
// This script handles the interactions for the key inputs used by the display
// mode button inside the pause menu and options screen of the main menu. It
// displays the display mode selected by the user
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class DisplayModeButtonScript : public IScript
{
public:
  DisplayModeButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "DisplayModeButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {

      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
          if (FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active) {
            Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "SFX Volume Sprite", true});
          }
        }
        else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "SFX Volume Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
          Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "Master Volume Sprite", true});
        }
        else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Return Button Sprite", true});
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
        if (FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active) {
          Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "Display Mode Sprite", true});
        }
      }
      else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Display Mode Sprite", true});
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    {
      // TODO: Insert Quit Game Function
      // Application::MessagingSystem::Send("MoveOne clicked", true);
    }
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static DisplayModeButtonScript DisplayModeButton;
