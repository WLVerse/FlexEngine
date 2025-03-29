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

class BGMSliderKnobScript : public IScript
{
private:
  bool is_draggable = false;
public:
  BGMSliderKnobScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "BGMSliderKnob";
  }

  void Update() override
  {
    FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("BGM Slider Fill");
    FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

    Vector3& new_position = self.GetComponent<Position>()->position;

    if (is_draggable) {
      new_position.x += Input::GetMousePositionDelta().x;
      new_position.x = std::clamp(new_position.x, slider_details->min_position, slider_details->max_position);
    }
    if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) {
      is_draggable = false;
    }
  }

  void OnMouseEnter() override
  {
    if (!is_draggable) self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
  }

  void OnMouseStay() override
  {
    if (!is_draggable) self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.25f);
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
      self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
      is_draggable = true;
    }
  }

  void OnMouseExit() override
  {
    if (!is_draggable) self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
  }
};

// Static instance to ensure registration
static BGMSliderKnobScript BGMSliderKnob;
