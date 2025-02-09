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

    #if 1
    {
      auto scene = FlexECS::Scene::CreateScene();
      FlexECS::Scene::SetActiveScene(scene);
      // Camera Test
      {
        FlexECS::Entity cam = scene->CreateEntity("Test Cam");
        cam.AddComponent<Position>({});
        cam.AddComponent<Rotation>({});
        cam.AddComponent<Scale>({});
        cam.AddComponent<Transform>({});
        // There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
        // each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        //  Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera({ 850.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam.AddComponent<Camera>(gameTestCamera);

        FlexECS::Entity cam2 = scene->CreateEntity("Test Cam2");
        cam2.AddComponent<Position>({});
        cam2.AddComponent<Rotation>({});
        cam2.AddComponent<Scale>({});
        cam2.AddComponent<Transform>({});
        // There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
        // each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        //  Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera2({ 650.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam2.AddComponent<Camera>(gameTestCamera2);
      }


      FlexECS::Entity ui = FlexECS::Scene::CreateEntity("UI_Lose_V");
      ui.AddComponent<Position>({ Vector3(0, -425 , 0) });
      ui.AddComponent<Scale>({ Vector3(56, 27, 1) });
      ui.AddComponent<Rotation>({});
      ui.AddComponent<Transform>({});
      ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Lose_V.png") });

      ui = FlexECS::Scene::CreateEntity("Player Stats");
      ui.AddComponent<Position>({ Vector3(-634, -48, 0) });
      ui.AddComponent<Scale>({ Vector3(400, 113, 1) });
      ui.AddComponent<Rotation>({});
      ui.AddComponent<Transform>({});
      ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Player Stats.png") });

      ui = FlexECS::Scene::CreateEntity("Press any button");
      ui.AddComponent<Position>({ Vector3(0, -370, 0) });
      ui.AddComponent<Scale>({ Vector3(455, 44, 1) });
      ui.AddComponent<Rotation>({});
      ui.AddComponent<Transform>({});
      ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Text_Press Any Button To Continue.png") });

      ui = FlexECS::Scene::CreateEntity("Sample Text");
      ui.AddComponent<Position>({ Vector3(0, -370, 0) });
      ui.AddComponent<Scale>({ Vector3(455, 44, 1) });
      ui.AddComponent<Rotation>({});
      ui.AddComponent<Transform>({});
      ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)") });
      #pragma endregion
    }
    #endif 

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
    }


    void EditorBaseLayer::OnDetach()
    {
    }

    void EditorBaseLayer::Update()
    {
      //Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
      //Window::FrameBufferManager.GetCurrentFrameBuffer()->GetColorAttachment();

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
                        FlexECS::Scene::SetActiveScene(FlexECS::Scene::CreateScene());
                        current_save_name = default_save_name;
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
                                              FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
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
                                            } });
                }

                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        // Execute the function queue
        function_queue.Flush();
    }

} // namespace Editor




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


#if 0 // Test case
static std::string extra = "";
if (Input::GetKey(GLFW_KEY_J))
extra += "j";
Renderer2DText sample;
static std::string fullText = "The whole human fraternity is becoming highly dependent on the computer technology; no one can imagine life without computer. As, it has spread its wings so deeply in every area and made people used of it. It is very beneficial for the students of any class. They can use it to prepare their projects, learn poems, read different stories, download notes for exam preparations, collect large information within seconds, learn about painting, drawing, etc. However it enhances the professional skills of the students and helps in getting job easily.";
static std::string displayedText = ""; // Start with an empty string
static float elapsedTime = 0.0f;       // To track time
elapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 100;
if (elapsedTime >= 1.0f && displayedText.size() < fullText.size()) {
    displayedText += fullText[displayedText.size()]; // Append the next character
    elapsedTime = 0.0f; // Reset the timer
}
sample.m_words = displayedText;
//sample.m_words = "hello there my name is markiplier and welcome back to another game of amnesia the dark descent" + extra;
sample.m_color = Vector3::Zero;
sample.m_fonttype = R"(/fonts/Bangers/Bangers-Regular.ttf)";
sample.m_transform = Matrix4x4(1.00, 0.00, 0.00, 0.00,
    0.00, 1.00, 0.00, 0.00,
    0.00, 0.00, 1.00, 0.00,
     822.00, 248.00, 0.00, 1.00);
sample.m_window_size = Vector2(static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()), static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()));
//sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Top };
//sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Middle };
//sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Bottom };
//sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Top };
sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle };
//sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Bottom };
//sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Top };
//sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Middle };
//sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Bottom };
sample.m_maxwidthtextbox = 850.0f;
#endif


// Test for cam
#if 0
auto cameraEntities = FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>();

for (auto& entity : cameraEntities)
{

    auto camera = entity.GetComponent<Camera>();
    if (camera)
    {
        auto& position = camera->m_data.position;

        // Adjust movement speed as needed
        float speed = 300.0f;

        // Check for WASD input
        if (Input::GetKey('W')) // Replace 'W' with your input library's key codes
            entity.GetComponent<Camera>()->m_data.position.y -=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move forward
        if (Input::GetKey('S'))
            entity.GetComponent<Camera>()->m_data.position.y +=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move backward
        if (Input::GetKey('A'))
            entity.GetComponent<Camera>()->m_data.position.x -=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move left
        if (Input::GetKey('D'))
            entity.GetComponent<Camera>()->m_data.position.x +=
            speed * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime(); // Move right

        entity.GetComponent<Transform>()->is_dirty = true;
    }
}
#endif