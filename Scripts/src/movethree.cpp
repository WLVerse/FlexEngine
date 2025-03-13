/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// movethree.cpp
//
// This script handles the interactions for the mouse inputs used by the move
// one button of the game's move selection. It sends messages via the messaging
// system whenever the button has been hovered or clicked by the player. 
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class MoveThreeScript : public IScript
{
public:
  MoveThreeScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MoveThree";
  }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("MoveThree hovered", true);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("MoveThree clicked", true);
    }
  }

  void OnMouseExit() override
  {
  }
};

// Static instance to ensure registration
static MoveThreeScript MoveThree;
