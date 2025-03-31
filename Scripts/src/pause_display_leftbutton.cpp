/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// bgmvolumeincrease.cpp
//
// This script handles the interactions for the mouse inputs used by the right
// arrow of the background music slider. It increases the volume each time the
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

class DisplayLeftButtonScript : public IScript
{
public:
  DisplayLeftButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "DisplayLeftButton";
  }

  void Update() override
  {
    if (FlexECS::Scene::GetEntityByName("Display Mode Sprite").GetComponent<Transform>()->is_active &&
      !FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active) {
      if (Input::GetKeyDown(GLFW_KEY_A)) Application::GetCurrentWindow()->ToggleFullScreen(true);
    }
  }

  void OnMouseEnter() override
  {
    self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) &&
      !FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active) {
      self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
      Application::GetCurrentWindow()->ToggleFullScreen(true);
    }
    if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT) &&
      !FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active) {
      self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
    }
  }

  void OnMouseExit() override
  {
    self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
  }
};

// Static instance to ensure registration
static DisplayLeftButtonScript DisplayLeftButton;
