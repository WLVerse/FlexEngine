#include <windows.h>
#include <string>
#include "FlexEngine/FlexECS/datastructures.h"
#include "Scripting/IScript.h"

class ScriptingSystem 
{
  //using ScriptFunction = void(*)(std::shared_ptr<FlexEngine::FlexECS::Scene>);
  //using ScriptFunction = void(*)(FlexEngine::FlexECS::Manager&);
  //using GenericFunction = void(*)(void*); // Takes in all information as void* which requires reinterpret casting, use with caution.
  
  using ScriptFunction = IScript* (*)();

  void GetFunction(const std::string& function_name);
  //void GetGenericFunction(const std::string& function_name);
public:
  void LoadDLL(std::string const& dll_path);
  void UnloadDLL();
  ScriptingSystem() = default;
  ~ScriptingSystem() = default;

  //static std::map<std::string, GenericFunction> functions;
  //static std::map<std::string, ScriptFunction> ecs_functions;
  static std::map<std::string, IScript*> ecs_functions;

private:
  HMODULE dllHandle;
};