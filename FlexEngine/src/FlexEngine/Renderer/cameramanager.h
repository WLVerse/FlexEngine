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

//Will later shift the code to cpp
namespace FlexEngine
{
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
    };

    /*!************************************************************************
    * \class CameraManager
    * \brief
    * A manager for handling camera entities within the ECS framework. It provides
    * functionality to register, update, and retrieve camera data, as well as
    * manage transitions between main and editor cameras.
    *************************************************************************/
    class __FLX_API CameraManager
    {
        static std::unordered_map<FlexECS::EntityID, CameraData> m_cameraEntities; /*!< Maps entity IDs to CameraData */
        static FlexECS::EntityID m_currMainID;                                     /*!< Current main camera entity ID */
        static FlexECS::EntityID m_currEditorID;                                   /*!< Current editor camera entity ID */
    public:
        /*!************************************************************************
         * \brief Adds a camera entity to the manager.
         * \param entityID ID of the entity.
         * \param cameraData Camera data to associate with the entity.
         * \return True if the camera was added, false otherwise.
         *************************************************************************/
        static void AddCameraEntity(FlexECS::EntityID entityID, const CameraData& cameraData);

        /*!************************************************************************
         * \brief Switches the main camera to the given entity ID.
         * \param entityID ID of the new main camera.
         * \return True if successful, false otherwise.
         *************************************************************************/
        static bool SwitchMainCamera(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Switches the editor camera to the given entity ID.
         * \param entityID ID of the new editor camera.
         * \return True if successful, false otherwise.
         *************************************************************************/
        static bool SwitchEditorCamera(FlexECS::EntityID entityID);

        /*!***************************************************************************
        * \brief
        * Removes a specific camera entity by its ID.
        *
        * \param entityID The ID of the entity to remove.
        * \return True if the camera was removed, false if the entity ID was not found.
        *****************************************************************************/
        static bool RemoveCameraEntity(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Removes all registered camera entities.
         *************************************************************************/
        static void RemoveCameraEntities();

        /*!************************************************************************
         * \brief Retrieves the camera data for a specific entity.
         * \param entityID ID of the entity.
         * \return Pointer to CameraData if the entity exists, nullptr otherwise.
         *************************************************************************/
        static const CameraData* GetCameraData(FlexECS::EntityID entityID);

        /*!************************************************************************
         * \brief Gets the current main camera ID.
         * \return Main camera entity ID.
         *************************************************************************/
        static FlexECS::EntityID GetMainCamera();

        /*!************************************************************************
         * \brief Gets the current editor camera ID.
         * \return Editor camera entity ID.
         *************************************************************************/
        static FlexECS::EntityID GetEditorCamera();

        /*!************************************************************************
         * \brief Checks if a given entity ID is registered as a camera.
         * \param entityID ID of the entity.
         * \return True if the entity is a camera, false otherwise.
         *************************************************************************/
        static bool HasCameraEntity(FlexECS::EntityID entityID);

        /*!***************************************************************************
        * \brief
        * Updates the CameraData for a given entity ID.
        *
        * \param entityID The ID of the entity to update.
        * \param curr The new CameraData to update.
        *****************************************************************************/
        static void UpdateData(const FlexECS::EntityID entityID, const CameraData& curr);
    };
}


