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

#include <FlexEngine.h>
#include "FlexECS/datastructures.h"
#include <unordered_map>


namespace FlexEngine
{
    constexpr uint64_t INVALID_ENTITY_ID = std::numeric_limits<uint64_t>::max();

    /*!************************************************************************
    * \class CameraManager
    * \brief
    * A manager for handling camera entities within the ECS framework. It provides
    * functionality to register, update, and retrieve camera data, as well as
    * manage transitions between main and editor cameras.
    *
    * 0 -> Editor Cam
    * 1-Inf -> Scene Cam
    *************************************************************************/
    class __FLX_API CameraManager 
    {
        static std::unordered_map<FlexECS::EntityID, Camera> m_cameraEntities; /*!< Maps entity IDs to Camera */
        FlexECS::EntityID m_currMainID = INVALID_ENTITY_ID; /*!< Current main camera entity ID */
        FlexECS::EntityID m_currEditorID = INVALID_ENTITY_ID; /*!< Current editor camera entity ID */
        bool m_autoCreateEditorCamera; /*!< Flag for auto editor camera creation */

        /*!************************************************************************
         * \brief Initializes the CameraManager, creating a default editor camera if enabled.
         *************************************************************************/
        void CreateDefaultEditorCamera();

        /*!************************************************************************
         * \brief Ensures the main camera ID is valid, switching to another active camera if necessary.
         *************************************************************************/
        void ValidateMainCamera();

        /*!************************************************************************
         * \brief Clears all registered camera entities, removing their data from the manager.
         *************************************************************************/
        void ClearAllCameras();

    public:
        // Non-copyable and non-movable to prevent accidental duplication
        CameraManager(const CameraManager&) = delete;
        CameraManager& operator=(const CameraManager&) = delete;
        CameraManager(CameraManager&&) = delete;
        CameraManager& operator=(CameraManager&&) = delete;

        /*!************************************************************************
         * \brief Constructor for CameraManager.
         * \param autoCreateEditor If true, automatically creates an editor camera on initialization.
         *************************************************************************/
        CameraManager(bool autoCreateEditor = true);

        /*!************************************************************************
         * \brief Destructor for CameraManager, ensuring all camera entities are cleaned up.
         *************************************************************************/
        ~CameraManager();

        /*!************************************************************************
         * \brief Adds a camera entity to the manager.
         * \param entityID ID of the entity to associate with the camera.
         * \param Camera Camera data to associate with the entity.
         *************************************************************************/
        void AddCameraEntity(FlexECS::EntityID entityID, const Camera& cameraData);

        /*!************************************************************************
         * \brief Removes a specific camera entity by its ID.
         * \param entityID The ID of the camera entity to remove.
         * \return True if the camera was removed, false otherwise.
         *************************************************************************/
        bool RemoveCameraEntity(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Switches the main camera to the given entity ID.
         * \param entityID ID of the new main camera.
         * \return True if successful, false otherwise.
         *************************************************************************/
        bool SwitchMainCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Switches the editor camera to the given entity ID.
         * \param entityID ID of the new editor camera.
         * \return True if successful, false otherwise.
         *************************************************************************/
        bool SwitchEditorCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Retrieves the camera data for a specific entity.
         * \param entityID ID of the entity to retrieve camera data for.
         * \return Pointer to CameraData if the entity exists, nullptr otherwise.
         *************************************************************************/
        const Camera* GetCameraData(FlexECS::EntityID entityID) const;

        /*!************************************************************************
         * \brief Gets the current main camera ID.
         * \return Main camera entity ID.
         *************************************************************************/
        FlexECS::EntityID GetMainCamera() const { return m_currMainID; }

        /*!************************************************************************
         * \brief Gets the current editor camera ID.
         * \return Editor camera entity ID.
         *************************************************************************/
        FlexECS::EntityID GetEditorCamera() const { return m_currEditorID; }

        /*!************************************************************************
         * \brief Checks if a given entity ID is registered as a camera.
         * \param entityID ID of the entity to check.
         * \return True if the entity is registered as a camera, false otherwise.
         *************************************************************************/
        bool HasCameraEntity(FlexECS::EntityID entityID) const;

        /*!************************************************************************
         * \brief Updates the CameraData for a given entity ID.
         * \param entityID The ID of the entity to update.
         * \param curr The new CameraData to update.
         *************************************************************************/
        void UpdateData(FlexECS::EntityID entityID, const Camera& curr);

        /*!************************************************************************
         * \brief Sets the active state of a camera entity.
         * \param entityID The ID of the camera entity to update.
         * \param active True to enable the camera, false to disable it.
         *************************************************************************/
        void SetCameraActive(FlexECS::EntityID entityID, bool active);

        /*!************************************************************************
         * \brief Removes all cameras associated with the current scene.
         *************************************************************************/
        void RemoveCamerasInScene();
    };
}

