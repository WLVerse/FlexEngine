
/*!************************************************************************
 // WLVERSE [https://wlverse.web.app]
 // scenecamsorter.cpp
 //
 // This file defines the `SceneCamSorter` class, a singleton utility
 // within the FlexEngine framework for managing camera entities in a 3D scene.
 // The `SceneCamSorter` class provides functionalities for registering and
 // tracking active camera entities, facilitating seamless transitions between
 // main and editor cameras. It also offers methods to retrieve and update
 // `Camera` properties, which encapsulate essential camera attributes
 // such as position, orientation, and projection matrices.
 //
 // Key functionalities include:
 // - Adding, switching, and removing camera entities in the scene.
 // - Providing fast access to `Camera`, including view and projection
 //   matrices, for any registered camera.
 // - Ensuring only one instance manages camera sorting, adhering to the
 //   singleton design pattern.
 //
 // The `Camera` struct, also defined here, includes properties for world
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
    #pragma region Static Member Initialization
    std::unordered_map<FlexECS::EntityID, Camera> CameraManager::m_cameraEntities;
    #pragma endregion

    #pragma region Private
    CameraManager::CameraManager(bool autoCreateEditor)
        : m_autoCreateEditorCamera(autoCreateEditor), m_currEditorID(INVALID_ENTITY_ID)
    {
        if (m_autoCreateEditorCamera)
        {
            CreateDefaultEditorCamera();
            Log::Debug("CameraManager initialized with an editor camera.");
        }
        else
        {
            Log::Debug("CameraManager initialized without an editor camera.");
        }
    }

    CameraManager::~CameraManager()
    {
        ClearAllCameras();
        Log::Debug("CameraManager destroyed and all cameras cleared.");
    }

    void CameraManager::CreateDefaultEditorCamera()
    {
        FlexECS::EntityID editorCameraID = 0; // Reserved for the editor camera
        Camera editorCamera{}; // Default editor camera data

        AddCameraEntity(editorCameraID, editorCamera);
        SwitchEditorCamera(editorCameraID);
    }

    void CameraManager::ClearAllCameras()
    {
        Log::Debug("ClearAllCameras() => Removing all cameras.");
        m_cameraEntities.clear();
        m_currMainID = INVALID_ENTITY_ID;
        m_currEditorID = INVALID_ENTITY_ID;
    }
    #pragma endregion

    #pragma region Camera Management
    void CameraManager::AddCameraEntity(FlexECS::EntityID entityID, const Camera& cameraData)
    {
        if (entityID == INVALID_ENTITY_ID)
        {
            Log::Error("AddCameraEntity() => Invalid entity ID provided.");
            return;
        }

        auto result = m_cameraEntities.emplace(entityID, cameraData);
        if (result.second)
        {
            Log::Debug("AddCameraEntity() => Added camera with entityID " + std::to_string(entityID));
        }
        else
        {
            Log::Warning("AddCameraEntity() => Camera with entityID " + std::to_string(entityID) +
                         " already exists. Overwriting.");
            m_cameraEntities[entityID] = cameraData;
        }

        if (m_currMainID == INVALID_ENTITY_ID && cameraData.getIsActive())
        {
            SwitchMainCamera(entityID);
        }
    }

    bool CameraManager::SwitchMainCamera(FlexECS::EntityID entityID)
    {
        if (entityID == INVALID_ENTITY_ID || m_cameraEntities.empty())
        {
            Log::Error("SwitchMainCamera() => Invalid operation with an empty camera list or invalid entityID.");
            return false;
        }

        if (m_cameraEntities.count(entityID))
        {
            m_currMainID = entityID;
            Log::Debug("SwitchMainCamera() => Main camera switched to entityID " + std::to_string(entityID));
            return true;
        }

        Log::Warning("SwitchMainCamera() => Camera entityID not found.");
        return false;
    }

    bool CameraManager::SwitchEditorCamera(FlexECS::EntityID entityID)
    {
        if (entityID == INVALID_ENTITY_ID || m_cameraEntities.empty())
        {
            Log::Error("SwitchEditorCamera() => Invalid operation with an empty camera list or invalid entityID.");
            return false;
        }

        if (m_cameraEntities.count(entityID))
        {
            m_currEditorID = entityID;
            Log::Debug("SwitchEditorCamera() => Editor camera switched to entityID " + std::to_string(entityID));
            return true;
        }

        Log::Warning("SwitchEditorCamera() => Camera entityID not found.");
        return false;
    }

    bool CameraManager::RemoveCameraEntity(FlexECS::EntityID entityID)
    {
        if (m_cameraEntities.erase(entityID))
        {
            Log::Debug("RemoveCameraEntity() => Removed camera with entityID " + std::to_string(entityID));

            if (entityID == m_currMainID)
            {
                m_currMainID = INVALID_ENTITY_ID;
            }
            if (entityID == m_currEditorID)
            {
                m_currEditorID = INVALID_ENTITY_ID;
            }

            return true;
        }

        Log::Warning("RemoveCameraEntity() => Camera entityID " + std::to_string(entityID) + " not found.");
        return false;
    }

    void CameraManager::RemoveCamerasInScene()
    {
        Log::Info("RemoveCamerasInScene() => Removing non-editor cameras.");
        for (auto it = m_cameraEntities.begin(); it != m_cameraEntities.end();)
        {
            if (it->first != m_currEditorID)
            {
                Log::Debug("Removing camera with entityID " + std::to_string(it->first));
                it = m_cameraEntities.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (m_currMainID != m_currEditorID)
        {
            m_currMainID = INVALID_ENTITY_ID;
        }
    }

    void CameraManager::UpdateData(FlexECS::EntityID entityID, const Camera& curr)
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end())
        {
            it->second = curr;
            Log::Debug("UpdateData() => Updated camera data for entityID " + std::to_string(entityID));
        }
        else
        {
            Log::Warning("UpdateData() => Camera with entityID " + std::to_string(entityID) + " not found.");
        }
    }

    void CameraManager::SetCameraActive(FlexECS::EntityID entityID, bool active)
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end())
        {
            it->second.getIsActive() = active;
            Log::Debug("SetCameraActive() => Set camera entityID " + std::to_string(entityID) +
                       " active state to " + (active ? "true" : "false"));

            if (!active)
            {
                ValidateMainCamera();
            }
        }
        else
        {
            Log::Warning("SetCameraActive() => Camera with entityID " + std::to_string(entityID) + " not found.");
        }
    }

    void CameraManager::ValidateMainCamera()
    {
        if (m_currMainID == INVALID_ENTITY_ID ||
            m_cameraEntities.count(m_currMainID) == 0 ||
            !m_cameraEntities[m_currMainID].getIsActive())
        {
            m_currMainID = INVALID_ENTITY_ID;

            for (const auto& [id, camera] : m_cameraEntities)
            {
                if (id != m_currEditorID && camera.getIsActive())
                {
                    m_currMainID = id;
                    break;
                }
            }

            if (m_currMainID == INVALID_ENTITY_ID)
            {
                Log::Warning("ValidateMainCamera() => No valid main camera found.");
            }
        }
    }
    #pragma endregion

    #pragma region Get Functions
    FlexECS::EntityID CameraManager::GetMainCamera() const
    {
        return m_currMainID;
    }

    FlexECS::EntityID CameraManager::GetEditorCamera() const
    {
        return m_currEditorID;
    }

    const Camera* CameraManager::GetCameraData(FlexECS::EntityID entityID) const
    {
        auto it = m_cameraEntities.find(entityID);
        if (it != m_cameraEntities.end())
        {
            return &it->second;
        }

        Log::Warning("GetCameraData() => Camera with entityID " + std::to_string(entityID) + " not found.");
        return nullptr;
    }

    bool CameraManager::HasCameraEntity(FlexECS::EntityID entityID) const
    {
        return m_cameraEntities.count(entityID) > 0;
    }
    #pragma endregion
}
