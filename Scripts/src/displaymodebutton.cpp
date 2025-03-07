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
      // TODO: Insert Quit Game Function
      // Application::MessagingSystem::Send("MoveOne clicked", true);
    }
  }

  void OnMouseExit() override
  {
    self.GetComponent<Transform>()->is_active = false;
  }
};

// Static instance to ensure registration
static DisplayModeButtonScript DisplayModeButton;
