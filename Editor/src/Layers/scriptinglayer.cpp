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
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor
{

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

  void ScriptingLayer::Internal_DebugWithImGui()
  {
    FunctionQueue function_queue{};
    static bool is_playing = false;

    ImGui::Begin("Scripting Layer");

    ImGui::BeginDisabled(is_playing);

    // Loads the scripting DLL and starts the scripts
    if (ImGui::Button("Play") && !is_playing)
    {
      function_queue.Insert({
        [this]()
        {
          is_playing = true;
          Internal_LoadScriptingDLL();
        }
      });
    }

    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::BeginDisabled(!is_playing);

    // Stops the scripts and unloads the scripting DLL
    if (ImGui::Button("Stop") && is_playing)
    {
      function_queue.Insert({
        [this]()
        {
          Internal_UnloadScriptingDLL();
          is_playing = false;
        }
      });
    }

    ImGui::EndDisabled();

    if (ImGui::CollapsingHeader("Scripts", ImGuiTreeNodeFlags_DefaultOpen))
    {
      for (auto& [name, script] : ScriptRegistry::GetScripts())
      {
        ImGui::Text(script->GetName().c_str());
      }
    }

    // hardcoded test
    //if (ImGui::Button("ComponentTest Start()"))
    //{
    //  function_queue.Insert({
    //    []()
    //    {
    //      auto script = ScriptRegistry::GetScript("ComponentTest");
    //      if (script) script->Start();
    //    }
    //  });
    //}

    ImGui::End();

    function_queue.Flush();
  }

  void ScriptingLayer::OnAttach()
  {

  }

  void ScriptingLayer::OnDetach()
  {
    // auto cleanup in case the user forgot to stop the scripts
    if (is_scripting_dll_loaded)
    {
      Internal_UnloadScriptingDLL();
    }
  }

  void ScriptingLayer::Update()
  {
    // Always remember to set the context before using ImGui
    //FLX_GLFW_ALIGNCONTEXT();
    FLX_IMGUI_ALIGNCONTEXT();

    Internal_DebugWithImGui();

    // for debugging
    ScriptRegistry::Internal_Debug_Clear();

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
      if (
        transform.is_active &&
        !script_component.is_start &&
        script_component.is_awake
      )
      {
        // always remember to set the context before calling functions
        script->Internal_SetContext(entity);
        script->Start();
        script_component.is_start = true;
      }

      // update is called every frame for awake and start objects
      if (
        script_component.is_start &&
        script_component.is_awake
      )
      {
        // always remember to set the context before calling functions
        script->Internal_SetContext(entity);
        script->Update();

        // for debugging
        ScriptRegistry::Internal_Debug_LogScript(
          FLX_STRING_GET(*entity.GetComponent<EntityName>()),
          script->GetName()
        );
      }

      if (!transform.is_active && script_component.is_start)
      {
        // always remember to set the context before calling functions
        script->Internal_SetContext(entity);
        script->Stop();
        script_component.is_start = false;
      }
    }

  }

}
