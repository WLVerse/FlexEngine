//Component includes
#include "ScriptLibrary.h"

class PhysicsScript : public IScript
{
public:
  void onUpdate(std::shared_ptr<FlexEngine::FlexECS::Scene> ecs) override
  {
    FlexEngine::FlexECS::ecs_manager.CreateEntity("PhysicsEntity");
  }
};

#include <iostream>
IScript* RunPhysicsSystem(void)
//void RunPhysicsSystem(FlexEngine::FlexECS::Manager& s)
{
  return new PhysicsScript();
}

// Ok I know this works, tested it on an int* and it does indeed modify the value at the exe.
void ModifyAnInt(void* valueptr)
{
  int* value = reinterpret_cast<int*>(valueptr);
  *value = 100;
}