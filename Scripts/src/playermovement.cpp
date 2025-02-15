// WLVERSE [https://wlverse.web.app]
// playermovement.cpp
//
// scripting cpp file for player movement
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include <FlexEngine.h>
using namespace FlexEngine;

class PlayerMovementScript : public IScript
{
private:
  float movement_speed = 8.f;
  FlexECS::Scene::StringIndex anim_idle_right;
  FlexECS::Scene::StringIndex anim_idle_left;
  FlexECS::Scene::StringIndex anim_up;
  FlexECS::Scene::StringIndex anim_down;
  FlexECS::Scene::StringIndex anim_left;
  FlexECS::Scene::StringIndex anim_right;
  bool is_left = false;

public:
  PlayerMovementScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MovePlayer";
  }

  void Awake() override
  {
  }

  void Start() override
  {
    anim_idle_right = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
    anim_idle_left = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Left_Anim_Sheet.flxspritesheet)");
    anim_up = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Up_Anim_Sheet.flxspritesheet)");
    anim_down = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Down_Anim_Sheet.flxspritesheet)");
    anim_left = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Left_Anim_Sheet.flxspritesheet)");
    anim_right = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Right_Anim_Sheet.flxspritesheet)");
  }

  void Update() override
  {
    if (Input::GetKey(GLFW_KEY_W))
    {
      self.GetComponent<Position>()->position.y += movement_speed;
      self.GetComponent<Scale>()->scale = { 41, 99, 0 };
      self.GetComponent<Animator>()->spritesheet_handle = anim_up;
    }
    if (Input::GetKey(GLFW_KEY_A))
    {
      self.GetComponent<Position>()->position.x -= movement_speed;
      self.GetComponent<Scale>()->scale = { 58, 92, 0 };
      self.GetComponent<Animator>()->spritesheet_handle = anim_left;
      is_left = true;
    }
    if (Input::GetKey(GLFW_KEY_S))
    {
      self.GetComponent<Position>()->position.y -= movement_speed;
      self.GetComponent<Scale>()->scale = { 40, 99, 0 };
      self.GetComponent<Animator>()->spritesheet_handle = anim_down;
    }
    if (Input::GetKey(GLFW_KEY_D))
    {
      self.GetComponent<Position>()->position.x += movement_speed;
      self.GetComponent<Scale>()->scale = { 58, 92, 0 };
      self.GetComponent<Animator>()->spritesheet_handle = anim_right;
      is_left = false;
    }

    if (!Input::GetKey(GLFW_KEY_W) && !Input::GetKey(GLFW_KEY_A) && !Input::GetKey(GLFW_KEY_S) &&
        !Input::GetKey(GLFW_KEY_D))
    {
      self.GetComponent<Scale>()->scale = { 40, 93, 0 };
      self.GetComponent<Animator>()->spritesheet_handle = is_left ? anim_idle_left : anim_idle_right;
    }

    self.GetComponent<Scale>()->scale *= 1.5;
  }

  void Stop() override
  {
  }
};

// Static instance to ensure registration
static PlayerMovementScript MovePlayer;
