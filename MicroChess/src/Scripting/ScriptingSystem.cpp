#include "ScriptingSystem.h"
#include <stdexcept>
#include <iostream>
#include <map>
#include <functional>
#include "flexlogger.h"

void ScriptingSystem::LoadDLL(std::string const& dll_path)
{
  dllHandle = LoadLibraryA(dll_path.c_str());
  if (!dllHandle) {
    throw std::runtime_error("Failed to load DLL: " + dll_path);
  }

  // TODO: Load all functions from the DLL and perhaps identify where they're coming from so that we can figure out which functions
  // to slot into what hooks in the engine. ie some are supposed to be update

  GetFunction("EditingAValue"); // Temporarily put here for now
}

void ScriptingSystem::GetFunction(const std::string& function_name)
{
  // Retrieve the function pointer
  ScriptFunction func = (ScriptFunction)GetProcAddress(dllHandle, function_name.c_str());
  if (!func) {
    FreeLibrary(dllHandle); // Unload the DLL if function is not found
    throw std::runtime_error("Failed to find function 'EditingAValue' in DLL");
  }

  float f = func(100.0f); // Call the function temporarily
  FlexEngine::Log::Debug("Function 'EditingAValue' returned: " + std::to_string(f));
}

void ScriptingSystem::UnloadDLL()
{
  if (!dllHandle) return; // If the DLL is not loaded, return (no need to unload it)
  FreeLibrary(dllHandle);
}