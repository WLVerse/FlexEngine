/*!************************************************************************
 // WLVERSE [https://wlverse.web.app]
 // scenecamsorter.cpp
 //
 // This file defines the `SceneCamSorter` class, a singleton utility
 // within the FlexEngine framework for managing camera entities in a 3D scene.
 // The `SceneCamSorter` class provides functionalities for registering and
 // tracking active camera entities, facilitating seamless transitions between
 // main and editor cameras. It also offers methods to retrieve and update
 // `CameraData` properties, which encapsulate essential camera attributes
 // such as position, orientation, and projection matrices.
 //
 // Key functionalities include:
 // - Adding, switching, and removing camera entities in the scene.
 // - Providing fast access to `CameraData`, including view and projection
 //   matrices, for any registered camera.
 // - Ensuring only one instance manages camera sorting, adhering to the
 //   singleton design pattern.
 //
 // The `CameraData` struct, also defined here, includes properties for world
 // position, orientation vectors, and other parameters essential for view
 // transformations in both perspective and orthographic projections.
 //
 // AUTHORS
 // [100%] Soh Wei Jie (weijie.soh@digipen.edu)
 //   - Main Author
 //   - Developed the camera management interface, singleton pattern, and
 //     entity registration features within FlexEngine.
 //
 // Copyright (c) 2024 DigiPen, All rights reserved.
 **************************************************************************/

#include "cameramanager.h"

namespace FlexEngine
{
	#pragma region Reflection
    FLX_REFL_REGISTER_START(CameraData)
        FLX_REFL_REGISTER_PROPERTY(position)
        FLX_REFL_REGISTER_PROPERTY(target)
        FLX_REFL_REGISTER_PROPERTY(up)
        FLX_REFL_REGISTER_PROPERTY(right)
        FLX_REFL_REGISTER_PROPERTY(viewMatrix)
        FLX_REFL_REGISTER_PROPERTY(projMatrix)
        FLX_REFL_REGISTER_PROPERTY(fieldOfView)
        FLX_REFL_REGISTER_PROPERTY(aspectRatio)
        FLX_REFL_REGISTER_PROPERTY(nearClip)
        FLX_REFL_REGISTER_PROPERTY(farClip)
        FLX_REFL_REGISTER_PROPERTY(m_isOrthographic)
        FLX_REFL_REGISTER_PROPERTY(cam_is_active)
        FLX_REFL_REGISTER_PROPERTY(m_OrthoWidth)
        FLX_REFL_REGISTER_PROPERTY(m_OrthoHeight)
    FLX_REFL_REGISTER_END;
	#pragma endregion

    #pragma region Static Member initialization - Legacy
    
    //std::unordered_map<FlexECS::EntityID, CameraData> CameraManager::m_cameraEntities;
    //FlexECS::EntityID CameraManager::m_currMainID = static_cast<uint64_t>(-1);
    //FlexECS::EntityID CameraManager::m_currEditorID = static_cast<uint64_t>(-1);

    #pragma endregion

    #pragma region Private
    CameraManager::CameraManager(bool autoCreateEditor)
        : m_autoCreateEditorCamera(autoCreateEditor), m_currEditorID(0)
    {
        if (m_autoCreateEditorCamera)
        {
            CreateDefaultEditorCamera();
            Log::Debug("CameraManager initialized with editor camera.");
        }
        else
            Log::Debug("CameraManager initialized without editor camera.");
    }

    CameraManager::~CameraManager()
    {
        RemoveCameraEntities();
        Log::Debug("CameraManager has been successfully destroyed.");
    }

    /*!***************************************************************************
     * \brief
     * Creates a default editor camera and registers it with the CameraManager.
     *
     * \details
     * The editor camera is assigned a unique entity ID (typically 0) and is
     * initialized with default CameraData. This camera serves as the default
     * viewport for editing purposes and is automatically set as the current
     * editor camera.
     *****************************************************************************/
    void CameraManager::CreateDefaultEditorCamera()
    {
        // Generate a unique entity ID for the editor camera
        FlexECS::EntityID editorCameraID = 0;

        // Define default camera data for the editor -> Will be overwritten in sceneView
        CameraData editorCamera;

        // Add the editor camera to the manager
        AddCameraEntity(editorCameraID, editorCamera);

        // Set the new camera as the current editor camera
        m_currEditorID = editorCameraID;
    }

    /*!***************************************************************************
    * \brief
    * Removes all camera entities from the manager.
    *****************************************************************************/
    void CameraManager::RemoveCameraEntities()
    {
        Log::Debug("RemoveAllCameras(...) => Removing all cameras.");
        m_cameraEntities.clear();
        m_currMainID = static_cast<uint64_t>(-1);
        m_currEditorID = static_cast<uint64_t>(-1);
    }
    #pragma endregion

    #pragma region Camera Management

    /*!***************************************************************************
     * \brief
     * Adds an entity with CameraData to the camera manager.
     *
     * \param entityID The ID of the entity to add.
     * \param cameraData The CameraData associated with the entity.
     * \return True if the camera entity was added successfully, false if it already exists.
     *****************************************************************************/
    void CameraManager::AddCameraEntity(FlexECS::EntityID entityID, const CameraData& cameraData)
    {
        //Check validity of ID
        if (entityID == INVALID_ENTITY_ID)
        {
            Log::Error("AddCameraEntity(...) => Invalid entityID provided.");
            return;
        }

        //Check if editor cam already exists in the map
        if (entityID == 0)
        {
            if (m_cameraEntities.find(entityID) != m_cameraEntities.end())
            {
                Log::Warning("AddCameraEntity(...) => entityID = 0 for cameras is used by editor and cannot be overwritten.");
            }
            else
            {
                m_cameraEntities.emplace(entityID, cameraData);
                Log::Debug("AddCameraEntity(...) => Added editor camera.");
            }
            SwitchEditorCamera(entityID);
            return;
        }

        //Add Camera to map
        auto result = m_cameraEntities.emplace(entityID, cameraData);
        if (result.second)
        {
            Log::Debug("AddCameraEntity(...) => Added camera with entityID " + std::to_string(entityID));
        }
        else
        {
            Log::Warning("AddCameraEntity(...) => Existing camera with same id. Replacing camera " + std::to_string(entityID));
            m_cameraEntities[entityID] = cameraData; // Replace existing entry
        }

        //Update Main Camera automatically if missing
        if (m_currMainID == INVALID_ENTITY_ID && cameraData.cam_is_active)
            SwitchMainCamera(entityID);
    }

    /*!***************************************************************************
     * \brief
     * Changes the main camera to the specified entity ID, if it exists.
     *
     * \param entityID The ID of the entity to set as the main camera.
     * \return True if the main camera was successfully switched, false otherwise.
     *****************************************************************************/
    bool CameraManager::SwitchMainCamera(FlexECS::EntityID entityID)
    {
        if (entityID == INVALID_ENTITY_ID || m_cameraEntities.empty())
        {
            Log::Error("SwitchMainCamera(...) => Invalid operation on empty camera list or invalid entityID.");
            return false;
        }

        if (m_cameraEntities.find(entityID) != m_cameraEntities.end())
        {
            m_currMainID = entityID;
            Log::Debug("SwitchMainCamera(...) => Switched Main to camera with entityID " + std::to_string(entityID));
            return true;
        }
        Log::Warning("SwitchMainCamera(...) => Unable to find camera with your entityID. You forgot to add it.");
        return false;
    }

    /*!***************************************************************************
     * \brief
     * Changes the editor camera to the specified entity ID, if it exists.
     *
     * \param entityID The ID of the entity to set as the editor camera.
     * \return True if the editor camera was successfully switched, false otherwise.
     *****************************************************************************/
    bool CameraManager::SwitchEditorCamera(FlexECS::EntityID entityID)
    {
        if (entityID == INVALID_ENTITY_ID || m_cameraEntities.empty())
        {
            Log::Error("SwitchEditorCamera(...) => Invalid operation on empty camera list or invalid entityID.");
            return false;
        }

        if (m_cameraEntities.find(entityID) != m_cameraEntities.end())
        {
            m_currEditorID = entityID;
            Log::Debug("SwitchEditorCamera(...) => Switched Editor to camera with entityID " + std::to_string(entityID));
            return true;
        }
        Log::Warning("SwitchEditorCamera(...) => Unable to find camera with your entityID. You forgot to add it.");
        return false;
    }

    /*!***************************************************************************
    * \brief
    * Removes a specific camera entity by its ID.
    *
    * \param entityID The ID of the entity to remove.
    * \return True if the camera was removed, false if the entity ID was not found.
    *****************************************************************************/
    bool CameraManager::RemoveCameraEntity(FlexECS::EntityID entityID)
    {
        // Check if the camera entity exists in the map
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end())
        {
            // Erase the camera from the map
            m_cameraEntities.erase(it);
            Log::Debug("RemoveCameraEntity(...) => Removed camera with entityID " + std::to_string(entityID));

            // If the removed entity was the main or editor camera, reset those to invalid values
            if (entityID == m_currMainID)
            {
                m_currMainID = INVALID_ENTITY_ID; // Reset to invalid ID
            }
            if (entityID == m_currEditorID)
            {
                m_currEditorID = INVALID_ENTITY_ID; // Reset to invalid ID
            }
            return true;
        }

        // If the camera entity wasn't found
        Log::Warning("RemoveCameraEntity(...) => Entity ID not found. Unable to remove camera with entityID " + std::to_string(entityID));
        return false;
    }

    /*!***************************************************************************
    * \brief
    * Removes all cameras except the editor camera.
    *****************************************************************************/
    void CameraManager::RemoveCamerasInScene()
    {
        Log::Info("RemoveNonEditorCameras(...) => Removing " + std::to_string(m_cameraEntities.size()-1) + " camera(s) in scene.");
        for (auto it = m_cameraEntities.begin(); it != m_cameraEntities.end();)
        {
            if (it->first != m_currEditorID)
            {
                Log::Debug("L> Removing camera with entityID " + std::to_string(it->first));
                it = m_cameraEntities.erase(it);
            }
            else
            {
                ++it;
            }
        }

        m_currMainID = m_currMainID == m_currEditorID ? m_currMainID : static_cast<uint64_t>(-1);
    }

    /*!***************************************************************************
     * \brief
     * Updates the CameraData for a given entity ID.
     *
     * \param entityID The ID of the entity to update.
     * \param curr The new CameraData to update.
     *****************************************************************************/
    void CameraManager::UpdateData(FlexECS::EntityID entityID, const CameraData& curr)
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end())
        {
            it->second = curr;
        }
        else
        {
            Log::Warning("UpdateData(...) => Camera with entityID not found.");
        }
    }
    
    /*!***************************************************************************
    * \brief
    * Activates the specified camera entity by enabling its `cam_is_active` flag.
    *
    * \param entityID The ID of the entity to enable.
    * \return True if the camera was successfully enabled, false otherwise.
    *****************************************************************************/
    bool CameraManager::EnableCameraEntity(FlexECS::EntityID entityID) 
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end()) 
        {
            Log::Debug("EnableCameraEntity(...) => Enabling camera with entityID " + std::to_string(entityID));
            it->second.cam_is_active = true;
            return true;
        }
        Log::Debug("EnableCameraEntity(...) => Failed to enable camera with entityID " + std::to_string(entityID) + ". EntityID not found.");
        return false;
    }
   
    /*!***************************************************************************
    * \brief
    * Deactivates the specified camera entity by disabling its `cam_is_active` flag
    * and triggers revalidation of the main camera.
    *
    * \param entityID The ID of the entity to disable.
    * \return True if the camera was successfully disabled, false otherwise.
    *****************************************************************************/
    bool CameraManager::DisableCameraEntity(FlexECS::EntityID entityID) 
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end()) 
        {
            Log::Debug("DisableCameraEntity(...) => Disabling camera with entityID " + std::to_string(entityID));
            it->second.cam_is_active = false;
            ValidateMainCamera(); // Revalidate the main camera
            return true;
        }
        Log::Debug("DisableCameraEntity(...) => Failed to disable camera. EntityID " + std::to_string(entityID) + " not found.");
        return false;
    }
    
    /*!***************************************************************************
    * \brief
    * Ensures the main camera is valid and active. If the current main camera
    * becomes invalid, attempts to find and assign a new active camera as the main.
    *****************************************************************************/
    void CameraManager::ValidateMainCamera() 
    {
        // If the current main camera is invalid or inactive
        if (m_currMainID == INVALID_ENTITY_ID ||
            m_cameraEntities.find(m_currMainID) == m_cameraEntities.end() ||
            !m_cameraEntities[m_currMainID].cam_is_active)
        {
            // Try to find a new active main camera
            for (const auto& [id, data] : m_cameraEntities) 
            {
                if (id == m_currEditorID)
                    continue;

                if (data.cam_is_active)
                {
                    m_currMainID = id; // Assign the first active camera as the main camera
                    return;
                }
            }
            // If no active cameras are found, set the main camera to invalid
            m_currMainID = INVALID_ENTITY_ID;
        }
    }
    #pragma endregion

    #pragma region Get functions

    /*!***************************************************************************
    * \brief
    * Retrieves the main camera entity ID.
    *
    * \return The entity ID of the main camera.
    *****************************************************************************/
    FlexECS::EntityID CameraManager::GetMainCamera() const
    {
        if (m_currMainID == INVALID_ENTITY_ID)
        {
            Log::Warning("GetMainCamera() => Main Camera is of an invalid ID.");
            return INVALID_ENTITY_ID;
        }
        return m_currMainID;
    }

    /*!***************************************************************************
    * \brief
    * Retrieves the editor camera entity ID.
    *
    * \return The entity ID of the editor camera.
    *****************************************************************************/
    FlexECS::EntityID CameraManager::GetEditorCamera() const
    {
        if (m_currEditorID == INVALID_ENTITY_ID)
        {
            Log::Warning("GetEditorCamera() => Editor Camera is of an invalid ID.");
            return INVALID_ENTITY_ID;
        }
        return m_currEditorID;
    }

    /*!***************************************************************************
    * \brief
    * Retrieves the CameraData for a specific entity ID.
    *
    * \param entityID The ID of the entity to retrieve CameraData for.
    * \return Pointer to the CameraData if found, nullptr otherwise.
    *****************************************************************************/
    const CameraData* CameraManager::GetCameraData(FlexECS::EntityID entityID) const
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end())
        {
            return &it->second;
        }
        Log::Warning("GetCameraData(...) => Camera with Entity ID not found.");
        return nullptr;
    }

    /*!***************************************************************************
    * \brief
    * Checks if an entity has been added as a camera.
    *
    * \param entityID The ID of the entity to check.
    * \return True if the entity is a camera, false otherwise.
    *****************************************************************************/
    bool CameraManager::HasCameraEntity(FlexECS::EntityID entityID) const
    {
        return m_cameraEntities.find(entityID) != m_cameraEntities.end();
    }

    #pragma endregion
}
