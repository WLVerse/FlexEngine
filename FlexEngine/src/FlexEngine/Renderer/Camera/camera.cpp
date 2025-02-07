/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * camera.cpp
 *
 * The current planned implementation as of 11/12 is an orthographic camera for now.
 * This file implements the Camera class, providing both orthographic and perspective
 * camera functionalities. It supports creating cameras using position, target, and
 * various projection parameters. The camera can be toggled between orthographic and
 * perspective modes and automatically updates its projection and view matrices.
 *
 * Key functionalities include:
 * - Construction of cameras with either orthographic or perspective settings.
 * - Setting orthographic or perspective projection parameters.
 * - Toggling between orthographic and perspective camera modes.
 * - Updating the camera view and projection matrices based on changes.
 *
 * AUTHORS
 * [50%] Yew Chong (yewchong.k@digipen.edu)
 *   - Main Author
 * [50%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Main Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/

#include "Renderer/Camera/camera.h"

namespace FlexEngine
{
    /*!
     * \brief Default constructor for Camera.
     *
     * Constructs a Camera with default CameraData values and sets the camera
     * as inactive. Automatically updates the camera matrices.
     */
    Camera::Camera() : m_data(), is_active(false)
    {
        UpdateCameraMatrix();
    }

    /*!
     * \brief Constructs an orthographic Camera.
     *
     * This constructor initializes an orthographic camera using the provided
     * position, orthographic width and height, near/far clip values, and
     * active state. It calculates the aspect ratio based on the provided
     * width and height and sets up the orthographic projection matrix.
     *
     * \param t_pos The position of the camera.
     * \param t_orthoWidth The width of the orthographic projection.
     * \param t_orthoHeight The height of the orthographic projection.
     * \param t_nearClip The near clipping plane distance.
     * \param t_farClip The far clipping plane distance.
     * \param t_isActive Whether the camera is active.
     */
    Camera::Camera(const Vector3& t_pos,
        float t_orthoWidth,
        float t_orthoHeight,
        float t_nearClip,
        float t_farClip,
        bool t_isActive) : is_active(t_isActive)
    {
        m_data.position = t_pos;
        m_data.m_OrthoWidth = t_orthoWidth;
        m_data.m_OrthoHeight = t_orthoHeight;
        m_data.nearClip = t_nearClip;
        m_data.farClip = t_farClip;
        m_data.m_isOrthographic = true;
        m_data.aspectRatio = t_orthoWidth / t_orthoHeight;
        SetOrthographic(m_data.position.x - m_data.m_OrthoWidth / 2,
                        m_data.position.x + m_data.m_OrthoWidth / 2,
                        m_data.position.y - m_data.m_OrthoHeight / 2,
                        m_data.position.y + m_data.m_OrthoHeight / 2,
                        m_data.nearClip,
                        m_data.farClip);
    }

    /*!
     * \brief Constructs a perspective Camera.
     *
     * This constructor initializes a perspective camera using the provided
     * position, target, up vector, field of view, aspect ratio, and near/far
     * clip values. The camera is marked as inactive or active based on the
     * parameter and automatically updates the view and projection matrices.
     *
     * \param t_pos The position of the camera.
     * \param t_target The target point the camera is looking at.
     * \param t_up The up vector for the camera.
     * \param t_fieldOfView The camera's field of view.
     * \param t_aspectRatio The aspect ratio for the perspective projection.
     * \param t_nearClip The near clipping plane distance.
     * \param t_farClip The far clipping plane distance.
     * \param t_isActive Whether the camera is active.
     */
    Camera::Camera(const Vector3& t_pos,
        const Vector3& t_target,
        const Vector3& t_up,
        float t_fieldOfView,
        float t_aspectRatio,
        float t_nearClip,
        float t_farClip,
        bool t_isActive) : is_active(t_isActive)
    {
        m_data.position = t_pos;
        m_data.target = t_target;
        m_data.up = t_up;
        m_data.fieldOfView = t_fieldOfView;
        m_data.aspectRatio = t_aspectRatio;
        m_data.nearClip = t_nearClip;
        m_data.farClip = t_farClip;
        m_data.m_isOrthographic = false;
        UpdateCameraMatrix();
    }

    /*!
     * \brief Constructs a Camera using pre-defined CameraData.
     *
     * This constructor initializes the Camera using a CameraData struct,
     * allowing for flexible configuration. The active state is set via
     * the provided parameter. The camera matrices are updated after
     * construction.
     *
     * \param t_data The CameraData structure containing camera parameters.
     * \param t_isActive Whether the camera is active.
     */
    Camera::Camera(const CameraData& t_data, bool t_isActive) : m_data(t_data), is_active(t_isActive)
    {
        UpdateCameraMatrix();
    }

    /*!
     * \brief Sets the camera to orthographic mode.
     *
     * Configures the camera with an orthographic projection by setting the
     * left, right, bottom, top, near, and far values. Updates the internal
     * orthographic matrix and recalculates the combined projection-view matrix.
     *
     * \param left The left clipping plane.
     * \param right The right clipping plane.
     * \param bottom The bottom clipping plane.
     * \param top The top clipping plane.
     * \param near The near clipping plane distance.
     * \param far The far clipping plane distance.
     */
    void Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far)
    {
        m_data.m_isOrthographic = true;
        m_data.nearClip = near;
        m_data.farClip = far;
        m_data.m_OrthoWidth = right - left;
        m_data.m_OrthoHeight = top - bottom;

        m_ortho_matrix = Matrix4x4::Orthographic(left, right, bottom, top, near, far);
        UpdateCameraMatrix();
    }

    /*!
     * \brief Sets the camera to perspective mode.
     *
     * Configures the camera with a perspective projection by setting the field
     * of view, aspect ratio, and near/far clip values. Updates the internal
     * perspective matrix and recalculates the combined projection-view matrix.
     *
     * \param fieldOfView The field of view angle.
     * \param aspectRatio The aspect ratio of the projection.
     * \param near The near clipping plane distance.
     * \param far The far clipping plane distance.
     */
    void Camera::SetPerspective(float fieldOfView, float aspectRatio, float near, float far)
    {
        m_data.m_isOrthographic = false;
        m_data.fieldOfView = fieldOfView;
        m_data.aspectRatio = aspectRatio;
        m_data.nearClip = near;
        m_data.farClip = far;

        m_perspective_matrix = Matrix4x4::Perspective(fieldOfView, aspectRatio, near, far);
        UpdateCameraMatrix();
    }

    /*!
     * \brief Toggles the camera mode between orthographic and perspective.
     *
     * Sets the camera mode based on the provided boolean flag and updates
     * the camera matrices accordingly.
     *
     * \param isOrthographic Pass \c true to set the camera to orthographic mode,
     *                       \c false for perspective mode.
     */
    void Camera::SetMode(bool isOrthographic)
    {
        m_data.m_isOrthographic = isOrthographic;
        UpdateCameraMatrix();
    }

    /*!
     * \brief Updates the camera view or projection based on the current mode.
     *
     * For an orthographic camera, it recalculates the orthographic projection
     * parameters based on the camera position and orthographic dimensions.
     * For a perspective camera, it updates the view matrix using a LookAt calculation.
     */
    void Camera::Update()
    {
        if (m_data.m_isOrthographic) // Orthographic mode
        {
            SetOrthographic(m_data.position.x - m_data.m_OrthoWidth / 2,
                            m_data.position.x + m_data.m_OrthoWidth / 2,
                            m_data.position.y - m_data.m_OrthoHeight / 2,
                            m_data.position.y + m_data.m_OrthoHeight / 2,
                            m_data.nearClip,
                            m_data.farClip);
        }
        else // Perspective mode
        {
            m_view_matrix = Matrix4x4::LookAt(m_data.position, m_data.target, m_data.up);
        }
    }

    /*!
     * \brief Updates the combined projection-view matrix.
     *
     * Recalculates the combined projection-view matrix based on whether the
     * camera is in orthographic or perspective mode. This matrix is used for
     * transforming world coordinates to clip space.
     */
    void Camera::UpdateCameraMatrix()
    {
        if (m_data.m_isOrthographic)
        {
            m_proj_view_matrix = m_ortho_matrix * m_view_matrix;
        }
        else
        {
            m_proj_view_matrix = m_perspective_matrix * m_view_matrix;
        }
    }

    /*!
     * \brief Retrieves the current projection matrix.
     *
     * \return A reference to the orthographic matrix if in orthographic mode,
     *         otherwise a reference to the perspective matrix.
     */
    const Matrix4x4& Camera::GetProjectionMatrix() const
    {
        return m_data.m_isOrthographic ? m_ortho_matrix : m_perspective_matrix;
    }

    /*!
     * \brief Retrieves the current view matrix.
     *
     * \return A reference to the view matrix.
     */
    const Matrix4x4& Camera::GetViewMatrix() const
    {
        return m_view_matrix;
    }

    /*!
     * \brief Retrieves the combined projection-view matrix.
     *
     * \return A reference to the projection-view matrix.
     */
    const Matrix4x4& Camera::GetProjViewMatrix() const
    {
        return m_proj_view_matrix;
    }

    /*!
     * \brief Checks if the camera is currently active.
     *
     * \return \c true if the camera is active; otherwise, \c false.
     */
    bool Camera::getIsActive()
    {
        return is_active;
    }
} // namespace FlexEngine
