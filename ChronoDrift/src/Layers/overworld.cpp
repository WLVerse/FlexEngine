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
        camera.AddComponent<Position>({ {-150, 300 } });
        camera.AddComponent<Scale>({ { 0.5,0.5 } });
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
        RegisterRenderingComponents();
        RegisterPhysicsComponents();

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
      profiler.EndCounter("Custom Query Loops");
  
      profiler.StartCounter("Audio");
      // Audio system...
      for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Audio>())
      {
        if (!element.GetComponent<IsActive>()->is_active) continue; // skip non active entities

        if (element.GetComponent<Audio>()->should_play)
        {
          if (element.GetComponent<Audio>()->is_looping)
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
      // System to handle button collider callbacks
      // TODO @Wei jie
      for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Button, Sprite>())
      {
        if (!element.GetComponent<IsActive>()->is_active) continue;

        Vector2 mtw = Editor::GetInstance().GetPanel("GameView").mouse_to_world;
        BoundingBox2D bb = *element.GetComponent<BoundingBox2D>();
        if (mtw.x > bb.min.x && mtw.x < bb.max.x && mtw.y > bb.min.y && mtw.y < bb.max.y)
        {
          element.GetComponent<Sprite>()->color_to_add.x = 255;
        }
        else
        {
          element.GetComponent<Sprite>()->color_to_add.x = 0;
        }
      }

      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, Button, BoundingBox2D>())
      {
          if (!entity.GetComponent<IsActive>()->is_active || !entity.GetComponent<Button>()->is_interactable) continue;

          Vector2 mtw = Editor::GetInstance().GetPanel("GameView").mouse_to_world;
          BoundingBox2D bb = *entity.GetComponent<BoundingBox2D>();
          //is there a function for me to call to check rather than this ugly condition
          if (mtw.x > bb.min.x && mtw.x < bb.max.x && mtw.y > bb.min.y && mtw.y < bb.max.y)
          {
              //AABB check pass -> should change to raycast
              if (entity.HasComponent<OnHover>())
              {
                  auto hover = entity.GetComponent<OnHover>();
                  hover->on_enter = !hover->is_hovering;
                  hover->is_hovering = true;
                  hover->on_exit = false;
              }
              if (entity.HasComponent<OnClick>())
              {
                  auto click = entity.GetComponent<OnClick>();
                  if (Input::GetKey(GLFW_MOUSE_BUTTON_LEFT))
                  {
                      click->is_clicked = true;
                  }
                  else
                  {
                      click->is_clicked = false;
                  }
              }
          }
          else
          {
              //AABB check fail
              if (entity.HasComponent<OnHover>())
              {
                  auto hover = entity.GetComponent<OnHover>();
                  hover->on_exit = !hover->is_hovering;
                  hover->is_hovering = false;
                  hover->on_enter = true;
              }
          }
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
              curr_cam += Vector2(5.f, 0.0f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
              curr_camt = true;
          }
          else if (Input::GetKey(GLFW_KEY_L))
          {
              curr_cam += Vector2(-5.f, 0.0f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
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
