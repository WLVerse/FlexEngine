#include <FlexEngine.h>
using namespace FlexEngine;

class ExitButtonScript : public IScript
{
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
      if (self.GetComponent<Scale>()->scale.x != self.GetComponent<Slider>()->original_scale.x) {
        self.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        self.GetComponent<Scale>()->scale.x = std::clamp(self.GetComponent<Scale>()->scale.x,
          0.f, self.GetComponent<Slider>()->original_scale.x);
      }

      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        self.GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Credits Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Credits Button Sprite").GetComponent<Transform>()->is_active = true;
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        self.GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Resume Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Resume Button Sprite").GetComponent<Transform>()->is_active = true;
      }
      if (Input::GetKeyDown(GLFW_KEY_ENTER) || Input::GetKeyDown(GLFW_KEY_SPACE)) {
        Application::QueueCommand(Application::Command::QuitApplication);
      }
    }
  }

  void OnMouseEnter() override
  {
    /*if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active)
      self.GetComponent<Transform>()->is_active = true;*/
  }

  void OnMouseStay() override
  {
    //if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    //  Application::QueueCommand(Application::Command::QuitApplication);
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static ExitButtonScript ExitButton;
