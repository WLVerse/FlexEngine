#include <FlexEngine.h>
using namespace FlexEngine;

class MenuButtonScript : public IScript
{
public:
  MenuButtonScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "MenuButton"; }

  void OnMouseEnter() override
  {
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("Start Game", true);
    }
  }

  void OnMouseExit() override
  {
  }
};

// Static instance to ensure registration
static MenuButtonScript MenuButton;
