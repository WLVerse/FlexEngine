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

class OptionsButtonScript : public IScript
{
public:
  OptionsButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "OptionsButton";
  }

  void Update() override
  {

  }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("Menu Buttons", std::make_pair(2, true));
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
      Application::MessagingSystem::Send("OptionStart", true);
    }
  }

  void OnMouseExit() override
  {

  }

};

// Static instance to ensure registration
static OptionsButtonScript OptionsButton;
