#include <FlexEngine.h>
using namespace FlexEngine;

class MasterVolDecreaseScript : public IScript
{
public:
  MasterVolDecreaseScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "DecreaseMasterVolume";
  }

  void Update() override
  {
  }

  void OnMouseEnter() override
  {

  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT))
    {
      FlexECS::Entity knob = FlexECS::Scene::GetEntityByName("Master Knob");

      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("Master Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

      float& knob_pos = knob.GetComponent<Position>()->position.x;
      float current_volume_value = (knob_pos - slider_details->min_position) / slider_details->slider_length;

      current_volume_value -= 0.01f;

      knob_pos = current_volume_value * slider_details->slider_length + slider_details->min_position;
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MasterVolDecreaseScript IncreaseMasterVol;
