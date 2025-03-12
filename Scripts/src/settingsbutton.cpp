#include <FlexEngine.h>
using namespace FlexEngine;

class SettingsButtonScript : public IScript
{
public:
  SettingsButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "SettingsButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (self.GetComponent<Scale>()->scale.x != self.GetComponent<Slider>()->original_scale.x) {
        self.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        self.GetComponent<Scale>()->scale.x = std::clamp(self.GetComponent<Scale>()->scale.x,
          0.f, self.GetComponent<Slider>()->original_scale.x);
      }

      if (!FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active &&
        !FlexECS::Scene::GetEntityByName("BGM Volume Sprite").GetComponent<Transform>()->is_active &&
        !FlexECS::Scene::GetEntityByName("SFX Volume Sprite").GetComponent<Transform>()->is_active &&
        !FlexECS::Scene::GetEntityByName("Display Mode Sprite").GetComponent<Transform>()->is_active) {
        if (Input::GetKeyDown(GLFW_KEY_W)) {
          Input::Cleanup();
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
            entity.GetComponent<Transform>()->is_active = false;
          }
          FlexECS::Scene::GetEntityByName("Resume Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName("Resume Button Sprite").GetComponent<Transform>()->is_active = true;
          self.GetComponent<Transform>()->is_active = false;
        }
        if (Input::GetKeyDown(GLFW_KEY_S)) {
          Input::Cleanup();
          for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
            entity.GetComponent<Transform>()->is_active = false;
          }
          FlexECS::Scene::GetEntityByName("Credits Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName("Credits Button Sprite").GetComponent<Transform>()->is_active = true;
          self.GetComponent<Transform>()->is_active = false;
        }
        if (Input::GetKeyDown(GLFW_KEY_SPACE)) {
          Input::Cleanup();
          FlexECS::Scene::GetEntityByName("Master Volume Sprite").GetComponent<Transform>()->is_active = true;
        }
      }
    }
  }

  void OnMouseEnter() override
  {
    /*if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active) {
      if (!self.GetComponent<Transform>()->is_active) {
        self.GetComponent<Scale>()->scale.x = 0.f;
      }
      self.GetComponent<Transform>()->is_active = true;
    }*/
  }

  void OnMouseStay() override
  {
    //if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    //{
    //  // TODO: Send message to display different assets (Either settings or credits)
    //  for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
    //    if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = true;
    //  }
    //}
  }

  void OnMouseExit() override
  {
    //self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static SettingsButtonScript SettingsButton;
