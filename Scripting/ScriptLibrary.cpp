////Component includes
#include "ScriptLibrary.h"
//
//#include <iostream>
////void RunPhysicsSystem(std::shared_ptr<FlexEngine::FlexECS::Scene> s)
//void RunPhysicsSystem(FlexEngine::FlexECS::Manager& s)
//{
//  //std::cout << "Just to test this works, find the DLL again to test scene changes" << std::endl;
//  
//  //FlexEngine::FlexECS::Manager::GetInstance().CreateEntity("PhysicsEntity");
//  //s->DumpEntityIndex();
//  
//  //s.GetActiveScene()->DumpEntityIndex();
//  s.CreateEntity("PhysicsEntity");
//  //s.GetActiveScene()->DumpEntityIndex();
//}
//
//// Ok I know this works, tested it on an int* and it does indeed modify the value at the exe.
//void ModifyAnInt(void* valueptr)
//{
//  int* value = reinterpret_cast<int*>(valueptr);
//  *value = 100;
//}

void AddPhysicsEntityScript::Update(FlexEngine::FlexECS::Manager& s)
{
  s.CreateEntity("PhysicsEntity");
}

IScript* CreateAddPhysicsEntityScript()
{
  return new AddPhysicsEntityScript();
}