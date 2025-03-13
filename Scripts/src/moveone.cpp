/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// moveone.cpp
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

class MoveOneScript : public IScript
{
public:
  MoveOneScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MoveOne";
  }

  void Update() override
  {
  }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("MoveOne hovered", true);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("MoveOne clicked", true);
    }
  }

  void OnMouseExit() override
  {
  }
};

// Static instance to ensure registration
static MoveOneScript MoveOne;
