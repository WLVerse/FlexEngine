/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// creditsbutton.cpp
//
// This script handles the interactions for the key inputs used by the credits
// button inside the pause menu. It displays the credits when it has been
// selected by the user
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class HowButtonScript : public IScript
{
  bool is_tutorial = false;
  int active_page = 1;
public:
  HowButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "HowButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (!is_tutorial) {
        if (Input::GetKeyDown(GLFW_KEY_W)) {
          Input::Cleanup();
          Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Settings Button Sprite", true});
        }
        if (Input::GetKeyDown(GLFW_KEY_S)) {
          Input::Cleanup();
          Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Quit Button Sprite", true});
        }
        if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
          Input::Cleanup();
          Application::MessagingSystem::Send("Resume Game", true);
        }
        if (Input::GetKeyDown(GLFW_KEY_SPACE)) {
          is_tutorial = true;
          active_page = 1;
          FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active = true;
          FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Transform>()->is_active = true;
          FlexECS::Scene::GetEntityByName("Tutorial P1").GetComponent<Transform>()->is_active = true;
        }
      }
      else {
        bool return_page = Application::MessagingSystem::Receive<bool>("HTPReturnPage");
        bool next_page = Application::MessagingSystem::Receive<bool>("HTPNextPage");

        if (Input::GetKeyDown(GLFW_KEY_A) || return_page) {
          if (active_page != 1) {
            FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(active_page)).GetComponent<Transform>()->is_active = false;
            --active_page;
            FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(active_page)).GetComponent<Transform>()->is_active = true;
          }
        }
        if (Input::GetKeyDown(GLFW_KEY_D) || next_page) {
          if (active_page != 5) {
            FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(active_page)).GetComponent<Transform>()->is_active = false;
            ++active_page;
            FlexECS::Scene::GetEntityByName("Tutorial P" + std::to_string(active_page)).GetComponent<Transform>()->is_active = true;
          }
        }
        if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
          Input::Cleanup();
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, CreditsUI>()) {
            entity.GetComponent<Transform>()->is_active = false;
          }
          is_tutorial = false;
        }
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "How Button Sprite", true});
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    {
      is_tutorial = true;
      active_page = 1;
      FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("How To Play Dark Background").GetComponent<Transform>()->is_active = true;
      FlexECS::Scene::GetEntityByName("Tutorial P1").GetComponent<Transform>()->is_active = true;
    }
  }

  void OnMouseExit() override
  {
    /*self.GetComponent<Transform>()->is_active = false;*/
  }
};

// Static instance to ensure registration
static HowButtonScript HowButton;
