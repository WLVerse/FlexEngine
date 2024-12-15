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

  #pragma region Reflection

  FLX_REFL_REGISTER_START(ScriptComponent)
    FLX_REFL_REGISTER_PROPERTY(script_name)
  FLX_REFL_REGISTER_END;

  #pragma endregion

  void ScriptRegistry::RegisterScript(Script* script)
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

  Script* ScriptRegistry::GetScript(const std::string& name)
  {
    auto& scripts = GetScripts();
    if (scripts.count(name) == 0)
    {
      Log::Error("Script not found: " + name);
      return nullptr;
    }
    return scripts[name];
  }

}
