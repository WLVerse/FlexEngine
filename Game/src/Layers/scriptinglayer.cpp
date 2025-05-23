// WLVERSE [https://wlverse.web.app]
// scriptinglayer.cpp
//
// Scripting layer for the editor. 
// 
// Very rough implementation of hotloading a scripting DLL. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Layers.h"

namespace Game
{
  void ScriptingLayer::Internal_UnloadScriptingDLL()
  {
    // guard
    if (!is_scripting_dll_loaded) return;

    // call stop for all scripts
    for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Script>())
    {
      auto& script_component = *entity.GetComponent<Script>();
      auto script = ScriptRegistry::GetScript(FLX_STRING_GET(script_component.script_name));

      // guard
      if (!script)
      {
        Log::Warning("Script: " + FLX_STRING_GET(script_component.script_name) + " does not exist.");
        continue;
      }

      // always remember to set the context before calling functions
      script->Internal_SetContext(entity);
      script->Stop();
    }

    // unload the scripting DLL
    if (hmodule_scripting) FreeLibrary(hmodule_scripting);
    hmodule_scripting = nullptr;
    ScriptRegistry::ClearScripts();

    is_scripting_dll_loaded = false;
    ScriptRegistry::is_running = false;
  }
  void ScriptingLayer::Internal_LoadScriptingDLL()
  {
    // guard
    if (is_scripting_dll_loaded) return;

    // Create the hotload directory if it does not exist
    if (!std::filesystem::exists(Path::current("hotload"))) std::filesystem::create_directory(Path::current("hotload"));

    // Get the path of the DLL
    std::string from = Path::current("Scripts.dll");
    std::string to = Path::current("hotload/Scripts.dll");

    auto& layerstack = Application::GetCurrentWindow()->GetLayerStack();

    // guard: DLL does not exist
    if (!std::filesystem::exists(from))
    {
      Log::Error("DLL for hotloading does not exist. Remember to build the scripting sln first.");
      layerstack.RemoveLayer(this->GetName()); // Remove self
      return;
    }

    // Copy the DLL to the hot loading directory
    bool success = std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing);
    if (!success)
    {
      Log::Error("Failed to copy DLL for hotloading.");
      layerstack.RemoveLayer(this->GetName()); // Remove self
      return;
    }

    // Load the scripting DLL
    LPCSTR dll_path = to.c_str();
    hmodule_scripting = LoadLibraryA(dll_path);
    if (!hmodule_scripting)
    {
      Log::Error("Failed to load DLL. Error code: " + std::to_string(GetLastError()));
      layerstack.RemoveLayer(this->GetName()); // Remove self
      return;
    }

    Log::Info("Loaded DLL: " + to);
    is_scripting_dll_loaded = true;
    ScriptRegistry::is_running = true;
  }

  void ScriptingLayer::OnAttach()
  {
    Internal_LoadScriptingDLL();
  }

  void ScriptingLayer::OnDetach()
  {
    // auto cleanup in case the user forgot to stop the scripts
    if (is_scripting_dll_loaded) Internal_UnloadScriptingDLL();
  }

  void ScriptingLayer::Update()
  {
    // guard
    if (!is_scripting_dll_loaded) return;

    // process all scripts, multiple of the same script can exist
    // order is not guaranteed and should never be
    // call awake, start, update, and stop
    for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Script>())
    {
      Transform& transform = *entity.GetComponent<Transform>();
      if (!transform.is_active) continue; // skip non active entities

      auto& script_component = *entity.GetComponent<Script>();
      auto script = ScriptRegistry::GetScript(FLX_STRING_GET(script_component.script_name));

      // guard
      if (!script)
      {
        Log::Warning("Script: " + FLX_STRING_GET(script_component.script_name) + " does not exist.");
        continue;
      }

      // awake is called once when the script is created
      if (!script_component.is_awake)
      {
        // always remember to set the context before calling functions
        script->Internal_SetContext(entity);
        script->Awake();
        script_component.is_awake = true;
      }

      // start is called once when the object is enabled for the first time
      if (transform.is_active && !script_component.is_start && script_component.is_awake)
      {
        // always remember to set the context before calling functions
        script->Internal_SetContext(entity);
        script->Start();
        script_component.is_start = true;
      }

      // update is called every frame for awake and start objects
      if (script_component.is_start && script_component.is_awake)
      {
        // always remember to set the context before calling functions
        script->Internal_SetContext(entity);
        script->Update();

        // for debugging
        ScriptRegistry::Internal_Debug_LogScript(FLX_STRING_GET(*entity.GetComponent<EntityName>()), script->GetName());
      }

      if (!transform.is_active && script_component.is_start)
      {
        // always remember to set the context before calling functions
        script->Internal_SetContext(entity);
        script->Stop();
        script_component.is_start = false;
      }
    }

    // process callback for scripts
    for (FlexECS::Entity& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Script, BoundingBox2D>())
    {
      auto& bb = *entity.GetComponent<BoundingBox2D>();

      if (bb.is_mouse_over || bb.is_mouse_over_cached)
      {
        auto& script_name = FLX_STRING_GET(entity.GetComponent<Script>()->script_name);
        auto script = ScriptRegistry::GetScript(script_name);
        FLX_NULLPTR_ASSERT(script, "An expected script is missing from the script registry: " + script_name);

        script->Internal_SetContext(entity);

        if (bb.is_mouse_over && !bb.is_mouse_over_cached)
          script->OnMouseEnter();
        else if (bb.is_mouse_over)
          script->OnMouseStay();
        else if (!bb.is_mouse_over && bb.is_mouse_over_cached)
          script->OnMouseExit();
      }
    }
  }

} // namespace Editor