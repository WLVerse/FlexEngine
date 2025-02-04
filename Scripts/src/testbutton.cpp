#include <FlexEngine.h>
using namespace FlexEngine;

class TestButtonScript : public IScript
{
public:
  TestButtonScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "TestButton"; }

  void OnMouseEnter() override
  {
    Log::Debug("Mouse entered: " + FLX_STRING_GET(*self.GetComponent<EntityName>()));
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Log::Debug("Mouse clicked: " + FLX_STRING_GET(*self.GetComponent<EntityName>()));
    }
  }

  void OnMouseExit() override
  {
    Log::Debug("Mouse exited: " + FLX_STRING_GET(*self.GetComponent<EntityName>()));
  }
};

// Static instance to ensure registration
static TestButtonScript TestButton;
