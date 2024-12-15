#include <FlexEngine.h>
using namespace FlexEngine;

class ComponentTestScript : public Script
{
public:
  ComponentTestScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "ComponentTest"; }

  void Start() override
  {
    std::cout << "ComponentTest: Start" << std::endl;

    //Vec2 pos(1.0f, 2.0f);
    //pos.Print();
    //pos.Invert();
    //pos.Print();

    //Reflection::TypeDescriptor* type = Reflection::TypeResolver<Vec2>::Get();
    //std::cout << "Type: " << type->ToString() << std::endl;
    //type->Dump(&pos);
  }

  void Update() override
  {

  }

  void Stop() override
  {
    std::cout << "ComponentTest: Stop" << std::endl;
  }
};

// Static instance to ensure registration
static ComponentTestScript ComponentTest;
