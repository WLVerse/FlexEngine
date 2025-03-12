#include <FlexEngine.h>
using namespace FlexEngine;

class MainMenuQuitScript : public IScript
{
public:
  MainMenuQuitScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "MainMenuQuit"; }
  
  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (self.GetComponent<Scale>()->scale.x != self.GetComponent<Slider>()->original_scale.x) {
        self.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        self.GetComponent<Scale>()->scale.x = std::clamp(self.GetComponent<Scale>()->scale.x,
          0.f, self.GetComponent<Slider>()->original_scale.x);
      }

      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName("Display Mode Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Display Mode Sprite").GetComponent<Transform>()->is_active = true;
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_D) || Input::GetKeyDown(GLFW_KEY_A)) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName("Return Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Return Button Sprite").GetComponent<Transform>()->is_active = true;
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_SPACE) || Input::GetKeyDown(GLFW_KEY_ENTER)) {
        Application::QueueCommand(Application::Command::QuitApplication);
      }
    }
  }
  void OnMouseEnter() override
  {

  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::QueueCommand(Application::Command::QuitApplication);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MainMenuQuitScript MainMenuQuit;
