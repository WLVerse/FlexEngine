#include <FlexEngine.h>
using namespace FlexEngine;

class SFXButtonScript : public IScript
{
private:
  bool inc_selected = false;
  bool dec_selected = false;
  float timer = 0.f;
public:
  SFXButtonScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "SFXButton";
  }

  void Update() override
  {
    if (self.GetComponent<Transform>()->is_active) {
      if (self.GetComponent<Scale>()->scale.x != self.GetComponent<Slider>()->original_scale.x) {
        self.GetComponent<Scale>()->scale.x += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        self.GetComponent<Scale>()->scale.x = std::clamp(self.GetComponent<Scale>()->scale.x,
          0.f, self.GetComponent<Slider>()->original_scale.x);
      }

      if (Input::GetKeyDown(GLFW_KEY_W)) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName("BGM Volume Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("BGM Volume Sprite").GetComponent<Transform>()->is_active = true;
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_S)) {
        Input::Cleanup();
        FlexECS::Scene::GetEntityByName("Display Mode Sprite").GetComponent<Scale>()->scale.x = 0.f;
        FlexECS::Scene::GetEntityByName("Display Mode Sprite").GetComponent<Transform>()->is_active = true;
        self.GetComponent<Transform>()->is_active = false;
      }
      if (Input::GetKeyDown(GLFW_KEY_ESCAPE) && FlexECS::Scene::GetEntityByName("Return Button Sprite") == FlexECS::Entity::Null) {
        Input::Cleanup();
        self.GetComponent<Transform>()->is_active = false;
      }
      FlexECS::Entity knob = FlexECS::Scene::GetEntityByName("SFX Knob");

      FlexECS::Entity slider_fill = FlexECS::Scene::GetEntityByName("SFX Slider Fill");
      FlexEngine::Slider* slider_details = slider_fill.GetComponent<Slider>();

      float& knob_pos = knob.GetComponent<Position>()->position.x;
      float current_volume_value = (knob_pos - slider_details->min_position) / slider_details->slider_length;

      if (Input::GetKeyDown(GLFW_KEY_D) && current_volume_value < 1.f) {
        inc_selected = true;
        current_volume_value += 0.01f;
        timer = 0.5f;
        FlexECS::Scene::GetEntityByName("SFX Volume Right").GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.f);
      }
      if (Input::GetKeyDown(GLFW_KEY_A) && current_volume_value > 0.f) {
        dec_selected = true;
        current_volume_value -= 0.01f;
        timer = 0.5f;
        FlexECS::Scene::GetEntityByName("SFX Volume Left").GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.f);
      }

      timer > 0.f ? timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() : timer = 0.f;

      if (inc_selected || dec_selected) {
        FlexECS::Scene::GetEntityByName("SFX Knob").GetComponent<Scale>()->scale = Vector3(1.25f, 1.25f, 1.f);
      }
      else {
        FlexECS::Scene::GetEntityByName("SFX Knob").GetComponent<Scale>()->scale = Vector3(1.f, 1.f, 1.f);
      }

      if (inc_selected) {
        if (Input::GetKeyUp(GLFW_KEY_D) || current_volume_value > 1.f) {
          inc_selected = false;
          FlexECS::Scene::GetEntityByName("SFX Volume Right").GetComponent<Scale>()->scale = Vector3(1.f, 1.f, 1.f);
        }
        if (timer == 0.f) current_volume_value < 1.f ? current_volume_value += 0.01f : current_volume_value = 1.f;
      }

      if (dec_selected) {
        if (Input::GetKeyUp(GLFW_KEY_A) || current_volume_value < 0.f) {
          dec_selected = false;
          FlexECS::Scene::GetEntityByName("SFX Volume Left").GetComponent<Scale>()->scale = Vector3(1.f, 1.f, 1.f);
        }
        if (timer == 0.f) current_volume_value > 0.f ? current_volume_value -= 0.01f : current_volume_value = 0.f;
      }

      FMODWrapper::Core::AdjustGroupVolume(FMODWrapper::Core::CHANNELGROUP::SFX, current_volume_value);

      knob_pos = current_volume_value * slider_details->slider_length + slider_details->min_position;
    }
  }

  void OnMouseEnter() override
  {
   /* if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active)
      self.GetComponent<Transform>()->is_active = true;*/
  }

  void OnMouseStay() override
  {
    if (Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT) && self.GetComponent<Transform>()->is_active)
    {
      // TODO: Insert Quit Game Function
      // Application::MessagingSystem::Send("MoveOne clicked", true);
    }
  }

  void OnMouseExit() override
  {
    /*self.GetComponent<Transform>()->is_active = false;*/
  }
};

// Static instance to ensure registration
static SFXButtonScript SFXButton;
