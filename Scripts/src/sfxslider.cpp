#include <FlexEngine.h>
using namespace FlexEngine;

class SFXSliderScript : public IScript
{
private:
  bool is_draggable = false;
public:
  SFXSliderScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "SFXSlider";
  }

  void Update() override
  {
    FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("SFX Slider Fill");
    FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

    Vector3& new_position = self.GetComponent<Position>()->position;

    if (is_draggable) {
      // Update slider knob position
      new_position.x += Input::GetMousePositionDelta().x;
      new_position.x = std::clamp(new_position.x, slider_details->min_position, slider_details->max_position);
    }
    if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) is_draggable = false;

    // Update slider fill position and scale
    slider_fill.GetComponent<Position>()->position.x = (new_position.x + slider_details->min_position) / 2.f;
    float fill_to_set = (new_position.x - slider_details->min_position) / slider_details->slider_length;
    slider_fill.GetComponent<Scale>()->scale.x = slider_details->original_scale.x / slider_details->original_value * fill_to_set;

    // Update BGM volume
    float& master_value = FlexECS::Scene::GetEntityByName("Master Knob").GetComponent<Position>()->position.x;
    float volume_value = (new_position.x > master_value) ? master_value : new_position.x;

    float volume_to_set = (volume_value - slider_details->min_position) / slider_details->slider_length;
    FMODWrapper::Core::AdjustGroupVolume(FMODWrapper::Core::CHANNELGROUP::SFX, volume_to_set);
  }

  void OnMouseEnter() override
  {

  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) is_draggable = true;
  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static SFXSliderScript SFXSlider;
