#include <windows.h>
#include <string>
#include "FlexEngine/FlexECS/datastructures.h"

class ScriptingSystem 
{
  using ScriptFunction = void(*)(std::shared_ptr<FlexEngine::FlexECS::Scene>);
  using GenericFunction = void(*)(void*); // Takes in all information as void* which requires reinterpret casting, use with caution.

  void GetFunction(const std::string& function_name);
public:
  void LoadDLL(std::string const& dll_path);
  void UnloadDLL();
  ScriptingSystem() = default;
  ~ScriptingSystem() = default;

  static std::map<std::string, GenericFunction> functions;
  static std::map<std::string, ScriptFunction> ecs_functions;

private:
  HMODULE dllHandle;
};