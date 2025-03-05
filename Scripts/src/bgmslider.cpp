#include <FlexEngine.h>
using namespace FlexEngine;

class BGMSliderScript : public IScript
{
private:
  bool is_draggable = false;
public:
  BGMSliderScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "BGMSlider";
  }

  void Update() override
  {
    FlexECS::Entity tempEntity = FlexECS::Scene::GetEntityByName("BGM Slider Background");

    float min_value = tempEntity.GetComponent<Position>()->position.x - tempEntity.GetComponent<Sprite>()->scale.x - 20.f;
    float max_value = tempEntity.GetComponent<Position>()->position.x + tempEntity.GetComponent<Sprite>()->scale.x + 20.f;

    Vector3& new_position = self.GetComponent<Position>()->position;

    tempEntity = FlexECS::Scene::GetEntityByName("Master Knob");
    float& master_value = tempEntity.GetComponent<Position>()->position.x;
    float volume_value = (new_position.x > master_value) ? master_value : new_position.x;

    float volume_to_set = ((volume_value - min_value)) / (max_value - min_value);
    FMODWrapper::Core::AdjustGroupVolume(FMODWrapper::Core::CHANNELGROUP::BGM, volume_to_set);

    if (is_draggable) {
      new_position.x += Input::GetMousePositionDelta().x;
      new_position.x = std::clamp(new_position.x, min_value, max_value);

      tempEntity = FlexECS::Scene::GetEntityByName("BGM Slider Fill");

      Vector3& new_slider_fill_pos = tempEntity.GetComponent<Position>()->position;
      new_slider_fill_pos.x = (new_position.x + min_value - 5.f) / 2.f;

      Vector3& new_slider_fill_scale = tempEntity.GetComponent<Scale>()->scale;
      float fill_to_set = ((new_position.x - min_value)) / (max_value - min_value);
      new_slider_fill_scale.x =
        tempEntity.GetComponent<Slider>()->original_scale.x / tempEntity.GetComponent<Slider>()->original_value * fill_to_set;
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
static BGMSliderScript BGMSlider;
