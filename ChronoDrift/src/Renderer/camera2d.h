#pragma once
/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// camera2d.h
//
// Manages the 2D camera system for the game engine, supporting both
// orthographic and perspective projections. Provides functionalities for
// camera transformations, movement, and view updates, enabling dynamic
// and smooth camera behavior during gameplay.
//
// Key functionalities include:
// - Setting and updating the camera's view and projection matrices for 2D
//   rendering, supporting both orthographic and perspective modes.
// - Managing camera movement and transitions, including smooth linear
//   interpolation (lerp) for camera position changes.
// - Handling camera parameters such as field of view, aspect ratio, and
//   clipping planes to ensure correct 2D rendering in various scenarios.
// - Providing flexible camera controls through target-based direction
//   setting and position adjustments.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//   - Developed camera transformation system and matrix management to
//     facilitate 2D camera behavior and rendering flexibility.
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#include "flx_api.h"

#include "FlexMath/matrix4x4.h"
#include <Renderer/cameramanager.h>

namespace FlexEngine
{
    /*!************************************************************************
    * \class Camera2D
    * \brief Utility class for managing 2D camera transformations and properties.
    *************************************************************************/
    class __FLX_API Camera2D
    {
    public:
        #pragma region Set Functions

        /*!***************************************************************************
        * \brief
        * Sets the camera's direction by updating the target vector and recalculating
        * the view matrix accordingly.
        *
        * Legacy
        *
        * \param[in,out] curr The CameraData instance to update.
        * \param[in] target The direction vector to set for the camera.
        *****************************************************************************/
        static void SetDirection(CameraData& curr, const Vector3& direction);

        /*!***************************************************************************
        * \brief
        * Sets the camera's field of view and updates the projection matrix.
        *
        * Legacy
        *
        * \param[in,out] curr The CameraData instance to update.
        * \param[in] fov The new field of view in degrees.
        *****************************************************************************/
        static void SetFieldOfView(CameraData& curr, float fov);

        /*!***************************************************************************
        * \brief
        * Updates the camera's aspect ratio and recalculates the projection matrix.
        *
        * Legacy
        *
        * \param[in,out] curr The CameraData instance to update.
        * \param[in] aspect The new aspect ratio to set.
        *****************************************************************************/
        static void SetAspectRatio(CameraData& curr, float aspect);

        /*!***************************************************************************
        * \brief
        * Adjusts the near and far clipping planes and recalculates the projection matrix.
        *
        * Legacy
        *
        * \param[in,out] curr The CameraData instance to update.
        * \param[in] near The near clipping plane distance.
        * \param[in] far The far clipping plane distance.
        *****************************************************************************/
        static void SetClippingPlanes(CameraData& curr, float near, float far);

        #pragma endregion

        #pragma region Camera Movement
        /*!***************************************************************************
        * \brief
        * Moves the camera to a specified position and updates the view matrix.
        *
        * Legacy
        *
        * \param[in,out] curr The CameraData instance to update.
        * \param[in] targetPosition The new position for the camera.
        *****************************************************************************/
        static void MoveTo(CameraData& curr, const Vector3& targetPosition);

        /*!***************************************************************************
        * \brief
        * Interpolates the camera's position towards a target position over time.
        *
        * Legacy
        *
        * \param[in,out] curr The CameraData instance to update.
        * \param[in] targetPosition The target position for the camera.
        * \param[in] t The interpolation factor (0.0 to 1.0).
        *****************************************************************************/
        static void LerpTo(CameraData& curr, const Vector3& targetPosition, float t);

        /*!***************************************************************************
        * \brief
        * Orients the camera to face a specific target position.
        *
        * Legacy
        *
        * \param[in,out] curr The CameraData instance to update.
        * \param[in] target The position the camera should look at.
        *****************************************************************************/
        static void LookAt(CameraData& curr, const Vector3& target);

        #pragma endregion

        #pragma region Camera Management

        /*!***************************************************************************
        * \brief
        * Updates the camera's view matrix based on its position and target.
        *
        * \param[in,out] curr The CameraData instance to update.
        *****************************************************************************/
        static void UpdateViewMatrix(CameraData& curr);

        /*!***************************************************************************
        * \brief
        * Updates the camera's projection matrix, switching between orthographic
        * and perspective modes as needed.
        *
        * \param[in,out] curr The CameraData instance to update.
        *****************************************************************************/
        static void UpdateProjectionMatrix(CameraData& curr);

        #pragma endregion
    };
}

