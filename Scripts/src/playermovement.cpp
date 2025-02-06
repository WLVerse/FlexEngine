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
  float movement_speed = 10.f;
  std::string character_relaxed_animation;
public:
  PlayerMovementScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "MovePlayer"; }

  void Awake() override
  {

  }

  void Start() override
  {
    character_relaxed_animation = R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)";
    Log::Info("PlayerMovementScript attached to entity: " + FLX_STRING_GET(*self.GetComponent<EntityName>()) + ". This script will enable sprites to move via WASD.");
  }

  void Update() override
  {
    if (Input::GetKey(GLFW_KEY_W)) {
      self.GetComponent<Position>()->position.y += movement_speed;
      self.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Up_Anim_Sheet.flxspritesheet)");
    }
    if (Input::GetKey(GLFW_KEY_A)) {
      self.GetComponent<Position>()->position.x -= movement_speed;
      self.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Left_Anim_Sheet.flxspritesheet)");
      character_relaxed_animation = R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Left_Anim_Sheet.flxspritesheet)";
    }
    if (Input::GetKey(GLFW_KEY_S)) {
      self.GetComponent<Position>()->position.y -= movement_speed;
      self.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Down_Anim_Sheet.flxspritesheet)");
    }
    if (Input::GetKey(GLFW_KEY_D)) {
      self.GetComponent<Position>()->position.x += movement_speed;
      self.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Right_Anim_Sheet.flxspritesheet)");
      character_relaxed_animation = R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)";
    }

    if (!Input::AnyKey()) {
      self.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(character_relaxed_animation);
    }
  }

  void Stop() override
  {

  }
};

// Static instance to ensure registration
static PlayerMovementScript MovePlayer;
