/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// camera2d.cpp
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
#include "camera2d.h"

namespace FlexEngine 
{
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
    void Camera2D::SetDirection(CameraData& curr, const Vector3& target)
    {
        curr.target = target;
        curr.right = Cross(curr.up, target).Normalize();
        curr.viewMatrix = Matrix4x4::LookAt(curr.position, curr.position + curr.target, curr.up);
    }

    /*!***************************************************************************
    * \brief
    * Sets the camera's field of view and updates the projection matrix.
    *
    * Legacy
    * 
    * \param[in,out] curr The CameraData instance to update.
    * \param[in] fov The new field of view in degrees.
    *****************************************************************************/
    void Camera2D::SetFieldOfView(CameraData& curr, float fov)
    {
        curr.fieldOfView = fov;
        curr.projMatrix = Matrix4x4::Perspective(fov, curr.aspectRatio, curr.nearClip, curr.farClip);
    }

    /*!***************************************************************************
    * \brief
    * Updates the camera's aspect ratio and recalculates the projection matrix.
    *
    * Legacy
    * 
    * \param[in,out] curr The CameraData instance to update.
    * \param[in] aspect The new aspect ratio to set.
    *****************************************************************************/
    void Camera2D::SetAspectRatio(CameraData& curr, float aspect)
    {
        curr.aspectRatio = aspect;
        curr.projMatrix = Matrix4x4::Perspective(curr.fieldOfView, aspect, curr.nearClip, curr.farClip);
    }

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
    void Camera2D::SetClippingPlanes(CameraData& curr, float near, float far)
    {
        curr.nearClip = near;
        curr.farClip = far;
        curr.projMatrix = Matrix4x4::Perspective(curr.fieldOfView, curr.aspectRatio, near, far);
    }

    #pragma endregion

    #pragma region Camera Management
    /*!***************************************************************************
    * \brief
    * Updates the camera's view matrix based on its position and target.
    *
    * \param[in,out] curr The CameraData instance to update.
    *****************************************************************************/
    void Camera2D::UpdateViewMatrix(CameraData& curr)
    {
        //curr.position, curr.target, curr.up
        curr.viewMatrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Back, Vector3::Up);
        curr.proj_viewMatrix = curr.projMatrix * curr.viewMatrix;
    }
    /*!***************************************************************************
    * \brief
    * Updates the camera's projection matrix, switching between orthographic
    * and perspective modes as needed.
    *
    * \param[in,out] curr The CameraData instance to update.
    *****************************************************************************/
    void Camera2D::UpdateProjectionMatrix(CameraData& curr)
    {
        if (curr.m_isOrthographic)
        {
           //float width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
           //float height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());
           //
            //Center Cam
            curr.projMatrix = Matrix4x4::Orthographic(
                curr.position.x + curr.m_OrthoWidth / 2.0f,
                curr.position.x - curr.m_OrthoWidth / 2.0f,
                curr.position.y + curr.m_OrthoHeight / 2.0f,
                curr.position.y - curr.m_OrthoHeight / 2.0f,
                curr.nearClip,
                curr.farClip
            );
        }
        else
        {
            curr.projMatrix = Matrix4x4::Perspective(curr.fieldOfView, curr.aspectRatio, curr.nearClip, curr.farClip);
        }
        curr.proj_viewMatrix = curr.projMatrix * curr.viewMatrix;
    }

    #pragma endregion

    #pragma region Camera Movement
    /*!***************************************************************************
    * \brief
    * Orients the camera to face a specific target position.
    *
    * \param[in,out] curr The CameraData instance to update.
    * \param[in] target The position the camera should look at.
    *****************************************************************************/
    void Camera2D::LookAt(CameraData& curr, const Vector3& target)
    {
        curr.target = target;
        curr.right = Cross(curr.up, curr.position - curr.target).Normalize();
        curr.viewMatrix = Matrix4x4::LookAt(curr.position, target, curr.up);
    }

    /*!***************************************************************************
    * \brief
    * Moves the camera to a specified position and updates the view matrix.
    *
    * \param[in,out] curr The CameraData instance to update.
    * \param[in] targetPosition The new position for the camera.
    *****************************************************************************/
    void Camera2D::MoveTo(CameraData& curr, const Vector3& targetPosition)
    {
        curr.position = targetPosition;
        UpdateViewMatrix(curr);
    }

    /*!***************************************************************************
    * \brief
    * Interpolates the camera's position towards a target position over time.
    *
    * \param[in,out] curr The CameraData instance to update.
    * \param[in] targetPosition The target position for the camera.
    * \param[in] t The interpolation factor (0.0 to 1.0).
    *****************************************************************************/
    void Camera2D::LerpTo(CameraData& curr, const Vector3& targetPosition, float t)
    {
        curr.position = Lerp(curr.position, targetPosition, t);
        UpdateViewMatrix(curr);
    }
    #pragma endregion
}