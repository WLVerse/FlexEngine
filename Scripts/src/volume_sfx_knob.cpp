/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// bgmslider.cpp
//
// This script handles the interactions for the mouse inputs used by the knob
// of the background music slider. It also updates the background music volume.
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class SFXSliderKnobScript : public IScript
{
private:
  bool is_draggable = false;
public:
  SFXSliderKnobScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "SFXSliderKnob";
  }

  void Update() override
  {
    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("SFX Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

      Vector3& new_position = self.GetComponent<Position>()->position;

      if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) {
        is_draggable = false;
      }

      if (is_draggable) {
        new_position.x += Input::GetMousePositionDelta().x;
        new_position.x = std::clamp(new_position.x, slider_details->min_position, slider_details->max_position);
      }
    }
    else {
      if (is_draggable) is_draggable = false;
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      if (!is_draggable) self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
    }
  }

  void OnMouseStay() override
  {
    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      if (!is_draggable) self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.25f);
      if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        FlexECS::Scene::GetEntityByName("ButtonPress").GetComponent<Audio>()->should_play = true;
        self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
        is_draggable = true;
      }
    }
  }

  void OnMouseExit() override
  {
    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      if (!is_draggable) self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
    }
  }
};

// Static instance to ensure registration
static SFXSliderKnobScript SFXSliderKnob;
