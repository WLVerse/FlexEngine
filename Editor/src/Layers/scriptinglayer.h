#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class ScriptingLayer : public FlexEngine::Layer
  {
    bool is_scripting_dll_loaded = false;

    HMODULE hmodule_scripting{};
    using ScriptingFunctionType = void (*)();

    // INTERNAL FUNCTION
    // Load the scripting DLL using LoadLibrary
    void Internal_LoadScriptingDLL();

    // INTERNAL FUNCTION
    // Unload the scripting DLL using FreeLibrary
    void Internal_UnloadScriptingDLL();

    void Internal_DebugWithImGui();

  public:
    ScriptingLayer() : Layer("Scripting Layer") {}
    ~ScriptingLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}