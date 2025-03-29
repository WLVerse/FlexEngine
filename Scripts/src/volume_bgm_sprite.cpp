/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// bgmbutton.cpp
//
// This script handles the interactions for the key inputs used by the
// pause menu background music slider
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class BGMButtonScript : public IScript
{
private:
  bool is_exit = false;
  bool inc_selected = false;
  bool dec_selected = false;
  float timer = 0.f;
public:
  BGMButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "BGMButton";
  }

  void Update() override
  {
    if (FlexECS::Scene::GetEntityByName("BGM Volume Sprite").GetComponent<Transform>()->is_active) {
      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active Master Volume", true);
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Active SFX Volume", true);
      }
      /*if (Input::GetKeyDown(GLFW_KEY_ESCAPE) && FlexECS::Scene::GetEntityByName("Return Button Sprite") == FlexECS::Entity::Null) {
        Input::Cleanup();
        self.GetComponent<Transform>()->is_active = false;
      }*/

      FlexECS::Entity knob = FlexECS::Scene::GetEntityByName("BGM Knob");

      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("BGM Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

      float& knob_pos = knob.GetComponent<Position>()->position.x;
      float current_volume_value = (knob_pos - slider_details->min_position) / slider_details->slider_length;

      if (Input::GetKeyDown(GLFW_KEY_D) && current_volume_value < 1.f) {
        inc_selected = true;
        current_volume_value += 0.01f;
        timer = 0.5f;
        FlexECS::Scene::GetEntityByName("BGM Volume Right").GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.f);
      }
      if (Input::GetKeyDown(GLFW_KEY_A) && current_volume_value > 0.f) {
        dec_selected = true;
        current_volume_value -= 0.01f;
        timer = 0.5f;
        FlexECS::Scene::GetEntityByName("BGM Volume Left").GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.f);
      }

      timer > 0.f ? timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() : timer = 0.f;

      if (inc_selected || dec_selected) {
        FlexECS::Scene::GetEntityByName("BGM Knob").GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.f);
      }
      else {
        FlexECS::Scene::GetEntityByName("BGM Knob").GetComponent<Scale>()->scale = Vector3(1.f, 1.f, 1.f);
      }

      if (inc_selected) {
        if (Input::GetKeyUp(GLFW_KEY_D) || current_volume_value > 1.f) {
          inc_selected = false;
          FlexECS::Scene::GetEntityByName("BGM Volume Right").GetComponent<Scale>()->scale = Vector3(1.f, 1.f, 1.f);
        }
        if (timer == 0.f) current_volume_value < 1.f ? current_volume_value += 0.01f : current_volume_value = 1.f;
      }

      if (dec_selected) {
        if (Input::GetKeyUp(GLFW_KEY_A) || current_volume_value < 0.f) {
          dec_selected = false;
          FlexECS::Scene::GetEntityByName("BGM Volume Left").GetComponent<Scale>()->scale = Vector3(1.f, 1.f, 1.f);
        }
        if (timer == 0.f) current_volume_value > 0.f ? current_volume_value -= 0.01f : current_volume_value = 0.f;
      }

      knob_pos = current_volume_value * slider_details->slider_length + slider_details->min_position;
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active
      && !FlexECS::Scene::GetEntityByName("BGM Volume Sprite").GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Active BGM Volume", true);
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      // TODO: Insert Quit Game Function
      // Application::MessagingSystem::Send("MoveOne clicked", true);
    }
  }

  void OnMouseExit() override
  {
    // FlexECS::Scene::GetEntityByName("BGM Volume Sprite").GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static BGMButtonScript BGMButton;
