/*!************************************************************************
 * CAMERA SYSTEM LAYER [https://wlverse.web.app]
 * camerasystemlayer.cpp - GAME
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

namespace Editor  // Future separation: GameCameraServiceLayer, EditorCameraServiceLayer.
{
    /*!
     * \brief Attaches the CameraSystemLayer to the engine.
     *
     * This function registers all active camera entities from the current scene
     * with the CameraManager. For each camera entity with an active transform that
     * is not yet registered, it sets the camera and logs the registration.
     *
     * \note Any exceptions during the registration process are caught and logged.
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
     * This function unregisters all camera entities from the current scene from the
     * CameraManager and logs the unregistration. It ensures that cameras are properly
     * removed when the layer is detached.
     *
     * \note Any exceptions during the unregistration process are caught and logged.
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
     * \brief Updates camera states and validates camera entities each frame.
     *
     * This function performs the following steps:
     * 1. Iterates through all camera entities in the current scene to ensure that:
     *    - Active cameras are registered with the CameraManager.
     *    - A valid main game camera is maintained.
     * 2. If the main game camera is deleted, it resets the main camera ID.
     * 3. For each active and dirty camera entity, updates its position and transformation matrix.
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

            // Validate and register active camera entities.
            for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            {
                if (elem.GetComponent<Transform>()->is_active)
                {
                    if (!CameraManager::HasCamera(elem.Get()))
                        CameraManager::SetCamera(elem.Get(), elem.GetComponent<Camera>());

                    // Set candidate for main game camera.
                    if (newMainCamID == INVALID_ENTITY_ID)
                        newMainCamID = elem.Get();

                    // Check if the main game camera still exists.
                    if (mainGameCamID == elem.Get())
                        mainGameCamStillExists = true;

                    foundActiveCamera = true;
                }
            }

            // If the current main game camera was deleted, reset it.
            if (!mainGameCamStillExists && CameraManager::GetMainGameCameraID() != INVALID_ENTITY_ID)
            {
                CameraManager::SetMainGameCameraID(INVALID_ENTITY_ID);
                Log::Info("Main game camera set to INVALID as it was deleted.");
            }

            // If no main game camera is set, use the first candidate.
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

        // Update camera components for all active and dirty camera entities.
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
} // namespace Editor
