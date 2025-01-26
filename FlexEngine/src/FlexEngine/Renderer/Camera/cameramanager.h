/*!************************************************************************
 // WLVERSE [https://wlverse.web.app]
 // cameramanager.h
 //
 // This header file defines the `SceneCamSorter` class, a singleton utility
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
#pragma once

#include "camera.h"
#include "FlexECS/datastructures.h"
#include <unordered_map>


namespace FlexEngine
{
    constexpr uint64_t INVALID_ENTITY_ID = std::numeric_limits<uint64_t>::max();

    class __FLX_API CameraManager
    {
        static std::unordered_map<FlexECS::EntityID, Camera&> m_cameraEntities; /*!< Storage for cameras. */
        static FlexECS::EntityID m_editorCameraID; /*!< Reserved editor camera ID. */
    
    public:
        static void Initialize();

        /*!************************************************************************
         * \brief Adds or updates a camera associated with the given entity ID.
         * \param entityID The ID of the entity to associate with the camera.
         * \param cameraData The camera data to associate with the entity.
         *************************************************************************/
        static void SetCameraRef(FlexECS::EntityID entityID, const Camera& cam);

        /*!************************************************************************
         * \brief Removes the camera associated with the given entity ID.
         * \param entityID The ID of the entity to remove.
         *************************************************************************/
        static void RemoveCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Retrieves the camera associated with the given entity ID.
         * \param entityID The ID of the entity to retrieve.
         * \return A pointer to the camera if it exists, nullptr otherwise.
         *************************************************************************/
        static const Camera* GetCameraRef(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Checks if a camera is registered with the given entity ID.
         * \param entityID The ID of the entity to check.
         * \return True if the camera exists, false otherwise.
         *************************************************************************/
        static bool HasCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Clears all registered cameras.
         *************************************************************************/
        static void Clear();

        /*!************************************************************************
         * \brief Gets the default editor camera entity ID.
         * \return The editor camera entity ID.
         *************************************************************************/
        static FlexECS::EntityID GetEditorCamera() { return m_editorCameraID; }
    };


}