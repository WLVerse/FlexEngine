// WLVERSE [https://wlverse.web.app]
// baselayer.cpp
// 
// Layer that handles the updates to validating cameras to update in CamManager
//
// AUTHORS
// [100%] Soh Wei Jie (weijie_soh.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor //In future will seperate this into GameCameraServiceLayer, EditorCameraServiceLayer, but for now combine
{                //CameraHandler script will be solely handling transitions, lerping and camera shake 
    void CameraSystemLayer::OnAttach()
    {
        Log::Info("CameraSystemLayer attached.");
        try
        {
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            {
                if (elem.GetComponent<Transform>()->is_active && !CameraManager::HasCamera(elem.Get()))
                {
                    CameraManager::SetCamera(elem.Get(), elem.GetComponent<Camera>());
                    Log::Info("Registered camera entity: " + std::to_string(elem.Get()));
                }
            }
        }
        catch (const std::exception& e)
        {
            Log::Error("Error in CameraSystemLayer::OnAttach: " + std::string(e.what()));
        }
    }

    void CameraSystemLayer::OnDetach()
    {
        Log::Info("CameraSystemLayer detached.");
        try
        {
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            {
                CameraManager::RemoveCamera(elem.Get());
                Log::Info("Unregistered camera entity: " + std::to_string(elem.Get()));
            }
        }
        catch (const std::exception& e)
        {
            Log::Error("Error in CameraSystemLayer::OnDetach: " + std::string(e.what()));
        }
    }

    void CameraSystemLayer::Update()
    {
        try
        {
            bool foundActiveCamera = false;
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            {
                if (elem.GetComponent<Transform>()->is_active)
                {
                    if (!CameraManager::HasCamera(elem.Get()))
                        CameraManager::SetCamera(elem.Get(), elem.GetComponent<Camera>());

                    if (CameraManager::GetMainGameCameraID() == INVALID_ENTITY_ID)
                        CameraManager::SetMainGameCameraID(elem.Get());

                    foundActiveCamera = true;
                }
                else if (CameraManager::GetMainGameCameraID() == elem.Get())
                {
                    CameraManager::SetMainGameCameraID(INVALID_ENTITY_ID);
                    Log::Info("Main game camera set to INVALID due to inactivity.");
                }
            }

            if (!foundActiveCamera)
            {
                CameraManager::SetMainGameCameraID(INVALID_ENTITY_ID);
                Log::Info("No active camera found, main game camera set to INVALID.");
            }
        }
        catch (const std::exception& e)
        {
            Log::Error("Error in CameraSystemLayer::Update: " + std::string(e.what()));
        }

        // Update Camera
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
        {
          auto entity = element.GetComponent<Camera>();
        
          if (!element.GetComponent<Transform>()->is_active ||
              !element.GetComponent<Transform>()->is_dirty ||
              !entity->getIsActive())
            continue;
        
          entity->Update();
        }
    }

}
