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
    FLX_REFL_REGISTER_END;
	#pragma endregion

    #pragma region Static Member initialization

    std::unordered_map<FlexECS::EntityID, CameraData> CameraManager::m_cameraEntities;
    FlexECS::EntityID CameraManager::m_currMainID = static_cast<uint64_t>(-1);
    FlexECS::EntityID CameraManager::m_currEditorID = static_cast<uint64_t>(-1);

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
        auto result = m_cameraEntities.emplace(entityID, cameraData);
        if (result.second)
        {
            Log::Debug("AddCameraEntity(...) => Added camera " + std::to_string(entityID));
        }
        else
        {
            Log::Warning("AddCameraEntity(...) => Existing camera with same id. Replacing camera " + std::to_string(entityID));
            m_cameraEntities[entityID] = cameraData; // Replace existing entry
        }
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
        if (m_cameraEntities.find(entityID) != m_cameraEntities.end())
        {
            m_currMainID = entityID;
            Log::Debug("SwitchMainCamera(...) => Switched Main to camera " + std::to_string(entityID));
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
        if (m_cameraEntities.find(entityID) != m_cameraEntities.end())
        {
            m_currEditorID = entityID;
            Log::Debug("SwitchEditorCamera(...) => Switched Editor to camera " + std::to_string(entityID));
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
                m_currMainID = static_cast<FlexECS::EntityID>(-1); // Reset to invalid ID
            }
            if (entityID == m_currEditorID)
            {
                m_currEditorID = static_cast<FlexECS::EntityID>(-1); // Reset to invalid ID
            }
            return true;
        }

        // If the camera entity wasn't found
        Log::Warning("RemoveCameraEntity(...) => Entity ID not found. Unable to remove camera with entityID " + std::to_string(entityID));
        return false;
    }

    /*!***************************************************************************
     * \brief
     * Removes all camera entities from the manager.
     *****************************************************************************/
    void CameraManager::RemoveCameraEntities()
    {
        m_cameraEntities.clear();
        m_currMainID = static_cast<uint64_t>(-1);
        m_currEditorID = static_cast<uint64_t>(-1);
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
            Log::Warning("UpdateData(...) => Entity ID not found.");
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
    FlexECS::EntityID CameraManager::GetMainCamera()
    {
        if (m_currMainID == static_cast<uint64_t>(-1))
        {
            Log::Warning("GetMainCamera() => Main Camera is of an invalid ID.");
        }
        return m_currMainID;
    }

    /*!***************************************************************************
     * \brief
     * Retrieves the editor camera entity ID.
     *
     * \return The entity ID of the editor camera.
     *****************************************************************************/
    FlexECS::EntityID CameraManager::GetEditorCamera()
    {
        if (m_currEditorID == static_cast<uint64_t>(-1))
        {
            Log::Warning("GetEditorCamera() => Editor Camera is of an invalid ID.");
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
    const CameraData* CameraManager::GetCameraData(FlexECS::EntityID entityID)
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end())
        {
            return &it->second;
        }
        Log::Warning("GetCameraData(...) => Entity ID not found.");
        return nullptr;
    }

    /*!***************************************************************************
     * \brief
     * Checks if an entity has been added as a camera.
     *
     * \param entityID The ID of the entity to check.
     * \return True if the entity is a camera, false otherwise.
     *****************************************************************************/
    bool CameraManager::HasCameraEntity(FlexECS::EntityID entityID)
    {
        return m_cameraEntities.find(entityID) != m_cameraEntities.end();
    }

    #pragma endregion
}
