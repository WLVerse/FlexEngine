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
    //self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Hover.png)");
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      //Application::MessagingSystem::Send("Start Cutscene", true);
        Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 3, 1.0 });
    }
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)");
  }
};

// Static instance to ensure registration
static MenuButtonScript MenuButton;
