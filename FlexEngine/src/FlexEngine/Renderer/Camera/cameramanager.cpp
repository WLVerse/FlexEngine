/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// cameramanager.cpp
//
// The only purpose of the cameramanager right now is to maintain who the main game camera is and offer easy access by way of static functions.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#include "cameramanager.h"
#include <stdexcept>
#include "FlexEngine/FlexECS/enginecomponents.h"
#include "application.h"
#include "input.h"

namespace FlexEngine
{
    #pragma region Static Member Initialization

    FlexECS::EntityID CameraManager::m_mainGameCameraID = INVALID_ENTITY_ID;
    bool CameraManager::has_main_camera = false;
    
    #pragma endregion

    #pragma region Camera Lifetime Management

    bool CameraManager::TryMainCamera()
    {
      for (auto& cam_targets : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
      {
        m_mainGameCameraID = cam_targets;
        has_main_camera = true;

        FlexECS::Entity cam_entity = cam_targets;
        Log::Info("Setting main camera with entity name: " + FLX_STRING_GET(*cam_entity.GetComponent<EntityName>()));
        break;
      }
    }

    void CameraManager::RemoveMainCamera()
    {
      Log::Debug("Removing main camera" + std::to_string(m_mainGameCameraID));
      has_main_camera = false;
    }

    // This assumes that the main camera is valid and has been set
    Camera* CameraManager::GetMainGameCamera()
    {
      FlexECS::Entity camera_entity = m_mainGameCameraID;

      return camera_entity.GetComponent<Camera>();
    }

    FlexECS::EntityID CameraManager::GetMainGameCameraID() { return m_mainGameCameraID; }

    void CameraManager::SetMainGameCameraID(FlexECS::EntityID id)
    {
        Log::Debug("Setting main game camera with Entity ID: " + std::to_string(id));
        m_mainGameCameraID = id;
        Log::Debug("Main Game camera set for Entity ID: " + std::to_string(id));
    }

    const Vector4& CameraManager::GetMouseToWorld()
    {
      Vector4 clip = {
        (2.0f * Input::GetMousePosition().x) / Application::GetCurrentWindow()->GetWidth() - 1.0f,
        1.0f - (2.0f * Input::GetMousePosition().y) / Application::GetCurrentWindow()->GetHeight(),
        1.0f,
        1.0f
      };

      return GetMainGameCamera()->GetProjViewMatrix().Inverse() * clip;
    }

}
