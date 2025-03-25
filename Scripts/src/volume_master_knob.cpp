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

class MasterSliderKnobScript : public IScript
{
private:
  bool is_draggable = false;
public:
  MasterSliderKnobScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MasterSliderKnob";
  }

  void Update() override
  {
    if (is_draggable) {
      if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) is_draggable = false;
      // Update slider knob position
      Vector3& new_position = self.GetComponent<Position>()->position;
      new_position.x += Input::GetMousePositionDelta().x;

      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("Master Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();
      new_position.x = std::clamp(new_position.x, slider_details->min_position, slider_details->max_position);
    }
  }

  void OnMouseEnter() override
  {
    self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) is_draggable = true;
  }

  void OnMouseExit() override
  {
    if (!is_draggable) self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
  }
};

// Static instance to ensure registration
static MasterSliderKnobScript MasterSliderKnob;
