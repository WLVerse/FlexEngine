#include <FlexEngine.h>
using namespace FlexEngine;

class MoveThreeScript : public IScript
{
public:
  MoveThreeScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MoveThree";
  }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("MoveThree hovered", true);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("MoveThree clicked", true);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MoveThreeScript MoveThree;
