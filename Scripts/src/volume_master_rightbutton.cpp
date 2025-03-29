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

class MasterVolIncreaseScript : public IScript
{
private:
  float timer = 0.f;
  bool to_spam = false;
public:
  MasterVolIncreaseScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "IncreaseMasterVolume";
  }

  void Update() override
  {
    if (timer > 0.f) {
      timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
    }
    else timer = 0.f;
  }

  void OnMouseEnter() override
  {
    self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
  }

  void OnMouseStay() override
  {
    FlexECS::Entity knob = FlexECS::Scene::GetEntityByName("Master Knob");

    FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("Master Slider Fill");
    FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

    float& knob_pos = knob.GetComponent<Position>()->position.x;
    float current_volume_value = (knob_pos - slider_details->min_position) / slider_details->slider_length;

    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
      current_volume_value = std::clamp(current_volume_value += 0.01f, 0.f, 1.f);
      timer = 0.5f;
      to_spam = true;
    }
    if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) {
      self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
      to_spam = false;
      timer = 0.f;
    }
    if (timer == 0.f && to_spam) {
      current_volume_value = std::clamp(current_volume_value += 0.01f, 0.f, 1.f);
    }
    knob_pos = current_volume_value * slider_details->slider_length + slider_details->min_position;
  }

  void OnMouseExit() override
  {
    self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
    to_spam = false;
  }
};

// Static instance to ensure registration
static MasterVolIncreaseScript IncreaseMasterVol;
