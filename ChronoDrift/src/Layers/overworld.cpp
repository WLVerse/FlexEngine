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
        CameraManager::AddCameraEntity(camera.Get(), camera.GetComponent<Camera>()->camera);
        CameraManager::SwitchMainCamera(camera.Get());

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
      profiler.EndCounter("Button Callbacks");

      #pragma region Camera Movement -> Should be moved to scripting

      FlexECS::Entity cam_entity = CameraManager::GetMainCamera();
      auto& curr_cam = cam_entity.GetComponent<Position>()->position;
      auto& curr_camt = cam_entity.GetComponent<Transform>()->is_dirty;
      if (Input::GetKey(GLFW_KEY_UP))
      {
          curr_cam += Vector2(0.0f, -5.f)* (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
          curr_camt = true;
      }
      else if (Input::GetKey(GLFW_KEY_DOWN))
      {
          curr_cam += Vector2(0.0f, 5.f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
          curr_camt = true;
      }

      if (Input::GetKey(GLFW_KEY_RIGHT))
      {
          curr_cam += Vector2(5.f, 0.0f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
          curr_camt = true;
      }
      else if (Input::GetKey(GLFW_KEY_LEFT))
      {
          curr_cam += Vector2(-5.f, 0.0f) * (30 * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime());
          curr_camt = true;
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
      
      // Update transformation matrix for all entities, whilst rendering is handled by the main window loop.
      profiler.StartCounter("Graphics");
      UpdateAllEntitiesMatrix();
      profiler.EndCounter("Graphics");

      profiler.ShowProfilerWindow();
    }
}
