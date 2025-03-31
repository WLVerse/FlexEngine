/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// playermovement.cpp
//
// This script handles the key inputs (W, A, S, D) that is used by the player
// to navigate through the town scene. It changes the animation of the character
// based on the movements of the character.
// 
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <FlexEngine.h>
using namespace FlexEngine;

class PlayerMovementScript : public IScript
{
private:
  float movement_speed = 400.f;
  bool is_left = false;
  #define dt Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime()

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
  }

  void Update() override
  {
    if (FlexECS::Scene::GetEntityByName("Pause Menu Background").GetComponent<Transform>()->is_active) {
      return;
    }
    if (Input::GetKey(GLFW_KEY_W))
    {
      if (!self.GetComponent<BoundingBox2D>()->is_colliding) self.GetComponent<Position>()->position.y += movement_speed * dt;
      self.GetComponent<Animator>()->spritesheet_handle =
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Up_Anim_Sheet.flxspritesheet)");
    }
    else if (Input::GetKey(GLFW_KEY_A))
    {
      if (!self.GetComponent<BoundingBox2D>()->is_colliding) self.GetComponent<Position>()->position.x -= movement_speed * dt;
      self.GetComponent<Animator>()->spritesheet_handle =
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Left_Anim_Sheet.flxspritesheet)");
      is_left = true;
    }
    else if (Input::GetKey(GLFW_KEY_S))
    {
      if (!self.GetComponent<BoundingBox2D>()->is_colliding) self.GetComponent<Position>()->position.y -= movement_speed * dt;
      self.GetComponent<Animator>()->spritesheet_handle =
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Down_Anim_Sheet.flxspritesheet)");
    }
    else if (Input::GetKey(GLFW_KEY_D))
    {
      if (!self.GetComponent<BoundingBox2D>()->is_colliding) self.GetComponent<Position>()->position.x += movement_speed * dt;
      self.GetComponent<Animator>()->spritesheet_handle =
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Run_Right_Anim_Sheet.flxspritesheet)");
      is_left = false;
    }
    else
    {
      self.GetComponent<Animator>()->spritesheet_handle = is_left ?
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Left_Anim_Sheet.flxspritesheet)") :
        FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
    }
  }

  void Stop() override
  {
  }
};

// Static instance to ensure registration
static PlayerMovementScript MovePlayer;
