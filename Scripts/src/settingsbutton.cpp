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
  }

  void OnMouseEnter() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active)
      self.GetComponent<Transform>()->is_active = true;
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    {
      // TODO: Send message to display different assets (Either settings or credits)
      // Application::MessagingSystem::Send("Resume Game", true);
    }
  }

  void OnMouseExit() override
  {
    self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static SettingsButtonScript SettingsButton;
