#pragma once
//#include "Components/physics.h"
//#include "FlexEngine/FlexECS/datastructures.h"
#include "ScriptingInterface.h"

//extern "C" __declspec(dllimport) class Manager; // Forward declaration of Manager class

// This function is being exported from the DLL, and extern "C" to guarantee no name mangling
extern "C" __declspec(dllexport) IScript* RunPhysicsSystem(void);
//extern "C" __declspec(dllexport) void RunPhysicsSystem(FlexEngine::FlexECS::Manager& s);

extern "C" __declspec(dllexport) void ModifyAnInt(void* valueptr);