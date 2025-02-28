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
    FLX_STRING_GET(self.GetComponent<Sprite>()->sprite_handle) = "/images/battle ui/Battle_UI_Skill_Selected.png";
  }

  void OnMouseStay() override
  {
    FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description").GetComponent<Transform>()->is_active = true;
    FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Transform>()->is_active = true;
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      Application::MessagingSystem::Send("MoveThree clicked", true);
    }
  }

  void OnMouseExit() override
  {
    FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description").GetComponent<Transform>()->is_active = false;
    FlexECS::Scene::GetActiveScene()->GetEntityByName("Move Description Text").GetComponent<Transform>()->is_active = false;
    FLX_STRING_GET(self.GetComponent<Sprite>()->sprite_handle) = "/images/battle ui/Battle_UI_Skill_Unselected.png";
  }
};

// Static instance to ensure registration
static MoveThreeScript MoveThree;
