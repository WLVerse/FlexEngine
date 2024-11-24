/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      edtiorlayer.cpp
\author    [90%] Rocky Sutarius
           - Main author, basically everything here.
\co-author [10%] Yew Chong
           - Migrated some code from overworld.cpp like Spawn 2500 Objects
\par       rocky.sutarius\@digipen.edu
\par       yewchong.k\@digipen.edu
\date      24 November 2024
\brief     Editor layer behavior, editor is initialized in this layer.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/


#include "editorlayer.h"
#include "Components/physics.h"
#include "Components/battlecomponents.h"
#include "Editor/editor.h"

#include "FlexEngine/Core/imguiwrapper.h"
#include "FlexEngine/Renderer/OpenGL/opengldebugrenderer.h"
#include "windowsizes.h"


namespace ChronoDrift 
{
  void EditorLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();
    Editor::GetInstance().Init();

    RegisterRenderingComponents();
    RegisterBattleComponents();

    FreeQueue::Push([]() {
      Editor::GetInstance().Shutdown();
    }, "Editor Shutdown");
  }

  void EditorLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void EditorLayer::Update()
  {
    // Jesus christ, this is a lot of code that wasn't factored before. I've shifted the old IMGUI code from base.cpp over here - YC 
    // This goes from functionqueue all the way to statistics panel
    FunctionQueue function_queue;
    
    // setup dockspace
    //ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    //ImGuiDockNodeFlags dockspace_flags = 0;
    #pragma warning(suppress: 4189) // local variable is initialized but not referenced
    ImGuiID dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

    #pragma region Title Bar

    #if 1
    {
      ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 8.0f));
      ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16.0f, 8.0f));

      if (ImGui::BeginMainMenuBar())
      {
        // center the logo
        float logo_scl = 25.f;
        float prev_cursor_y = ImGui::GetCursorPosY();
        float cursor_y = prev_cursor_y + (ImGui::GetFrameHeight() - logo_scl) * 0.5f;
        ImGui::SetCursorPosY(cursor_y);

        // display the flexengine logo
        Asset::Texture& flexengine_logo = FLX_ASSET_GET(Asset::Texture, AssetKey(R"(\images\flexengine\flexengine_logo_white.png)"));
        ImGui::Image(flexengine_logo.GetTextureImGui(), ImVec2((logo_scl / 7 * 20), logo_scl));

        // reset the cursor position
        ImGui::SetCursorPosY(prev_cursor_y);

        if (ImGui::BeginMenu("File"))
        {
          if (ImGui::MenuItem("New", "Ctrl+N"))
          {
            function_queue.Insert({
              [this]()
              {
              // Clear the scene and reset statics
              CameraManager::RemoveCameraEntities();
              FlexECS::Scene::SetActiveScene(std::make_shared<FlexECS::Scene>());

              // Every default scene should have a camera.
              FlexECS::Entity camera = FlexECS::Scene::GetActiveScene()->CreateEntity("MainCamera");
              camera.AddComponent<Position>({ {-150, 300 } });
              camera.AddComponent<Scale>({ { 0.5,0.5 } });
              camera.AddComponent<Rotation>({ });
              camera.AddComponent<Transform>({});
              camera.AddComponent<Camera>({});
              CameraManager::AddCameraEntity(camera.Get(), camera.GetComponent<Camera>()->camera);
              CameraManager::SwitchMainCamera(camera.Get());
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

                // TODO: Delete this when camera is properly done. This sets the camera to be the one from the scene
                //CameraManager::RemoveCameraEntities(); // Nuclear clear for scene loading.
                std::vector<FlexECS::Entity> camera_list = FlexECS::Scene::GetActiveScene()->Query<Camera>();
                if (camera_list.size() > 0)
                {
                  FlexECS::Entity camera = camera_list[0];
                  CameraManager::AddCameraEntity(camera.Get(), camera.GetComponent<Camera>()->camera);
                  CameraManager::SwitchMainCamera(camera.Get());
                }
                Log::Info("Loaded scene from: " + file.path.string());
              }
            });
          }

          if (ImGui::MenuItem("Save", "Ctrl+S"))
          {
            function_queue.Insert({
              [this]()
              {
                Path current_save_path = current_save_directory.append(current_save_name + ".flxscene");
                FlexECS::Scene::SaveActiveScene(File::Open(current_save_path));
                Log::Info("Saved scene to: " + current_save_path.string());
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

          //if (ImGui::MenuItem("Reload", "Ctrl+R")) {}
          //if (ImGui::MenuItem("Close", "Ctrl+W")) {}

          if (ImGui::MenuItem("Exit", "Ctrl+Q"))
          {
            Application::Close();
          }

          ImGui::EndMenu();
        }

        //if (ImGui::BeginMenu("Edit"))
        //{
        //  if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
        //  if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
        //  ImGui::EndMenu();
        //}

        if (ImGui::BeginMenu("View"))
        {
          if (ImGui::MenuItem("Center Window"))
          {
            function_queue.Insert({
              [this]()
              {
                Application::GetCurrentWindow()->CenterWindow();
              }
            });
          }
          ImGui::EndMenu();
        }

        // Set full screen
        Window* window = Application::GetCurrentWindow();
        if (ImGui::Button("Full Screen") && !window->IsFullScreen()) {
          window->CacheMiniWindowParams();

          // Get the primary monitor
          GLFWmonitor* monitor = glfwGetPrimaryMonitor();
          // Get the video mode of the monitor
          const GLFWvidmode* mode = glfwGetVideoMode(monitor);

          // Switch to fullscreen
          glfwSetWindowMonitor(window->GetGLFWWindow(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }

        // Minimize!!!
        if ((ImGui::Button("Esc") || Input::GetKeyDown(GLFW_KEY_ESCAPE)) && window->IsFullScreen()) {
          std::pair<int, int> window_pos = window->UnCacheMiniWindowsParams();
          glfwSetWindowMonitor(window->GetGLFWWindow(), NULL, window_pos.first, window_pos.second, window->GetWidth(), window->GetHeight(), window->GetFPS());
        }

        ImGui::EndMainMenuBar();
      }
      function_queue.Flush();

      ImGui::PopStyleVar(2);

      // Title bar window drag functionality
      #pragma region Window Drag Functionality

      static bool is_dragging_window = false;

      ImGuiIO& io = ImGui::GetIO();
      ImVec2 mouse_pos = io.MousePos;

      // The main menu bar height is 30 pixels
      ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImVec2 top_left = viewport->Pos;
      ImVec2 bottom_right = ImVec2(top_left.x + viewport->Size.x, top_left.y + 30.0f);

      // Window dragging state
      if (io.MouseDown[0])
      {
        if (
          io.MouseClicked[0] &&
          mouse_pos.x >= top_left.x && mouse_pos.x <= bottom_right.x &&
          mouse_pos.y >= top_left.y && mouse_pos.y <= bottom_right.y
        )
        {
          is_dragging_window = true;
        }
      }
      else
      {
        is_dragging_window = false;
      }

      // Dragging the window
      if (is_dragging_window)
      {
        Window* current_window = Application::GetCurrentWindow();
        if (current_window)
        {
          int pos_x, pos_y;
          current_window->GetWindowPosition(&pos_x, &pos_y);

          // We can't use Input::GetCursorPositionDelta() because it only handles within the glfw window
          // ImGui's IO MousePos is in global screen coordinates

          //Vector2 mouse_delta = Input::GetCursorPositionDelta();
          //if (mouse_delta.x != 0.0f || mouse_delta.y != 0.0f)
          //{
          //  current_window->SetWindowPosition(
          //    pos_x + static_cast<int>(mouse_delta.x),
          //    pos_y + static_cast<int>(mouse_delta.y)
          //  );
          //}

          if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f)
          {
            current_window->SetWindowPosition(
              pos_x + static_cast<int>(io.MouseDelta.x),
              pos_y + static_cast<int>(io.MouseDelta.y)
            );
          }
        }
      }

      #pragma endregion

    }
    #endif

    #pragma endregion

    #pragma region Statistics Panel

    #if 1
    {
      SETSTATISTICSWINDOW
        ImGui::Begin("Statistics");
      ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick; // open by default

      if (ImGui::CollapsingHeader("Window", tree_node_flags))
      {
        auto window = Application::GetCurrentWindow();
        int win_pos_x, win_pos_y;
        window->GetWindowPosition(&win_pos_x, &win_pos_y);
        ImGui::Text("Window Position: %d, %d", win_pos_x, win_pos_y);
        ImGui::Text("Window Size: %d x %d", window->GetWidth(), window->GetHeight());
        ImGui::Text("FPS: %d", window->GetFPS());
        ImGui::Text("Delta Time: %.3f ms", window->GetDeltaTime());
      }

      if (ImGui::CollapsingHeader("Scene", tree_node_flags))
      {
        ImGui::Text("Active Scene: %s", current_save_name.c_str());
        ImGui::Text("Entities: %d", FlexECS::Scene::GetActiveScene()->CachedQuery<EntityName>().size());
        ImGui::Text("Archetypes: %d", ARCHETYPE_INDEX.size());
      }

      if (ImGui::CollapsingHeader("Renderer", tree_node_flags))
      {
        ImGui::Text("Draw Calls: %d", OpenGLRenderer::GetDrawCallsLastFrame());
      }

      ImGui::End();
    }
    #endif

    #pragma endregion

    Editor::GetInstance().Update();

    SETTOOLSWINDOW
    ImGui::Begin("Tools");

    ImGui::Checkbox("Show Colliders", &show_colliders);

    if (ImGui::Button("Spawn 2500 Objects"))
    {
      auto scene = FlexECS::Scene::GetActiveScene();
      FlexECS::Entity thing = FlexECS::Scene::CreateEntity("White Queen");
      thing.AddComponent<IsActive>({ true });
      thing.AddComponent<Position>({ {0,0} });
      thing.AddComponent<Rotation>({ });
      thing.AddComponent<Scale>({ { 15,15 } });
      thing.AddComponent<ZIndex>({ 10 });
      thing.AddComponent<Transform>({ });
      thing.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"(\images\chess_queen.png)"),
        Vector3::One,
        Vector3::Zero,
        Vector3::One,
        Renderer2DProps::Alignment_Center
       });
      thing.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

      for (size_t x = 0; x < 50; x++)
      {
        for (size_t y = 0; y < 50; y++)
        {
          FlexECS::Entity cloned_thing = scene->CloneEntity(thing);
          auto& position = cloned_thing.GetComponent<Position>()->position;
          position.x = static_cast<float>(15 * (x + 1));
          position.y = static_cast<float>(15 * (y + 1));
        }
      }
    }

    ImGui::End();

    // Set the pause/play/stop buttons, no window, no title, no resize, no move, no scrollbar, no collapse, no background
    // This is really inefficient, but it's just a placeholder for now
    /*ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;*/

    // Create the window
    ImGui::Begin("##PlaybackControls");//, nullptr, windowFlags);

    // Create the buttons
    if (ImGui::Button("Play")) {
      //currentState = Play;
    }
    ImGui::SameLine();

    if (ImGui::Button("Pause")) {
      Application::GetCurrentWindow()->pauseLayers = !Application::GetCurrentWindow()->pauseLayers;
    }
    ImGui::SameLine();

    if (ImGui::Button("Stop")) {
      //currentState = Stop;
    }

    ImGui::End();

    if (show_colliders)
    {
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<BoundingBox2D>())
      {
        const Vector3& max = entity.GetComponent<BoundingBox2D>()->max;
        const Vector3& min = entity.GetComponent<BoundingBox2D>()->min;
        //construct lines for AABB
        Vector3 topleft = min;
        Vector3 topright = { max.x, min.y };
        Vector3 botleft = { min.x, max.y };
        Vector3 botright = max;
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(topleft, topright);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(topright, botright);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(botright, botleft);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(botleft, topleft);
      }
    }
  }

}