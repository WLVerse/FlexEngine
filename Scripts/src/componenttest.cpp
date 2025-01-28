#include <FlexEngine.h>
using namespace FlexEngine;

class ComponentTestScript : public Script
{
public:
  ComponentTestScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "ComponentTest"; }
  
  void Awake() override
  {
    Log::Debug("ComponentTest: Awake");
  }

  void Start() override
  {
    Log::Debug("ComponentTest: Start");

    // simple test to display the mouse position
    Vector2 mouse_pos = Input::GetMousePosition();
    std::stringstream ss;
    ss << "Mouse Position: " << mouse_pos.x << ", " << mouse_pos.y;
    Log::Debug(ss);

    //Reflection::TypeDescriptor* type = Reflection::TypeResolver<Vec2>::Get();
    //std::cout << "Type: " << type->ToString() << std::endl;
    //type->Dump(&pos);
  }

  void Update() override
  {
    //Log::Debug("ComponentTest: Update");
  }

  void Stop() override
  {
    Log::Debug("ComponentTest: Stop");
  }
};

// Static instance to ensure registration
static ComponentTestScript ComponentTest;
