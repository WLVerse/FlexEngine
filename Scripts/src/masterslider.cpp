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
    if (is_draggable) {
      FlexECS::Entity tempEntity = FlexECS::Scene::GetEntityByName("Master Slider Background");

      float min_value = tempEntity.GetComponent<Position>()->position.x - tempEntity.GetComponent<Sprite>()->scale.x - 20.f;
      float max_value = tempEntity.GetComponent<Position>()->position.x + tempEntity.GetComponent<Sprite>()->scale.x + 20.f;

      Vector3& new_position = self.GetComponent<Position>()->position;
      new_position.x += Input::GetMousePositionDelta().x;
      new_position.x = std::clamp(new_position.x, min_value, max_value);

      float volume_to_set = ((new_position.x - min_value)) / (max_value - min_value);

      tempEntity = FlexECS::Scene::GetEntityByName("Master Slider Fill");

      Vector3& new_slider_fill_pos = tempEntity.GetComponent<Position>()->position;
      new_slider_fill_pos.x = (new_position.x + min_value - 5.f) / 2.f;

      Vector3& new_slider_fill_scale = tempEntity.GetComponent<Scale>()->scale;
      new_slider_fill_scale.x =
        tempEntity.GetComponent<Slider>()->original_scale.x / tempEntity.GetComponent<Slider>()->original_value * volume_to_set;
    }
    if (Input::GetMouseButtonUp(GLFW_MOUSE_BUTTON_LEFT)) is_draggable = false;
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
