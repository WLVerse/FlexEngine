/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// mastervolumeincrease.cpp
//
// This script handles the interactions for the mouse inputs used by the right
// arrow of the master volume slider. It increases the volume each time the
// user clicks on the arrow
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class HTPBackScript : public IScript
{
public:
  HTPBackScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "HTPLeftButton";
  }

  void Update() override
  {

  }

  void OnMouseEnter() override
  {

  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("HTPReturnPage", true);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static HTPBackScript HTPLeftButton;
