#include <FlexEngine.h>
using namespace FlexEngine;

class MasterSliderScript : public IScript
{
private:
  bool is_draggable = false;
public:
  MasterSliderScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MasterSlider";
  }

  void Update() override
  {
    FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("Master Slider Fill");
    FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

    Vector3& new_position = self.GetComponent<Position>()->position;

    if (is_draggable) {
      new_position.x += Input::GetMousePositionDelta().x;
      new_position.x = std::clamp(new_position.x, slider_details->min_position, slider_details->max_position);
    }
    if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) is_draggable = false;

    // Update slider fill position and scale
    slider_fill.GetComponent<Position>()->position.x = (new_position.x + slider_details->min_position - 4.f) / 2.f;
    float fill_to_set = (new_position.x - slider_details->min_position) / slider_details->slider_length;
    slider_fill.GetComponent<Scale>()->scale.x = slider_details->original_scale.x / slider_details->original_value * fill_to_set;
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
static MasterSliderScript MasterSlider;
