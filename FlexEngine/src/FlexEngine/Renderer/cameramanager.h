/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// cameramanager.h
//
// This file implements the `CameraManager` class, a singleton utility
// within the FlexEngine framework for managing and organizing camera
// entities in a 3D scene. It is designed to handle both runtime and editor
// camera management efficiently.
//
// Key Responsibilities:
// - Registering, enabling, disabling, and removing camera entities.
// - Facilitating seamless transitions between main and editor cameras.
// - Providing fast access to `CameraData` for rendering purposes,
//   including view and projection matrices.
// - Automatically managing a default editor camera for scene editing.
// - Validating and maintaining an active main camera for rendering.
//
// CameraData encapsulates critical camera attributes such as:
// - World position and orientation vectors (position, up, right).
// - Projection properties like field of view, aspect ratio, and clip planes.
// - Flags for orthographic or perspective projections.
//
// Main Features:
// - Automatic editor camera creation for development workflows.
// - Efficient lifecycle management for all registered camera entities.
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
        
        /*!***************************************************************************
        * \brief
        * Removes all cameras in the scene except the editor camera.
        *****************************************************************************/
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
         * Replaces the CameraData for a specific entity with new data.
         *
         * \param entityID The ID of the camera entity to update.
         * \param curr The new CameraData to assign to the entity.
         *****************************************************************************/
        void UpdateData(FlexECS::EntityID entityID, const CameraData& curr);
    
        /*!***************************************************************************
         * \brief
         * Activates the specified camera entity by enabling its `cam_is_active` flag.
         *
         * \param entityID The ID of the entity to enable.
         * \return True if the camera was successfully enabled, false otherwise.
         *****************************************************************************/
        bool EnableCameraEntity(FlexECS::EntityID entityID);

        /*!***************************************************************************
         * \brief
         * Deactivates the specified camera entity by disabling its `cam_is_active` flag
         * and triggers revalidation of the main camera.
         *
         * \param entityID The ID of the entity to disable.
         * \return True if the camera was successfully disabled, false otherwise.
         *****************************************************************************/
        bool DisableCameraEntity(FlexECS::EntityID entityID);

        /*!***************************************************************************
         * \brief
         * Ensures the main camera is valid and active. If the current main camera
         * becomes invalid, attempts to find and assign a new active camera as the main.
         *****************************************************************************/
        void ValidateMainCamera();
    };
}

