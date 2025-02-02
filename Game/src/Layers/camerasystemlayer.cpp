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

namespace Game //In future will seperate this into GameCameraServiceLayer, EditorCameraServiceLayer, but for now combine
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
            bool mainGameCamStillExists = false;
            FlexECS::EntityID mainGameCamID = CameraManager::GetMainGameCameraID();
            FlexECS::EntityID newMainCamID = INVALID_ENTITY_ID;

            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            {
                if (elem.GetComponent<Transform>()->is_active)
                {
                    if (!CameraManager::HasCamera(elem.Get()))
                        CameraManager::SetCamera(elem.Get(), elem.GetComponent<Camera>());

                    if (newMainCamID == INVALID_ENTITY_ID)
                        newMainCamID = elem.Get();

                    if (mainGameCamID == elem.Get())
                        mainGameCamStillExists = true;

                    foundActiveCamera = true;
                }
            }

            if (!mainGameCamStillExists && CameraManager::GetMainGameCameraID() != INVALID_ENTITY_ID)
            {
                CameraManager::SetMainGameCameraID(INVALID_ENTITY_ID);
                Log::Info("Main game camera set to INVALID as it was deleted.");
            }

            if (CameraManager::GetMainGameCameraID() == INVALID_ENTITY_ID && newMainCamID != INVALID_ENTITY_ID)
            {
                CameraManager::SetMainGameCameraID(newMainCamID);
                Log::Info("New main game camera set: " + std::to_string(newMainCamID));
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
