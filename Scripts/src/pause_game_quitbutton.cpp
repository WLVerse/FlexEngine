/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// quitbutton.cpp
//
// This script handles the interactions for the key inputs used by the quit
// button inside the pause menu. It exits the game when user selects the button
// 
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include <FlexEngine.h>
using namespace FlexEngine;

class ExitButtonScript : public IScript
{
private:
  bool is_quit_mode = false;
  bool to_quit = true;
public:
  ExitButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "ExitButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "How Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Resume Button Sprite", true});
      }
      if (Input::GetKeyDown(GLFW_KEY_ENTER) || Input::GetKeyDown(GLFW_KEY_SPACE) ||
        Application::MessagingSystem::Receive<bool>("Cancel Quit")) {
        Input::Cleanup();
        if (is_quit_mode) {
          if (FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active) {
            Application::QueueCommand(Application::Command::QuitApplication);
          }
          if (FlexECS::Scene::GetEntityByName("Quit Game No Sprite").GetComponent<Transform>()->is_active) {
            for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, QuitUI>()) {
              entity.GetComponent<Transform>()->is_active = false;
            }
            is_quit_mode = false;
          }
        }
        else {
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, QuitUI>()) {
            if (entity.HasComponent<PauseHoverUI>()) entity.GetComponent<Transform>()->is_active = false;
            else entity.GetComponent<Transform>()->is_active = true;
          }
          FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active = true;
          is_quit_mode = true;
        }
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE)) {
        Input::Cleanup();
        if (is_quit_mode) {
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, QuitUI>()) {
            entity.GetComponent<Transform>()->is_active = false;
          }
          is_quit_mode = false;
        }
        else Application::MessagingSystem::Send("Resume Game", true);
      }
      if (is_quit_mode) {
        if (Input::GetKeyDown(GLFW_KEY_A) || Input::GetKeyDown(GLFW_KEY_D)) {
          Input::Cleanup();
          if (FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active) {
            FlexECS::Scene::GetEntityByName("Quit Game No Sprite").GetComponent<Scale>()->scale.x = 0.f;
          }
          else FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active ^= true;
          FlexECS::Scene::GetEntityByName("Quit Game No Sprite").GetComponent<Transform>()->is_active ^= true;
        }
        if (Application::MessagingSystem::Receive<bool>("Quit Yes") &&
          FlexECS::Scene::GetEntityByName("Quit Game No Sprite").GetComponent<Transform>()->is_active) {
          FlexECS::Scene::GetEntityByName("Quit Game No Sprite").GetComponent<Transform>()->is_active = false;
          FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active = true;
        }
        if (Application::MessagingSystem::Receive<bool>("Quit No") && 
          FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active) {
          FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active = false;
          FlexECS::Scene::GetEntityByName("Quit Game No Sprite").GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName("Quit Game No Sprite").GetComponent<Transform>()->is_active = true;
        }
      }
    }
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active &&
      !self.GetComponent<Transform>()->is_active) {
      Application::MessagingSystem::Send("Pause Sprite", std::pair <std::string, bool> { "Quit Button Sprite", true});
    }
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active) {
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, QuitUI>()) {
        if (entity.HasComponent<PauseHoverUI>()) entity.GetComponent<Transform>()->is_active = false;
        else entity.GetComponent<Transform>()->is_active = true;
      }
      FlexECS::Scene::GetEntityByName("Quit Game Yes Sprite").GetComponent<Transform>()->is_active = true;
      is_quit_mode = true;
    }
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static ExitButtonScript ExitButton;
