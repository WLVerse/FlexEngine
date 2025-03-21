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

class BGMIncreaseScript : public IScript
{
public:
  BGMIncreaseScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "IncreaseBGM";
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
      FlexECS::Entity knob = FlexECS::Scene::GetEntityByName("BGM Knob");

      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("BGM Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

      float& knob_pos = knob.GetComponent<Position>()->position.x;
      float current_volume_value = (knob_pos - slider_details->min_position) / slider_details->slider_length;

      current_volume_value += 0.01f;
      FMODWrapper::Core::AdjustGroupVolume(FMODWrapper::Core::CHANNELGROUP::BGM, current_volume_value);

      knob_pos = current_volume_value * slider_details->slider_length + slider_details->min_position;
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static BGMIncreaseScript IncreaseBGM;
