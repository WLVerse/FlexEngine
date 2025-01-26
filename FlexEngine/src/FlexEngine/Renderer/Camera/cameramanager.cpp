
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

    std::unordered_map<FlexECS::EntityID, Camera&> CameraManager::m_cameraEntities;
    FlexECS::EntityID CameraManager::m_editorCameraID = INVALID_ENTITY_ID;
    
    #pragma endregion

    #pragma region Private

    #pragma endregion

    #pragma region Camera Lifetime Management
    void CameraManager::Initialize()
    {
        // Create the default editor camera
        Camera editorCamera(0.0f, 1600.0f, 900.0f, 0.0f, -2.0f, 2.0f);
        // Initialize editorCamera as needed
        m_editorCameraID = 0; // Assign a unique ID for the editor camera
        m_cameraEntities[m_editorCameraID] = editorCamera;
    }

    void CameraManager::SetCameraRef(FlexECS::EntityID entityID, const Camera& cam)
    {
        m_cameraEntities[entityID] = cam;
    }

    void CameraManager::RemoveCamera(FlexECS::EntityID entityID)
    {
        m_cameraEntities.erase(entityID);
    }

    const Camera* CameraManager::GetCameraRef(FlexECS::EntityID entityID)
    {
        auto it = m_cameraEntities.find(entityID);
        return (it != m_cameraEntities.end()) ? &it->second : nullptr;
    }

    bool CameraManager::HasCamera(FlexECS::EntityID entityID)
    {
        return m_cameraEntities.find(entityID) != m_cameraEntities.end();
    }

    void CameraManager::Clear()
    {
        m_cameraEntities.clear();
        m_editorCameraID = INVALID_ENTITY_ID;
    }
    #pragma endregion
}
