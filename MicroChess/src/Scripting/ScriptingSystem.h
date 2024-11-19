#include <windows.h>
#include <string>

class ScriptingSystem 
{
  using ScriptFunction = float(*)(float);

  void GetFunction(const std::string& function_name);
public:
  void LoadDLL(std::string const& dll_path);
  void UnloadDLL();
  ScriptingSystem() = default;
  ~ScriptingSystem() = default;

private:
  HMODULE dllHandle;
};