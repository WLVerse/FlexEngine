// WLVERSE [https://wlverse.web.app]
// scriptregistry.h
// 
// Scripts register themselves to this registry and
// are managed by the engine.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"
#include "Reflection/base.h"
#include "FlexScripting/iscript.h"

#include <unordered_map>

namespace FlexEngine
{

  using ScriptMap = std::unordered_map<std::string, Script*>;

  // A global registry to store all scripts
  class __FLX_API ScriptRegistry
  {
  public:
    static void RegisterScript(Script* script);
    static void ClearScripts();
    static ScriptMap& GetScripts();
    static Script* GetScript(const std::string& name);
  };

  // Script component
  class __FLX_API ScriptComponent
  { FLX_REFL_SERIALIZABLE
    std::string script_name = "";
  };

}
