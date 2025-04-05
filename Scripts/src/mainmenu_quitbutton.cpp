/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// exitbutton.cpp
//
// This script handles the interactions for the key inputs used by the quit
// button inside the main menu. It exits the game when user selects the button
//
// AUTHORS
// [100%] Kuan Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class QuitButtonScript : public IScript
{
public:
  QuitButtonScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "QuitButton"; }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("Menu Buttons", std::make_pair(4, true));
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
static QuitButtonScript QuitButton;
