/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * cameraManager.cpp
 *
 * This file implements the CameraManager class, which provides a centralized
 * management system for all Camera objects used in the engine. The CameraManager
 * maintains a mapping between entity IDs and their associated Camera pointers,
 * allowing for quick access and management of both editor and game cameras.
 *
 * Key functionalities include:
 * - Initializing the CameraManager with default editor camera.
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
#include "cameramanager.h"
#include <stdexcept>

namespace FlexEngine
{
    #pragma region Static Member Initialization

    // Initialize static member variables.
    std::unordered_map<FlexECS::EntityID, Camera*> CameraManager::m_cameraEntities;
    FlexECS::EntityID CameraManager::m_editorCameraID = INVALID_ENTITY_ID;
    FlexECS::EntityID CameraManager::m_mainGameCameraID = INVALID_ENTITY_ID;

    #pragma endregion

    #pragma region Camera Lifetime Management

    /*!
     * \brief Initializes the CameraManager.
     *
     * This function sets up the CameraManager by creating and registering the
     * default editor camera. It assigns a unique ID (0) to the editor camera and
     * stores it in the camera mapping.
     */
    void CameraManager::Initialize()
    {
        try
        {
            Log::Debug("Initializing CameraManager...");
            // Create the default editor camera manually
            Camera* editorCamera = new Camera({ 800.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);

            m_editorCameraID = 0; // Assign a unique ID for the editor camera
            Log::Debug("Editor camera created with ID: " + std::to_string(m_editorCameraID));

            m_cameraEntities[m_editorCameraID] = editorCamera;
            Log::Debug("Editor camera registered in CameraManager.");
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::Initialize: " + std::string(e.what()));
        }
    }

    /*!
     * \brief Adds or updates a camera associated with the given entity ID.
     *
     * Associates the provided Camera pointer with the specified entity ID. If a
     * camera already exists for the given entity, it is updated.
     *
     * \param entityID The ID of the entity to associate with the camera.
     * \param cam The Camera pointer to associate with the entity.
     */
    void CameraManager::SetCamera(FlexECS::EntityID entityID, Camera* const& cam)
    {
        try
        {
            Log::Debug("Setting camera with Entity ID: " + std::to_string(entityID));
            if (!cam)
            {
                throw std::invalid_argument("Camera pointer is null.");
            }
            m_cameraEntities[entityID] = cam;
            Log::Debug("Camera set for Entity ID: " + std::to_string(entityID));
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::SetCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
        }
    }

    /*!
     * \brief Removes the camera associated with the given entity ID.
     *
     * Detaches and removes the Camera pointer associated with the specified entity ID.
     *
     * \param entityID The ID of the entity whose camera should be removed.
     */
    void CameraManager::RemoveCamera(FlexECS::EntityID entityID)
    {
        try
        {
            Log::Debug("Removing camera with Entity ID: " + std::to_string(entityID));
            auto it = m_cameraEntities.find(entityID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Camera with EntityID " + std::to_string(entityID) + " not found.");
            }

            m_cameraEntities.erase(entityID);
            Log::Debug("Camera removed for Entity ID: " + std::to_string(entityID));
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::RemoveCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
        }
    }

    /*!
     * \brief Retrieves the camera associated with the given entity ID.
     *
     * \param entityID The ID of the entity whose camera is to be retrieved.
     * \return A pointer to the Camera if found, or nullptr otherwise.
     */
    Camera* CameraManager::GetCamera(FlexECS::EntityID entityID)
    {
        try
        {
            // Log::Debug("Getting camera with Entity ID: " + std::to_string(entityID));
            auto it = m_cameraEntities.find(entityID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Camera with EntityID " + std::to_string(entityID) + " not found.");
            }

            // Log::Debug("Camera found for Entity ID: " + std::to_string(entityID));
            return it->second;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::GetCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
            return nullptr;  // Return nullptr if the camera was not found
        }
    }

    /*!
     * \brief Checks if a camera is registered with the given entity ID.
     *
     * \param entityID The ID of the entity to check.
     * \return True if a camera exists for the entity, false otherwise.
     */
    bool CameraManager::HasCamera(FlexECS::EntityID entityID)
    {
        try
        {
            bool exists = m_cameraEntities.find(entityID) != m_cameraEntities.end();
            return exists;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::HasCamera (EntityID: " + std::to_string(entityID) + "): " + std::string(e.what()));
            return false;
        }
    }

    /*!
     * \brief Clears all registered cameras.
     *
     * Removes all Camera pointers from the manager and clears the internal storage.
     * This function manually deletes the editor camera (entity ID 0) and resets
     * the editor camera ID to an invalid value.
     */
    void CameraManager::Clear()
    {
        try
        {
            Log::Debug("Clearing all cameras from CameraManager...");
            // Manager now only handles clearing out references to these cameras.
            // Manually delete the editor camera (if present) before clearing.
            delete m_cameraEntities.find(0)->second;
            m_cameraEntities.clear();
            m_editorCameraID = INVALID_ENTITY_ID;
            Log::Debug("All cameras cleared.");
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::Clear: " + std::string(e.what()));
        }
    }

    /*!
     * \brief Deregisters all ECS registered cameras.
     *
     * Removes all cameras that have been registered by the ECS system, except for
     * the default editor camera (entity ID 0).
     */
    void CameraManager::DeregisterECSCams()
    {
        for (auto it = m_cameraEntities.begin(); it != m_cameraEntities.end();)
        {
            if (it->first != 0) // Remove cameras that are NOT entityID = 0
                it = m_cameraEntities.erase(it); // Erase and update iterator
            else
                ++it;
        }
    }

    /*!
     * \brief Gets the default editor camera.
     *
     * \return A pointer to the default editor camera. If not found, returns nullptr.
     */
    Camera* CameraManager::GetEditorCamera()
    {
        try
        {
            auto it = m_cameraEntities.find(m_editorCameraID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Editor camera not found.");
            }
            return it->second;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::GetEditorCamera: " + std::string(e.what()));
            return nullptr;  // Return nullptr if the editor camera is not found
        }
    }

    /*!
     * \brief Gets the main game camera.
     *
     * \return A pointer to the main game camera if available, otherwise nullptr.
     */
    Camera* CameraManager::GetMainGameCamera()
    {
        try
        {
            // Log::Debug("Getting main game camera...");
            auto it = m_cameraEntities.find(m_mainGameCameraID);
            if (it == m_cameraEntities.end())
            {
                throw std::out_of_range("Main Game camera not found.");
            }
            // Log::Debug("Main Game camera found.");
            return it->second;
        }
        catch (const std::exception& e)
        {
            Log::Debug("Exception in CameraManager::GetMainGameCamera: " + std::string(e.what()));
            return nullptr;  // Return nullptr if the main game camera is not found
        }
    }

    /*!
     * \brief Retrieves the reserved main game camera ID.
     *
     * \return The entity ID reserved for the main game camera.
     */
    FlexECS::EntityID CameraManager::GetMainGameCameraID()
    {
        return m_mainGameCameraID;
    }

    /*!
     * \brief Sets the main game camera ID.
     *
     * Assigns the provided entity ID as the reserved main game camera ID.
     *
     * \param id The entity ID to set as the main game camera.
     */
    void CameraManager::SetMainGameCameraID(FlexECS::EntityID id)
    {
        Log::Debug("Setting main game camera with Entity ID: " + std::to_string(id));
        m_mainGameCameraID = id;
        Log::Debug("Main Game camera set for Entity ID: " + std::to_string(id));
    }

    /*!
     * \brief Sets the editor camera ID.
     *
     * Assigns the provided entity ID as the reserved editor camera ID.
     *
     * \param id The entity ID to set as the editor camera.
     */
    void CameraManager::SetEditorCameraID(FlexECS::EntityID id)
    {
        Log::Debug("Setting editor camera with Entity ID: " + std::to_string(id));
        m_editorCameraID = id;
        Log::Debug("Editor camera set for Entity ID: " + std::to_string(id));
    }
    #pragma endregion
}
