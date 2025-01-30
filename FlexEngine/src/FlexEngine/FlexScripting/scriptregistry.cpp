// WLVERSE [https://wlverse.web.app]
// scriptregistry.cpp
// 
// Scripts register themselves to this registry and
// are managed by the engine.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "FlexScripting/scriptregistry.h"

namespace FlexEngine
{
  // static member initialization
  #ifdef _DEBUG
  std::vector<std::pair<std::string, std::string>> ScriptRegistry::scripts_run_names{};
  #endif

  void ScriptRegistry::RegisterScript(IScript* script)
  {
    GetScripts().insert({ script->GetName(), script});
  }

  void ScriptRegistry::ClearScripts()
  {
    GetScripts().clear();
  }

  ScriptMap& ScriptRegistry::GetScripts()
  {
    static ScriptMap scripts;
    return scripts;
  }

  IScript* ScriptRegistry::GetScript(const std::string& name)
  {
    auto& scripts = GetScripts();
    if (scripts.count(name) == 0)
    {
      Log::Error("The script " + name + " does not exist in the script registry.");
      return nullptr;
    }
    return scripts[name];
  }

}
