// WLVERSE [https://wlverse.web.app]
// editorbaselayer.cpp
//
// Base layer for the editor. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "FlexEngine/FlexECS/enginecomponents.h"
#include "Layers.h"
#include "componentviewer.h"
#include "editor.h"
#include "editorcomponents.h"

namespace Editor
{
    void EditorBaseLayer::OnAttach()
    {
        auto scene = FlexECS::Scene::CreateScene();

        Editor::GetInstance().Init();
        RegisterComponents();

        // Add custom framebuffers
        Vector2 window_size = Vector2(static_cast<float>(Application::GetCurrentWindow()->GetWidth()), static_cast<float>(Application::GetCurrentWindow()->GetHeight()));
        Window::FrameBufferManager.AddFrameBuffer("Scene", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Game", window_size);

        // add physics layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<PhysicsLayer>());

        // add particle systems layer
        FLX_COMMAND_ADD_WINDOW_LAYER(
          "Editor",
          std::make_shared<ParticleSystemLayer>()
        );

        // add rendering layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<RenderingLayer>());

        // add audio layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<AudioLayer>());

        // add camera service layer
        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<CameraSystemLayer>());

        FLX_COMMAND_ADD_WINDOW_LAYER("Editor", std::make_shared<ProfilerLayer>());
    }

    void EditorBaseLayer::OnDetach()
    {
    }

    void EditorBaseLayer::Update()
    {
      Profiler::StartCounter("Editor");
      // Always remember to set the context before using ImGui
      FLX_IMGUI_ALIGNCONTEXT();

      // setup dockspace
      // ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode |
      // ImGuiDockNodeFlags_NoDockingInCentralNode;
      ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
      // #pragma warning(suppress: 4189) // local variable is initialized but not referenced
      dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

      Editor::GetInstance().Update();
      
      // For IMGUI
      FunctionQueue function_queue;
      if (ImGui::BeginMainMenuBar())
      {
          if (ImGui::BeginMenu("File"))
          {
              if (ImGui::MenuItem("New", "Ctrl+N"))
              {
                  function_queue.Insert({ [this]()
                                          {
                      // Clear the scene and reset statics
                      Editor::GetInstance().GetSystem<SelectionSystem>()->ClearSelection();
                      FlexECS::Scene::SetActiveScene(FlexECS::Scene::CreateScene());
                      current_save_name = default_save_name;
                      Window::FrameBufferManager.GetFrameBuffer("Scene")->Clear();
                    } });
              }

              if (ImGui::MenuItem("Open", "Ctrl+O"))
              {
                  function_queue.Insert({ [this]()
                                          {
                                            FileList files = FileList::Browse(
                                              "Open Scene", Path::current("saves"), "default.flxscene",
                                              L"FlexScene Files (*.flxscene)\0"
                                              L"*.flxscene\0",
                                              // L"All Files (*.*)\0" L"*.*\0",
                                              false, false
                                            );

                                            // cancel if no files were selected
                                            if (files.empty()) return;

                                            // open the scene
                                            File& file = File::Open(files[0]);

                                            // update the current save directory and name
                                            current_save_directory = file.path.parent_path();
                                            current_save_name = file.path.stem().string();

                                            // load the scene
                                            Editor::GetInstance().GetSystem<SelectionSystem>()->ClearSelection();
                                            FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
                                            Window::FrameBufferManager.GetFrameBuffer("Scene")->Clear();
                                          } });
              }

              if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
              {
                  function_queue.Insert({ [this]()
                                          {
                                            FileList files = FileList::Browse(
                                              "Save Scene", current_save_directory, current_save_name + ".flxscene",
                                              L"FlexScene Files (*.flxscene)\0"
                                              L"*.flxscene\0"
                                              L"All Files (*.*)\0"
                                              L"*.*\0",
                                              true
                                            );

                                            // cancel if no files were selected
                                            if (files.empty()) return;

                                            // sanitize the file extension
                                            std::string file_path = files[0];
                                            auto it = file_path.find_last_of(".");
                                            if (it != std::string::npos) file_path = file_path.substr(0, it);
                                            file_path += ".flxscene";

                                            // open the file
                                            File& file = File::Open(Path(file_path));

                                            // save the scene
                                            FlexECS::Scene::SaveActiveScene(file);
                                            Log::Info("Saved scene to: " + file.path.string());

                                            // update the current save directory and name
                                            current_save_directory = file.path.parent_path();
                                            current_save_name = file.path.stem().string();

                                            Window::FrameBufferManager.GetFrameBuffer("Scene")->Clear();
                                          } });
              }
              if (ImGui::MenuItem("Settings", "Ctrl+S"))
              {
                  SettingsPanel* settingsPanel = Editor::GetInstance().GetPanel<SettingsPanel>();
                  if (settingsPanel)
                  {
                      // Toggle the open state: if open, close it; if closed, open it.
                      settingsPanel->SetOpen(!settingsPanel->IsOpen());
                  }
              }

              ImGui::EndMenu();
          }
          ImGui::EndMainMenuBar();
      }

      // Execute the function queue
      function_queue.Flush();

      Profiler::EndCounter("Editor");
    }

} // namespace Editor