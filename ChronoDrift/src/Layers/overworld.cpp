/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      overworld.cpp
\author    [50%] Yew Chong (yewchong.k\@digipen.edu)
           - Main query loops, profiler, memleak code
\co-author [50%] Soh Wei Jie (weijie.soh\@digipen.edu)
           - Camera code, add registering
\date      24 November 2024
\brief     General layer for almost everything in the game to run.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/


#include "overworld.h"

#include "States.h"
#include "Layers.h"

#include "Components/battlecomponents.h"
#include "Components/physics.h"
#include "Components/rendering.h"
#include "Components/characterinput.h"

#include "BattleSystems/physicssystem.h"

#include "Renderer/sprite2d.h"

#include "Renderer/camera2d.h"

namespace ChronoDrift 
{
    void OverworldLayer::SetupWorld()
    {
        auto scene = FlexECS::Scene::GetActiveScene();

        //Camera 
        #if 1
        FlexECS::Entity camera = FlexECS::Scene::CreateEntity("MainCamera");
        camera.AddComponent<IsActive>({ true });
        camera.AddComponent<Position>({ {0,0} });
        camera.AddComponent<Scale>({ { static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth())/10,static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()) / 10 } }); // Screen display 1280 x 750
        camera.AddComponent<Rotation>({ });
        camera.AddComponent<Transform>({});
        camera.AddComponent<Camera>({});
        m_CamM_Instance->AddCameraEntity(camera.Get(), camera.GetComponent<Camera>()->camera);
        m_CamM_Instance->SwitchMainCamera(camera.Get());

        #endif
    }



    void OverworldLayer::OnAttach()
    {
        FLX_FLOW_BEGINSCOPE();

        // ECS Setup
        auto scene = FlexECS::Scene::CreateScene();
        FlexECS::Scene::SetActiveScene(scene);

        // Setup for level editor
        #ifndef GAME
        RegisterRenderingComponents();
        RegisterPhysicsComponents();
        #endif
        SetupWorld();

        #ifdef _MEMLEAK
        int* a = new int;
        #endif
    }

    void OverworldLayer::OnDetach()
    {
        FLX_FLOW_ENDSCOPE();
    }

  void OverworldLayer::Update()
  {
      m_CamM_Instance->ValidateMainCamera();

      Profiler profiler;

      profiler.StartCounter("Custom Query Loops");
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<CharacterInput>())
      {
          entity.GetComponent<CharacterInput>()->up = Input::GetKey(GLFW_KEY_W);
          entity.GetComponent<CharacterInput>()->down = Input::GetKey(GLFW_KEY_S);
          entity.GetComponent<CharacterInput>()->left = Input::GetKey(GLFW_KEY_A);
          entity.GetComponent<CharacterInput>()->right = Input::GetKey(GLFW_KEY_D);
      }

      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<CharacterInput, Rigidbody>())
      {
          auto& velocity = entity.GetComponent<Rigidbody>()->velocity;
          velocity.x = 0.0f;
          velocity.y = 0.0f;

          if (entity.GetComponent<CharacterInput>()->up)
          {
              velocity.y = -300.0f;
          }

          if (entity.GetComponent<CharacterInput>()->down)
          {
              velocity.y = 300.0f;
          }

          if (entity.GetComponent<CharacterInput>()->left)
          {
              velocity.x = -300.0f;
          }

          if (entity.GetComponent<CharacterInput>()->right)
          {
              velocity.x = 300.0f;
          }
        }

      // Quick scripting system, to be replaced with a proper scripting system
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Script>())
      {
        if (!entity.GetComponent<IsActive>()->is_active) continue; // skip non active entities

        Script* script = entity.GetComponent<Script>();
        if (script->script_id == 1)
        {
          if (entity.GetComponent<Rotation>() != nullptr)
          {
            entity.GetComponent<Rotation>()->rotation.z += 0.1f * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime();
          }
        }
        else if (script->script_id == 2)
        {
          if (entity.GetComponent<Scale>() != nullptr)
          {
            entity.GetComponent<Rotation>()->rotation.z -= 0.1f * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime();
          }
        }
        else
        {
          Log::Debug("Script ID not found: " + std::to_string(script->script_id));
        }
      }
      profiler.EndCounter("Custom Query Loops");
  
      profiler.StartCounter("Audio");
      // Audio system...
      for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Audio>())
      {
        if (!element.GetComponent<IsActive>()->is_active) continue; // skip non active entities

        ChronoDrift::Audio* audio = element.GetComponent<Audio>();
        if (audio->should_play)
        {
          if (FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(audio->audio_file) == "")
          {
            Log::Warning("Audio not attached to entity: " + 
              FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()));

            audio->should_play = false;
            continue;
          }

          if (audio->is_looping)
          {
            FMODWrapper::Core::PlayLoopingSound(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()),
                                                FLX_ASSET_GET(Asset::Sound, AssetKey{ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(element.GetComponent<Audio>()->audio_file) }));
          }
          else
          {
            FMODWrapper::Core::PlaySound(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()),
                                         FLX_ASSET_GET(Asset::Sound, AssetKey{ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(element.GetComponent<Audio>()->audio_file) }));
          }

          element.GetComponent<Audio>()->should_play = false;
        }
      }
      profiler.EndCounter("Audio");

      profiler.StartCounter("Physics");
      UpdatePhysicsSystem();
      profiler.EndCounter("Physics");

      profiler.StartCounter("Button Callbacks");
      #ifndef GAME
      // System to handle button collider callbacks
      ImGuiContext* context = GImGui;
      ImGuiWindow* hovered_window = context->HoveredWindow;
      bool is_scene = (hovered_window == ImGui::FindWindowByName("Scene"));
      #endif

      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, Button, BoundingBox2D>())
      {
          auto button = entity.GetComponent<Button>();
          if (!entity.GetComponent<IsActive>()->is_active || !entity.GetComponent<Button>()->is_interactable)
          {
              button->finalColorAdd = button->disabledColor;
              button->finalColorMul = button->disabledColor;
              continue;
          }

          #ifndef GAME
          Vector2 mtw = is_scene ? Editor::GetInstance().GetPanel("SceneView").mouse_to_world : Editor::GetInstance().GetPanel("GameView").mouse_to_world;
          #else
          Vector2 mtw;
          Vector2 mouse_pos_ss = Input::GetMousePosition();
          float app_width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
          float app_height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());
          int window_pos_x = 0, window_pos_y = 0;
          Application::GetCurrentWindow()->GetWindowPosition(&window_pos_x, &window_pos_y);
          Vector2 window_top_left = { static_cast<float>(window_pos_x), static_cast<float>(window_pos_y) };

          if (m_CamM_Instance->GetMainCamera() != INVALID_ENTITY_ID) 
          {
            auto cam_data = m_CamM_Instance->GetCameraData(m_CamM_Instance->GetMainCamera());

            // Get Mouse position relative to the viewport
            Vector2 relative_pos = Vector2(mouse_pos_ss.x - window_top_left.x,
                                         mouse_pos_ss.y - window_top_left.y);

            Vector2 ndc_click_pos = { (2 * relative_pos.x / app_width) - 1, 1 - 2 * relative_pos.y / app_height };
            Matrix4x4 inverse = (cam_data->proj_viewMatrix).Inverse();
            Vector4 clip = { ndc_click_pos.x,
                             ndc_click_pos.y,
                             1.0f,
                             1 };
            Vector4 world_pos = inverse * clip;
            world_pos.x = -world_pos.x;

            mtw = { world_pos.x, world_pos.y };
          }

          #endif

          BoundingBox2D bb = *entity.GetComponent<BoundingBox2D>();
          bool inside = (mtw.x > bb.min.x && mtw.x < bb.max.x && mtw.y > bb.min.y && mtw.y < bb.max.y);
          bool t_isClicked, t_isHovered;
          t_isClicked = t_isHovered = false;
          if (entity.HasComponent<OnHover>()) 
          {
              auto hover = entity.GetComponent<OnHover>();
              hover->on_enter = inside && !hover->is_hovering;
              hover->on_exit = !inside && hover->is_hovering;
              hover->is_hovering = inside;
              t_isHovered = hover->is_hovering;
          }
          if (entity.HasComponent<OnClick>()) 
          {
              auto click = entity.GetComponent<OnClick>();
              click->is_clicked = inside && Input::GetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
              t_isClicked = click->is_clicked;
          }

          //Update Color Mul
          button->finalColorMul = (button->normalColor == Vector3::One) ? Vector3::One :
              (t_isClicked ? button->pressedColor * button->colorMultiplier :
              (t_isHovered ? button->highlightedColor * button->colorMultiplier: button->normalColor));

          button->finalColorAdd = t_isClicked ? button->pressedColor * button->colorMultiplier : (t_isHovered ? button->highlightedColor * button->colorMultiplier : Vector3::Zero);
      }
      profiler.EndCounter("Button Callbacks");

      //TODO MOVE TO SCRIPTING ONCE DONE
      #pragma region Camera Movement -> Should be moved to scripting
      FlexECS::Entity cam_entity = m_CamM_Instance->GetMainCamera();
      if (cam_entity.Get() != INVALID_ENTITY_ID)
      {
          auto& curr_cam = cam_entity.GetComponent<Position>()->position;
          auto& curr_camt = cam_entity.GetComponent<Transform>()->is_dirty;
          if (Input::GetKey(GLFW_KEY_I))
          {
              curr_cam += Vector2(0.0f, -5.f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
              curr_camt = true;
          }
          else if (Input::GetKey(GLFW_KEY_K))
          {
              curr_cam += Vector2(0.0f, 5.f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
              curr_camt = true;
          }

          if (Input::GetKey(GLFW_KEY_J))
          {
              curr_cam += Vector2(-5.f, 0.0f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
              curr_camt = true;
          }
          else if (Input::GetKey(GLFW_KEY_L))
          {
              curr_cam += Vector2(5.f, 0.0f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
              curr_camt = true;
          }
      }
      

      // Regen Cam
      //for (auto& currCam : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, Camera>())
      //{
      //    if (!CameraManager::HasCameraEntity(currCam.Get()))
      //    {
      //        CameraManager::AddCameraEntity(currCam.Get(), currCam.GetComponent<Camera>()->camera);
      //        CameraManager::SwitchMainCamera(currCam.Get());
      //    }
      //}
      #pragma endregion
      
      //Render All Entities
      profiler.StartCounter("Graphics");
      UpdateAnimationInScene(1.0);
      UpdateAllEntitiesMatrix(m_CamM_Instance);
      RenderSprite2D();
      profiler.EndCounter("Graphics");

      profiler.ShowProfilerWindow();
    }
}
