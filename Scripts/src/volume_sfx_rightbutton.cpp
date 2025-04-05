/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// sfxvolumeincrease.cpp
//
// This script handles the interactions for the mouse inputs used by the right
// arrow of the sfx slider. It increases the volume each time the user clicks
// on the arrow
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class SFXIncreaseScript : public IScript
{
private:
  float timer = 0.f;
  bool to_spam = false;
  std::pair<bool, bool> is_selected = std::make_pair(false, false);
public:
  SFXIncreaseScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "IncreaseSFX";
  }

  void Update() override
  {
    if (timer != 0.f) {
      (timer > 0.f) ? timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() : timer = 0.f;
    }

    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      FlexECS::Entity knob = FlexECS::Scene::GetEntityByName("SFX Knob");

      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("SFX Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

      float& knob_pos = knob.GetComponent<Position>()->position.x;
      float current_volume_value = (knob_pos - slider_details->min_position) / slider_details->slider_length;

      if ((Input::GetKeyDown(GLFW_KEY_D) || is_selected.first) && current_volume_value < 1.f) {
        if (is_selected.first) {
          self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
          is_selected.first = false;
        }
        else {
          self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
        }
        current_volume_value += 0.01f;
        timer = 0.5f;
        to_spam = true;
      }

      if (to_spam) {
        if (Input::GetKeyUp(GLFW_KEY_D) || current_volume_value > 1.f || Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) {
          if (is_selected.second) {
            self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
            is_selected.second = false;
          }
          else {
            self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
          }
          to_spam = false;
          timer = 0.f;
        }
        if (timer == 0.f) {
          current_volume_value < 1.f ? current_volume_value += 0.01f : current_volume_value = 1.f;
        }
      }

      current_volume_value = std::clamp(current_volume_value, 0.f, 1.f);
      knob_pos = current_volume_value * slider_details->slider_length + slider_details->min_position;
    }
    else {
      if (to_spam) to_spam = false;
      if (is_selected.first) is_selected.first = false;
      if (is_selected.second) is_selected.second = false;
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      self.GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.0f);
    }
  }

  void OnMouseStay() override
  {
    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        is_selected.first = true;
        is_selected.second = true;
      }
    }
  }

  void OnMouseExit() override
  {
    if (FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active) {
      self.GetComponent<Scale>()->scale = Vector3(1.0f, 1.0f, 1.0f);
    }
  }
};

// Static instance to ensure registration
static SFXIncreaseScript IncreaseSFX;
