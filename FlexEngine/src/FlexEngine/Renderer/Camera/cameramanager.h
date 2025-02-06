/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * cameraManager.h
 *
 * This file implements the CameraManager class, which provides a centralized
 * management system for all Camera objects used in the engine. The CameraManager
 * maintains a mapping between entity IDs and their associated Camera pointers,
 * allowing for quick access and management of both editor and game cameras.
 *
 * Key functionalities include:
 * - Initializing the CameraManager with default editor cameras.
 * - Adding, updating, and removing cameras by entity ID.
 * - Retrieving cameras for both the editor and main game views.
 * - Clearing all registered cameras and deregistering ECS cameras.
 *
 * AUTHORS
 * [100%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Main Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/

#pragma once

#include "camera.h"
#include "FlexECS/datastructures.h"
#include <unordered_map>
#include <algorithm>
#include <limits>

namespace FlexEngine
{
    /// Constant representing an invalid entity ID.
    constexpr uint64_t INVALID_ENTITY_ID = std::numeric_limits<uint64_t>::max();

    class __FLX_API CameraManager
    {
        // Storage for cameras, using raw pointers to manage lifetime.
        static std::unordered_map<FlexECS::EntityID, Camera*> m_cameraEntities;

        // Reserved editor camera ID.
        static FlexECS::EntityID m_editorCameraID;
        // Reserved main game camera ID.
        static FlexECS::EntityID m_mainGameCameraID;

    public:
        /*!
         * \brief Initializes the CameraManager.
         *
         * Sets up the default editor camera and prepares the CameraManager for use.
         */
        static void Initialize();

        /*!************************************************************************
         * \brief Adds or updates a camera associated with the given entity ID.
         *
         * Associates the provided Camera pointer with the specified entity ID. If a
         * camera already exists for the given entity, it is updated.
         *
         * \param entityID The ID of the entity to associate with the camera.
         * \param cam The camera pointer to associate with the entity.
         *************************************************************************/
        static void SetCamera(FlexECS::EntityID entityID, Camera* const& cam);

        /*!************************************************************************
         * \brief Removes the camera associated with the given entity ID.
         *
         * Detaches and removes the Camera pointer associated with the specified entity ID.
         *
         * \param entityID The ID of the entity whose camera should be removed.
         *************************************************************************/
        static void RemoveCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Retrieves the camera associated with the given entity ID.
         *
         * \param entityID The ID of the entity whose camera is to be retrieved.
         * \return A pointer to the Camera if found, or nullptr if not.
         *************************************************************************/
        static Camera* GetCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Checks if a camera is registered with the given entity ID.
         *
         * \param entityID The ID of the entity to check.
         * \return True if a camera exists for the entity, false otherwise.
         *************************************************************************/
        static bool HasCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Clears all registered cameras.
         *
         * Removes all Camera pointers from the manager and clears the internal storage.
         *************************************************************************/
        static void Clear();

        /*!************************************************************************
         * \brief Deregisters all ECS registered cameras.
         *
         * Removes all cameras that have been registered by the ECS system.
         *************************************************************************/
        static void DeregisterECSCams();

        /*!************************************************************************
         * \brief Gets the default editor camera.
         *
         * \return A pointer to the default editor camera.
         *************************************************************************/
        static Camera* GetEditorCamera();

        /*!
         * \brief Retrieves the reserved editor camera ID.
         *
         * \return The entity ID reserved for the editor camera.
         */
        static FlexECS::EntityID GetEditorCameraID() { return m_editorCameraID; };

        /*!************************************************************************
         * \brief Gets the main game camera.
         *
         * \return A pointer to the main game camera if available, otherwise nullptr.
         *************************************************************************/
        static Camera* GetMainGameCamera();

        /*!
         * \brief Retrieves the reserved main game camera ID.
         *
         * \return The entity ID reserved for the main game camera.
         */
        static FlexECS::EntityID GetMainGameCameraID();

        /*!************************************************************************
         * \brief Sets the main game camera ID.
         *
         * Assigns the provided entity ID as the reserved main game camera ID.
         *
         * \param id The entity ID to set as the main game camera.
         *************************************************************************/
        static void SetMainGameCameraID(FlexECS::EntityID id);

        /*!
         * \brief Sets the editor camera ID.
         *
         * Assigns the provided entity ID as the reserved editor camera ID.
         *
         * \param id The entity ID to set as the editor camera.
         */
        static void SetEditorCameraID(FlexECS::EntityID id);
    };
}
