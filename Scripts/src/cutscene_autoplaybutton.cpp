#include <FlexEngine.h>
using namespace FlexEngine;

class Cutscene_AutoplayBtnScript : public IScript
{
public:
    Cutscene_AutoplayBtnScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "Cutscene_Autoplay";
  }

  void Update() override
  {
  }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("Cutscene_AutoplayBtn hovered", true);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("Cutscene_AutoplayBtn clicked", true);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static Cutscene_AutoplayBtnScript Cutscene_AutoplayBtn;
