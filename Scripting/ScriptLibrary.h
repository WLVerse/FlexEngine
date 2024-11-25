#pragma once
//#include "Components/physics.h"
#include "FlexEngine/FlexECS/datastructures.h"
#include "FlexEngine/Scripting/IScript.h"

// This function is being exported from the DLL, and extern "C" to guarantee no name mangling
//extern "C" __declspec(dllexport) void RunPhysicsSystem(std::shared_ptr<FlexEngine::FlexECS::Scene> s);
//extern "C" __declspec(dllexport) void RunPhysicsSystem(FlexEngine::FlexECS::Manager& s);

//extern "C" __declspec(dllexport) void ModifyAnInt(void* valueptr);

class AddPhysicsEntityScript : public IScript
{
public:
  void Update(FlexEngine::FlexECS::Manager& s) override;
};


extern "C" __declspec(dllexport) IScript* CreateAddPhysicsEntityScript();