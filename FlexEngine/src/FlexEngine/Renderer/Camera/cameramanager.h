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
    * \brief A singleton camera bank for managing camera data and ensuring
    * a default editor camera is created upon initialization.
    *************************************************************************/
    class __FLX_API CameraManager
    {
        std::unordered_map<FlexECS::EntityID, Camera> m_cameraEntities; /*!< Storage for cameras. */
        FlexECS::EntityID m_editorCameraID = INVALID_ENTITY_ID; /*!< Reserved editor camera ID. */

    public:
        /*!************************************************************************
         * \brief Retrieves the singleton instance of the CameraManager.
         * \return Reference to the singleton CameraManager.
         *************************************************************************/
        static CameraManager& Instance();

        /*!************************************************************************
         * \brief Retrieves the camera data for a specific entity.
         * \param entityID ID of the entity to retrieve camera data for.
         * \return Pointer to Camera if the entity exists, nullptr otherwise.
         *************************************************************************/
        const Camera* GetCameraData(FlexECS::EntityID entityID) const;

        /*!************************************************************************
         * \brief Adds or updates a camera for the given entity ID.
         * \param entityID ID of the entity.
         * \param cameraData Camera data to associate with the entity.
         *************************************************************************/
        void SetCameraData(FlexECS::EntityID entityID, const Camera& cameraData);

        /*!************************************************************************
        * \brief Create a camera and parses it in 
        * \param entityID The ID of the camera entity to remove.
        * \return EntityID
        *************************************************************************/
        FlexECS::EntityID CreateCamera(const Camera& cameraData);

        /*!************************************************************************
         * \brief Removes the camera associated with an entity, if it's not the editor camera.
         * \param entityID The ID of the camera entity to remove.
         * \return True if successful, false otherwise.
         *************************************************************************/
        bool RemoveCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Clears all cameras except the editor camera.
         *************************************************************************/
        void ClearCameras();

        /*!************************************************************************
         * \brief Gets the editor camera entity ID.
         * \return Editor camera entity ID.
         *************************************************************************/
        FlexECS::EntityID GetEditorCameraID() const { return m_editorCameraID; }

    private:
        CameraManager(); /*!< Private constructor to enforce singleton. */
        ~CameraManager() = default; /*!< Private destructor. */

        CameraManager(const CameraManager&) = delete; /*!< Non-copyable. */
        CameraManager& operator=(const CameraManager&) = delete; /*!< Non-assignable. */
        CameraManager(CameraManager&&) = delete; /*!< Non-movable. */
        CameraManager& operator=(CameraManager&&) = delete; /*!< Non-move assignable. */

        /*!************************************************************************
         * \brief Creates the default editor camera.
         *************************************************************************/
        void CreateEditorCamera();

    };

}