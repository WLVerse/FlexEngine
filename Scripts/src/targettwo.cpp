/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// targettwo.cpp
//
// This script handles the interactions for the mouse inputs used by the
// target two button of the game's target selection. It sends messages via
// the messaging system whenever the button has been clicked by the player. 
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class TargetTwoScript : public IScript
{
public:
  TargetTwoScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "TargetTwo";
  }

  void Update() override
  {
  }

  void OnMouseEnter() override
  {
    //self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Hover.png)");
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("TargetTwo clicked", true);
    }
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)");
  }
};

// Static instance to ensure registration
static TargetTwoScript TargetTwo;
