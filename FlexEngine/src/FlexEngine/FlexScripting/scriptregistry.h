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
#include "FlexEngine/FlexECS/datastructures.h"

#include <unordered_map>

namespace FlexEngine
{

  // A map of script names to script pointers
  using ScriptMap = std::unordered_map<std::string, IScript*>;

  // A global registry to store all scripts
  class __FLX_API ScriptRegistry
  {
  public:

    // Register a script to the registry
    static void RegisterScript(IScript* script);

    // Clear all scripts in the registry
    static void ClearScripts();

    // Get the registry.
    // Be careful with this function, prefer using the other functions to interact with the registry.
    static ScriptMap& GetScripts();

    // Get a script by its name
    static IScript* GetScript(const std::string& name);

    #pragma region Debugging functions

    #ifdef _DEBUG

    // Count how many scripts ran and what their names + entities are

  public:
    // A pair of entity and script names
    static std::vector<std::pair<std::string, std::string>> scripts_run_names;

    // INTERNAL FUNCTION
    // Helper function to log the entity and script name in a vector
    static void Internal_Debug_LogScript(const std::string& entity_name, const std::string& script_name) { scripts_run_names.push_back({ entity_name, script_name }); }

    // INTERNAL FUNCTION
    // Clear the debug vector of entity and script names
    static void Internal_Debug_Clear() { scripts_run_names.clear(); }

    // Get the number of times the scripts have been run
    static std::size_t GetScriptsRunCount() { return scripts_run_names.size(); }

    // Get the pair of entity and script names
    static const std::vector<std::pair<std::string, std::string>>& GetScriptsRunNames() { return scripts_run_names; }

    // Get the names of the scripts that have been run in a formatted string
    // Example: "EntityName ran ScriptName\n"
    static std::string GetFormattedScriptsRunNames()
    {
      std::string result;
      for (const auto& [script_name, entity_name] : scripts_run_names)
      {
        result += entity_name + " ran " + script_name + "\n";
      }
      return result;
    }

    #else

    // Disable the debug functions in release mode

  public:
    static void Internal_Debug_LogScript(const std::string&, const std::string&) {}
    static void Internal_Debug_Clear() {}
    static std::size_t GetScriptsRunCount() { return 0; }
    static const std::vector<std::pair<std::string, std::string>>& GetScriptsRunNames() { return {}; }
    static std::string GetFormattedScriptsRunNames() { return ""; }

    #endif

    #pragma endregion
  };

}
