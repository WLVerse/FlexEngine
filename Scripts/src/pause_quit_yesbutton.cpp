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

class QuitYesScript : public IScript
{
private:
  bool is_selected = false;
public:
  QuitYesScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "QuitYesButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (self.GetComponent<Scale>()->scale.x != self.GetComponent<Slider>()->original_scale.x) {
        self.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        self.GetComponent<Scale>()->scale.x = std::clamp(self.GetComponent<Scale>()->scale.x, 0.f, self.GetComponent<Slider>()->original_scale.x);
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Quit Game Confirmation Prompt").GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Quit Yes", true);
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    {
      Application::QueueCommand(Application::Command::QuitApplication);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static QuitYesScript QuitYesButton;
