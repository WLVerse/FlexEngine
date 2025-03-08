#include <FlexEngine.h>
using namespace FlexEngine;

class MoveTwoScript : public IScript
{
public:
  MoveTwoScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MoveTwo";
  }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("MoveTwo hovered", true);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("MoveTwo clicked", true);
    }
  }

  void OnMouseExit() override
  {
  }
};

// Static instance to ensure registration
static MoveTwoScript MoveTwo;
