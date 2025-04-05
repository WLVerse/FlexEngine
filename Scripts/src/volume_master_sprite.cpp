/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// mastervolbutton.cpp
//
// This script handles the interactions for the key inputs used by the
// pause menu master volume slider
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class MasterVolButtonScript : public IScript
{
public:
  MasterVolButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MasterVolButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {

      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
          if (FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active) {
            Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "Display Mode Sprite", true});
          }
        }
        else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Return Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
          if (FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active) {
            Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "BGM Volume Sprite", true});
          }
        }
        else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "BGM Volume Sprite", true});
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
        Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "Master Volume Sprite", true});
      }
      else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Master Volume Sprite", true});
    }
  }

  void OnMouseStay() override
  {

  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MasterVolButtonScript MasterVolButton;
