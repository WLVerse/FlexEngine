/*!************************************************************************
 * CAMERA SYSTEM LAYER [https://wlverse.web.app]
 * camerasystemlayer.cpp
 *
 * This file defines the CameraSystemLayer class, which is responsible for
 * updating and validating camera entities in the CamManager. The layer ensures
 * that camera states are properly updated each frame, thereby maintaining
 * synchronization and consistency within the game engine's camera management system.
 *
 * Key functionalities include:
 * - Updating camera entities on each frame.
 * - Validating camera states and relaying updates to the CamManager.
 *
 * The design emphasizes modularity and performance, facilitating seamless
 * integration with the engine's layer system and flexible camera management.
 *
 * AUTHORS
 * [100%] Soh Wei Jie (weijie_soh.c@digipen.edu)
 *   - Main Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/
#include "Layers.h"

namespace Game
{
    /*!
    * \brief Attaches the CameraSystemLayer to the engine.
    *
    * During attachment, the function iterates through all camera entities in the
    * active scene. For each active camera that is not yet registered in the
    * CameraManager, it registers the camera and logs the registration.
    *
    * \note Exceptions during registration are caught and logged.
    */
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

    /*!
    * \brief Detaches the CameraSystemLayer from the engine.
    *
    * During detachment, the function iterates through all camera entities in the
    * active scene and unregisters them from the CameraManager. Each unregistration
    * is logged for debugging purposes.
    *
    * \note Exceptions during unregistration are caught and logged.
    */
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

    /*!
    * \brief Updates the camera system each frame.
    *
    * This function performs the following tasks:
    * - Iterates over all camera entities in the active scene.
    * - Registers any active cameras not already present in the CameraManager.
    * - Validates the main game camera; if it no longer exists, resets it.
    * - Selects a new main camera if needed.
    * - Finally, updates each camera's state if the camera's transform is active,
    *   dirty, and the camera itself is active.
    *
    * \note Exceptions during the update process are caught and logged.
    */
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

        // Update each camera that requires an update.
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
        {
            auto cameraComponent = element.GetComponent<Camera>();

            if (!element.GetComponent<Transform>()->is_active ||
                !element.GetComponent<Transform>()->is_dirty ||
                !cameraComponent->getIsActive())
            {
                continue;
            }

            cameraComponent->Update();
        }
    }
} // namespace Game