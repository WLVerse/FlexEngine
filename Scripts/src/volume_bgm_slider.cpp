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

class BGMSliderScript : public IScript
{
public:
  BGMSliderScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "BGMSlider";
  }

  void Update() override
  {
    FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("BGM Slider Fill");
    FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

    Vector3& new_position = FlexECS::Scene::GetEntityByName("BGM Knob").GetComponent<Position>()->position;

    // Update slider fill position and scale
    slider_fill.GetComponent<Position>()->position.x = (new_position.x + slider_details->min_position - 4.f) / 2.f;
    float fill_to_set = (new_position.x - slider_details->min_position) / slider_details->slider_length;
    slider_fill.GetComponent<Scale>()->scale.x = slider_details->original_scale.x / slider_details->original_value * fill_to_set;

    // Update BGM volume
    float& master_value = FlexECS::Scene::GetEntityByName("Master Knob").GetComponent<Position>()->position.x;
    float volume_value = (new_position.x > master_value) ? master_value : new_position.x;

    float volume_to_set = (volume_value - slider_details->min_position) / slider_details->slider_length;
    FMODWrapper::Core::AdjustGroupVolume(FMODWrapper::Core::CHANNELGROUP::BGM, volume_to_set);
  }

  void OnMouseEnter() override
  {

  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
      Vector4 clip = {
        (2.0f * Input::GetMousePosition().x) / Application::GetCurrentWindow()->GetWidth() - 1.0f,
        1.0f - (2.0f * Input::GetMousePosition().y) / Application::GetCurrentWindow()->GetHeight(),
        1.0f,
        1.0f
      };

      Vector4 new_pos = CameraManager::GetMainGameCamera()->GetProjViewMatrix().Inverse() * clip;

      FlexECS::Scene::GetEntityByName("BGM Knob").GetComponent<Position>()->position.x = new_pos.x;
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static BGMSliderScript BGMSlider;
