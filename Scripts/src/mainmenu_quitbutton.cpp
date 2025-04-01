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
    self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Hover.png)");
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::QueueCommand(Application::Command::QuitApplication);
    }
  }

  void OnMouseExit() override
  {
    self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)");
  }
};

// Static instance to ensure registration
static QuitButtonScript QuitButton;
