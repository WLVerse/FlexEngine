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
    self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Hover.png)");
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) Log::Info("MoveThree clicked");
  }

  void OnMouseExit() override
  {
    self.GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)");
  }
};

// Static instance to ensure registration
static MoveThreeScript MoveThree;
