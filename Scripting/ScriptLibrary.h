#pragma once
//#include "Components/physics.h"
#include "FlexEngine/FlexECS/datastructures.h"

// This function is being exported from the DLL, and extern "C" to guarantee no name mangling
extern "C" __declspec(dllexport) void RunPhysicsSystem(std::shared_ptr<FlexEngine::FlexECS::Scene> s);

extern "C" __declspec(dllexport) void ModifyAnInt(void* valueptr);