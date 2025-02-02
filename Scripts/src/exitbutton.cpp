#include <FlexEngine.h>
using namespace FlexEngine;

class QuitButtonScript : public IScript
{
public:
  QuitButtonScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "QuitButton"; }

  void OnMouseEnter() override
  {
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::Quit();
    }
  }

  void OnMouseExit() override
  {
  }
};

// Static instance to ensure registration
static QuitButtonScript QuitButton;
