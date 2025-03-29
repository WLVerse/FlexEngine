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
        Application::MessagingSystem::Send("Active SFX Volume", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE) && FlexECS::Scene::GetEntityByName("Return Button Sprite") == FlexECS::Entity::Null) {
        Input::Cleanup();
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        std::string message_to_send = "Active Return Button";
        if (FlexECS::Scene::GetEntityByName("Return Button Sprite") == FlexECS::Entity::Null) {
          message_to_send = "Active Master Volume";
        }
        Application::MessagingSystem::Send(message_to_send, true);
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Settings Menu Background").GetComponent<Transform>()->is_active
      && !FlexECS::Scene::GetEntityByName("Display Mode Sprite").GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Active Display Mode", true);
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
