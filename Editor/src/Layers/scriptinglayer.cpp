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

#include <thread>

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
      std::stringstream ss;
      ss << "Failed to load DLL. Error code: " << std::to_string(GetLastError());
      Log::Error(ss);
      layerstack.RemoveLayer(this->GetName()); // Remove self
      return;
    }
    Log::Info("Loaded DLL: " + to);
    
    is_scripting_dll_loaded = true;
  }

  void ScriptingLayer::Internal_UnloadScriptingDLL()
  {
    // guard
    if (!is_scripting_dll_loaded) return;

    if (hmodule_scripting) FreeLibrary(hmodule_scripting);
    hmodule_scripting = nullptr;
    ScriptRegistry::ClearScripts();

    is_scripting_dll_loaded = false;
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

    if (ImGui::Button("ComponentTest Start()"))
    {
      function_queue.Insert({
        []()
        {
          auto script = ScriptRegistry::GetScript("ComponentTest");
          if (script) script->Start();
        }
      });
    }

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

    auto script = ScriptRegistry::GetScript("GameplayLoops");
    if (script) script->Update();
  }

}
