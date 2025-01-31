// WLVERSE [https://wlverse.web.app]
// editorbaselayer.h
// 
// Base layer for the editor.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"
#include "editor.h"
#include "FlexEngine/FlexECS/enginecomponents.h"
#include "editorcomponents.h"
#include "componentviewer.h"

namespace Editor
{

  void EditorBaseLayer::OnAttach()
  {

    // test scene
    #if 1
    {
      auto scene = FlexECS::Scene::CreateScene();
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 0");
        entity.AddComponent<Vector2>({ 2, 4 });
        entity.AddComponent<Transform>({});
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 1");
        entity.AddComponent<Vector2>({ 35, 42 });
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Transform>({true, Matrix4x4::Identity, true});
        entity.AddComponent<ScriptComponent>({ FLX_STRING_NEW("ComponentTest") });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 2222222222222222222222222222222222");
        entity.AddComponent<Vector2>({ 1, 2 });
        entity.AddComponent<Vector3>({ 1, 2, 3 });
        entity.AddComponent<Position>({});
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Scale>({});
        entity.AddComponent<Transform>({});
        entity.AddComponent<Audio>({ true, false, false, FLX_STRING_NEW(R"(/audio/attack.mp3)") });
        entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)"), -1});
        entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/Prop_Flaming_Barrel.flxspritesheet)"), true, 0.f});
      }
      //scene->DumpArchetypeIndex();
    }
    #endif

    // preliminary test
    #if 0
    {
      for (auto& [type_list, archetype] : scene->archetype_index)
      {
        // test conversion
        FlexECS::_Archetype _archetype_index = FlexECS::Internal_ConvertToSerializedArchetype(archetype);

        // print the archetype
        std::stringstream ss;
        for (std::size_t i = 0; i < _archetype_index.type.size(); ++i)
        {
          ss << _archetype_index.type[i] << " ";
        }
        Log::Warning("Archetype: " + ss.str());

        // print what was converted
        for (std::size_t i = 0; i < _archetype_index.archetype_table.size(); ++i)
        {
          // print the type and data
          Log::Info("Type: " + _archetype_index.type[i]);

          // print per entity
          for (std::size_t j = 0; j < _archetype_index.archetype_table[i].size(); ++j)
          {
            Log::Debug("Entity: " + std::to_string(j));
            Log::Debug("Data: " + _archetype_index.archetype_table[i][j]);
          }
        }
      }
    }
    #endif

    // test serialization
    #if 0
    {
      Path save_to_path = Path::current("temp/serialization_test_scene.flxscene");
      File& file = File::Open(save_to_path);
      scene->Save(file);
    }
    #endif

    // test deserialization
    #if 0
    {
      Path load_from_path = Path::current("temp/serialization_test_scene.flxscene");
      File& file = File::Open(load_from_path);
      auto loaded_scene = FlexECS::Scene::Load(file);
      loaded_scene->DumpArchetypeIndex();
    }
    #endif

    Editor::GetInstance().Init();
    RegisterComponents();


    // Add custom framebuffers
    Window::FrameBufferManager.AddFrameBuffer("custom1", Vector2(1080, 640));                                             
    Window::FrameBufferManager.AddFrameBuffer("custom2", Vector2(1280, 720));

  }

  void EditorBaseLayer::OnDetach()
  {
  }

  void EditorBaseLayer::Update()
  {
    Window::FrameBufferManager.SetCurrentFrameBuffer("custom1");
    Window::FrameBufferManager.GetCurrentFrameBuffer()->GetColorAttachment();

    // Always remember to set the context before using ImGui
    FLX_IMGUI_ALIGNCONTEXT();

    // setup dockspace
    //ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    //#pragma warning(suppress: 4189) // local variable is initialized but not referenced
    dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

    Editor::GetInstance().Update();

    // good practise
    OpenGLFrameBuffer::Unbind();

    // For IMGUI
    FunctionQueue function_queue;
    if (ImGui::BeginMainMenuBar())
    {
      if (ImGui::BeginMenu("File"))
      {
        if (ImGui::MenuItem("New", "Ctrl+N"))
        {
          function_queue.Insert({
            [this]()
            {
            // Clear the scene and reset statics
            FlexECS::Scene::SetActiveScene(FlexECS::Scene::CreateScene());
            current_save_name = default_save_name;
            }
            });
        }

        if (ImGui::MenuItem("Open", "Ctrl+O"))
        {
          function_queue.Insert({
            [this]()
            {
              FileList files = FileList::Browse(
                  "Open Scene",
                  Path::current("saves"), "default.flxscene",
                  L"FlexScene Files (*.flxscene)\0" L"*.flxscene\0",
                  //L"All Files (*.*)\0" L"*.*\0",
                  false,
                  false
              );

              // cancel if no files were selected
              if (files.empty()) return;

              // open the scene
              File& file = File::Open(files[0]);

              // update the current save directory and name
              current_save_directory = file.path.parent_path();
              current_save_name = file.path.stem().string();

              // load the scene
              FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
            }
          });
        }

        if (ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
        {
          function_queue.Insert({
            [this]()
            {
              FileList files = FileList::Browse(
                  "Save Scene",
                  current_save_directory, current_save_name + ".flxscene",
                  L"FlexScene Files (*.flxscene)\0" L"*.flxscene\0"
                  L"All Files (*.*)\0" L"*.*\0",
                  true
              );

              // cancel if no files were selected
              if (files.empty()) return;

              // sanitize the file extension
              std::string file_path = files[0];
              auto it = file_path.find_last_of(".");
              if (it != std::string::npos)
              {
                file_path = file_path.substr(0, it);
              }
              file_path += ".flxscene";

              // open the file
              File& file = File::Open(Path(file_path));

              // save the scene
              FlexECS::Scene::SaveActiveScene(file);
              Log::Info("Saved scene to: " + file.path.string());

              // update the current save directory and name
              current_save_directory = file.path.parent_path();
              current_save_name = file.path.stem().string();
            }
          });
        }
        
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
  
    // Execute the function queue
    function_queue.Flush();
  }

}
