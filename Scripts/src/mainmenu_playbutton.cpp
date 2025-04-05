/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// menubutton.cpp
//
// This script handles the interactions for the mouse inputs used by the menu
// button of the main menu. It sends the message to start transition via the
// messaging system when player clicks on it
//
// AUTHORS
// [100%] Kuan Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class MenuButtonScript : public IScript
{
public:
  MenuButtonScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "MenuButton"; }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("Menu Buttons", std::make_pair(0, true));
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("StartTransitionToGame", true);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MenuButtonScript MenuButton;
