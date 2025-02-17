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

        #pragma region Create the entities
        #if 1
        {
            auto scene = FlexECS::Scene::CreateScene();
            {
                FlexECS::Entity entity = scene->CreateEntity("Empty Box");
                entity.AddComponent<Position>({ {600, 600, 0} });
                entity.AddComponent<Rotation>({});
                entity.AddComponent<Scale>({ {300, 200, 0} });
                entity.AddComponent<Transform>({});
                entity.AddComponent<Sprite>({ FLX_STRING_NEW("") });
            }
            {
                FlexECS::Entity entity = scene->CreateEntity("Grace");
                entity.AddComponent<Position>({
                  Vector3{ 100, 300, 0 }
                });
                entity.AddComponent<Rotation>({
                  Vector3{ 0, 0, 0 }
                });
                entity.AddComponent<Scale>({
                  { 1, 1, 1 }
                });
                entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)") });
                entity.AddComponent<Transform>({ Matrix4x4::Identity, true });
            }
            {
                FlexECS::Entity entity = scene->CreateEntity("Aniamted Grace");
                entity.AddComponent<Position>({
                  Vector3{ 100, 100, 0 }
                });
                entity.AddComponent<Rotation>({
                  Vector3{ 0, 0, 0 }
                });
                entity.AddComponent<Scale>({
                  { 1, 1, 1 }
                });
                entity.AddComponent<Transform>({});
                entity.AddComponent<Audio>({ FLX_STRING_NEW(R"(/audio/generic attack.mp3)")});
                entity.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)") });
                entity.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Attack_Anim_Sheet.flxspritesheet)"),
                  FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Attack_Anim_Sheet.flxspritesheet)")});
                entity.AddComponent<Script>({ FLX_STRING_NEW("PlayAnimation") });
            }
            // Camera Test
            {
                FlexECS::Entity cam = scene->CreateEntity("Main Camera");
                cam.AddComponent<Position>({});
                cam.AddComponent<Rotation>({});
                cam.AddComponent<Scale>({});
                cam.AddComponent<Transform>({});
                // There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
                // each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
                //  Second way is to create a camera outside and then copy constructor it -> Easier
                Camera gameTestCamera({ 0, 0, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
                cam.AddComponent<Camera>(gameTestCamera);

                //FlexECS::Entity cam2 = scene->CreateEntity("Test Cam2");
                //cam2.AddComponent<Position>({});
                //cam2.AddComponent<Rotation>({});
                //cam2.AddComponent<Scale>({});
                //cam2.AddComponent<Transform>({});
                //// There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
                //// each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
                ////  Second way is to create a camera outside and then copy constructor it -> Easier
                //Camera gameTestCamera2({ 650.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
                //cam2.AddComponent<Camera>(gameTestCamera2);
            }
            // Text test
            {
                FlexECS::Entity txt = scene->CreateEntity("Text");
                txt.AddComponent<Position>({ Vector3(822.0f, 248.0f, 0.0f) });
                txt.AddComponent<Rotation>({});
                txt.AddComponent<Scale>({});
                txt.AddComponent<Transform>({});
                txt.AddComponent<ZIndex>({ 1000 });
                txt.AddComponent<Text>({
                  FLX_STRING_NEW(R"(/fonts/Bangers/Bangers-Regular.ttf)"),
                  FLX_STRING_NEW(
                    R"(Manually update the string by adding letters to it each loop for type writing kind of animation or let the gpu handle the animation(not done, need to make Text class like camera class))"
                  ),
                  Vector3(1.0f, 0.0, 0.0f),
                  { Renderer2DText::Alignment_Center, Renderer2DText::Alignment_Middle }
                });
            }

            // button
            {
                FlexECS::Entity button = scene->CreateEntity("Button");
                button.AddComponent<Position>({ Vector3(200.0f, 200.0f, 0.0f) });
                button.AddComponent<Rotation>({});
                button.AddComponent<Scale>({ Vector3(300.0f, 100.0f, 0.0f) });
                button.AddComponent<Transform>({});
                button.AddComponent<Button>({});
                button.AddComponent<BoundingBox2D>({});
                button.AddComponent<Sprite>({});
                button.GetComponent<Sprite>()->center_aligned = true;
                button.AddComponent<Script>({ FLX_STRING_NEW("TestButton") });
            }

            //Particle System test
            {
                //FlexECS::Entity emitter = scene->CreateEntity("Test Spawner");
                //emitter.AddComponent<Position>({ Vector3(200.0f, -200.0f, 0.0f) });
                //emitter.AddComponent<Rotation>({Vector3(0,0,180)}); //In radians ah
                //emitter.AddComponent<Scale>({ Vector3(1.0f, 1.0f, 0.0f) });
                //emitter.AddComponent<Transform>({});
                //emitter.AddComponent<Sprite>({});
                //emitter.AddComponent<ParticleSystem>({});
                ////For particle systems no-choice easier to do this if manual -> too many variables
                //emitter.GetComponent<ParticleSystem>()->particlesprite_handle = FLX_STRING_NEW(R"(/images/chrono_drift_grace.png)");
                //emitter.GetComponent<ParticleSystem>()->particleEmissionShapeIndex = (int)ParticleEmitShape::Hemisphere;

                //FlexECS::Entity emitter2 = scene->CreateEntity("Test Spawner 2");
                //emitter2.AddComponent<Position>({ Vector3(600.0f, -200.0f, 0.0f) });
                //emitter2.AddComponent<Rotation>({ Vector3(0.0f,0.0f,90.0f)});
                //emitter2.AddComponent<Scale>({ Vector3(1.0f, 1.0f, 0.0f) });
                //emitter2.AddComponent<Transform>({});
                //emitter2.AddComponent<Sprite>({});
                //emitter2.AddComponent<ParticleSystem>({});
                //emitter2.GetComponent<ParticleSystem>()->particleEmissionShapeIndex = (int)ParticleEmitShape::Box;

            }
            //scene->DumpArchetypeIndex();
            // test find
            {
                scene->CreateEntity("Waldo!");
                auto entity = scene->GetEntityByName("Waldo!");
                if (entity)
                    Log::Info("Found " + FLX_STRING_GET(*entity.GetComponent<EntityName>()));
                else
                    Log::Error("Entity not found");
            }
            // scene->DumpArchetypeIndex();
        }
        #endif


        #pragma endregion

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