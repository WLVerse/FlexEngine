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
        if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
          if (FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active) {
            Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "Master Volume Sprite", true});
          }
        }
        else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> {  "Master Volume Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
          if (FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active) {
            Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "SFX Volume Sprite", true});
          }
        }
        else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "SFX Volume Sprite", true});
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      if (FlexECS::Scene::GetEntityByName("Settings Button Sprite") != FlexECS::Entity::Null) {
        Application::MessagingSystem::Send("Volume Sprite", std::pair <std::string, bool> { "BGM Volume Sprite", true});
      }
      else Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "BGM Volume Sprite", true});
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
static BGMButtonScript BGMButton;
