/*!************************************************************************
 // WLVERSE [https://wlverse.web.app]
 // scenecamsorter.h
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
    #pragma region Data-type
    /*!************************************************************************
   * \struct CameraData
   * \brief
   * Holds core camera properties, including position, orientation,
   * and transformation matrices.
   *************************************************************************/
    struct __FLX_API CameraData
    {
        FLX_REFL_SERIALIZABLE
        Vector3 position = Vector3::Zero;        /*!< Camera position in world space */
        Vector3 target = Vector3::Zero;          /*!< Target the camera is facing towards */
        Vector3 up = Vector3::Up;                /*!< Up vector for the camera's orientation */
        Vector3 right = Vector3::Right;          /*!< Right vector for the camera's orientation */

        Matrix4x4 viewMatrix = Matrix4x4::Zero;  /*!< View matrix calculated from position and orientation */
        Matrix4x4 projMatrix = Matrix4x4::Zero;  /*!< Projection matrix based on FOV and aspect ratio */
        Matrix4x4 proj_viewMatrix = Matrix4x4::Zero;

        float fieldOfView = 45.0f;               /*!< Field of view in degrees */
        float aspectRatio = 1.77f;               /*!< Aspect ratio (width/height) */
        float nearClip = -2.0f;                  /*!< Near clipping plane */
        float farClip = 2.0f;                    /*!< Far clipping plane */
        bool m_isOrthographic = true;           /*!< Flag indicating orthographic (true) or perspective (false) projection */
    
        bool cam_is_active = true;
        float m_OrthoWidth = 1280.0f;
        float m_OrthoHeight = 750.0f;
    };

    #pragma endregion

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
        std::unordered_map<FlexECS::EntityID, CameraData> m_cameraEntities; /*!< Maps entity IDs to CameraData */
        FlexECS::EntityID m_currMainID = INVALID_ENTITY_ID;                 /*!< Current main camera entity ID */
        FlexECS::EntityID m_currEditorID = INVALID_ENTITY_ID;               /*!< Current editor camera entity ID */
        bool m_autoCreateEditorCamera;                                      /*!< Flag for auto editor camera creation */

        /*!************************************************************************
         * \brief Initializes the CameraManager, creating an editor camera if enabled.
         *************************************************************************/
        void CreateDefaultEditorCamera();

        /*!************************************************************************
        * \brief Removes all registered camera entities.
        *************************************************************************/
        void RemoveCameraEntities();

    public:
        
        // Non-copyable and non-movable to ensure no accidental duplication
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
         * \brief Destructor for CameraManager.
         *************************************************************************/
        ~CameraManager();

        /*!************************************************************************
        * \brief Adds a camera entity to the manager.
        * \param entityID ID of the entity.
        * \param cameraData Camera data to associate with the entity.
        *************************************************************************/
        void AddCameraEntity(FlexECS::EntityID entityID, const CameraData& cameraData);

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

        /*!***************************************************************************
         * \brief
         * Removes a specific camera entity by its ID.
         * \param entityID The ID of the entity to remove.
         * \return True if the camera was removed, false otherwise.
         *************************************************************************/
        bool RemoveCameraEntity(FlexECS::EntityID entityID);

        void RemoveCamerasInScene();

        /*!************************************************************************
         * \brief Retrieves the camera data for a specific entity.
         * \param entityID ID of the entity.
         * \return Pointer to CameraData if the entity exists, nullptr otherwise.
         *************************************************************************/
        const CameraData* GetCameraData(FlexECS::EntityID entityID) const;

        /*!************************************************************************
         * \brief Gets the current main camera ID.
         * \return Main camera entity ID.
         *************************************************************************/
        FlexECS::EntityID GetMainCamera() const;

        /*!************************************************************************
         * \brief Gets the current editor camera ID.
         * \return Editor camera entity ID.
         *************************************************************************/
        FlexECS::EntityID GetEditorCamera() const;

        /*!************************************************************************
         * \brief Checks if a given entity ID is registered as a camera.
         * \param entityID ID of the entity.
         * \return True if the entity is a camera, false otherwise.
         *************************************************************************/
        bool HasCameraEntity(FlexECS::EntityID entityID) const;

        /*!***************************************************************************
         * \brief
         * Updates the CameraData for a given entity ID.
         * \param entityID The ID of the entity to update.
         * \param curr The new CameraData to update.
         *************************************************************************/
        void UpdateData(FlexECS::EntityID entityID, const CameraData& curr);
    
        /*!************************************************************************
        * \brief Enables a camera entity by marking it as active.
        * \param entityID The ID of the camera entity to enable.
        * \return True if the camera was successfully enabled, false otherwise.
        *************************************************************************/
        bool EnableCameraEntity(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Disables a camera entity by marking it as inactive.
         * \param entityID The ID of the camera entity to disable.
         * \return True if the camera was successfully disabled, false otherwise.
         *************************************************************************/
        bool DisableCameraEntity(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Validates and ensures the main camera ID is always valid.
         *        If the current main camera becomes inactive, it switches to the first
         *        available active camera.
         *************************************************************************/
        void ValidateMainCamera();
    };
}

