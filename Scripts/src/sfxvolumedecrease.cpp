#include <FlexEngine.h>
using namespace FlexEngine;

class SFXDecreaseScript : public IScript
{
public:
  SFXDecreaseScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "DecreaseSFX";
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
      FlexECS::Entity knob = FlexECS::Scene::GetEntityByName("SFX Knob");

      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("SFX Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

      float& knob_pos = knob.GetComponent<Position>()->position.x;
      float current_volume_value = (knob_pos - slider_details->min_position) / slider_details->slider_length;

      current_volume_value -= 0.01f;
      FMODWrapper::Core::AdjustGroupVolume(FMODWrapper::Core::CHANNELGROUP::SFX, current_volume_value);

      knob_pos = current_volume_value * slider_details->slider_length + slider_details->min_position;
    }
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static SFXDecreaseScript DecreaseSFX;
