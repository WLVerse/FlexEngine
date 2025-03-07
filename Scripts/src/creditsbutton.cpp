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
static CreditsButtonScript CreditsButton;
