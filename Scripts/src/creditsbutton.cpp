#include <FlexEngine.h>
using namespace FlexEngine;

class CreditsButtonScript : public IScript
{
public:
  CreditsButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "CreditsButton";
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
        for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
          if (!entity.HasComponent<Slider>() || !entity.HasComponent<Script>()) entity.GetComponent<Transform>()->is_active = true;
        }
        self.GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active = true;
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        self.GetComponent<Transform>()->is_active = false;
        FlexECS::Scene::GetEntityByName("Quit Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Quit Button Sprite").GetComponent<Transform>()->is_active = true;
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
    //{
    //  // TODO: Send message to display different assets (Either settings or credits)
    //  for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, SettingsUI>()) {
    //    entity.GetComponent<Transform>()->is_active = false;
    //  }
    //}
  }

  void OnMouseExit() override
  {
    /*self.GetComponent<Transform>()->is_active = false;*/
  }
};

// Static instance to ensure registration
static CreditsButtonScript CreditsButton;
