#include <FlexEngine.h>
using namespace FlexEngine;

class MoveOneScript : public IScript
{
public:
  MoveOneScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MoveOne";
  }

  void Update() override
  {
  }

  void OnMouseEnter() override
  {
    Application::MessagingSystem::Send("MoveOne hovered", true);
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("MoveOne clicked", true);
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MoveOneScript MoveOne;
