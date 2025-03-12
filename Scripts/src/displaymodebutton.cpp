#include <FlexEngine.h>
using namespace FlexEngine;

class DisplayModeButtonScript : public IScript
{
public:
  DisplayModeButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "DisplayModeButton";
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
        FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active = true;
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE) && FlexECS::Scene::GetEntityByName("Return Button Sprite") == FlexECS::Entity::Null) {
        Input::Cleanup();
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        std::string entity_name = "Return Button Sprite";
        if (FlexECS::Scene::GetEntityByName("Return Button Sprite") == FlexECS::Entity::Null) {
          entity_name = "Master Volume Sprite";
        }
        FlexECS::Scene::GetEntityByName(entity_name).GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName(entity_name).GetComponent<Transform>()->is_active = true;
        self.GetComponent<Transform>()->is_active = false;
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
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    {
      // TODO: Insert Quit Game Function
      // Application::MessagingSystem::Send("MoveOne clicked", true);
    }
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static DisplayModeButtonScript DisplayModeButton;
