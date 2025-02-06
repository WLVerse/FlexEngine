#include <FlexEngine.h>
using namespace FlexEngine;

class PlayerMovementScript : public IScript
{
private:
  float movement_speed = 10.f;
public:
  PlayerMovementScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "MovePlayer"; }

  void Awake() override
  {

  }

  void Start() override
  {
    Log::Info("PlayerMovementScript attached to entity: " + FLX_STRING_GET(*self.GetComponent<EntityName>()) + ". This script will enable sprites to move via WASD.");

  }

  void Update() override
  {
    if (Input::GetKey(GLFW_KEY_W)) self.GetComponent<Position>()->position.y += movement_speed;
    if (Input::GetKey(GLFW_KEY_A)) self.GetComponent<Position>()->position.x -= movement_speed;
    if (Input::GetKey(GLFW_KEY_S)) self.GetComponent<Position>()->position.y -= movement_speed;
    if (Input::GetKey(GLFW_KEY_D)) self.GetComponent<Position>()->position.x += movement_speed;
    
  }

  void Stop() override
  {

  }
};

// Static instance to ensure registration
static PlayerMovementScript MovePlayer;
