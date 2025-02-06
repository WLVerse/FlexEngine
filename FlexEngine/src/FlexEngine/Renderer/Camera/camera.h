/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * camera.h
 *
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

#pragma once
#include "FlexEngine/FlexMath/matrix4x4.h"
#include "FlexEngine/FlexMath/vector3.h"
#include "flx_api.h"

namespace FlexEngine
{
    #pragma region Reflection
    struct __FLX_API CameraData
    {
        FLX_REFL_SERIALIZABLE
        Vector3 position = Vector3::Zero;        /*!< Camera position in world space */
        Vector3 target = Vector3::Zero;          /*!< Target the camera is facing towards */
        Vector3 up = Vector3::Up;                /*!< Up vector for the camera's orientation */
        Vector3 right = Vector3::Right;          /*!< Right vector for the camera's orientation */

        float fieldOfView = 45.0f;               /*!< Field of view in degrees */
        float aspectRatio = 1.77f;               /*!< Aspect ratio (width/height) */
        float nearClip = 0.1f;                   /*!< Near clipping plane */
        float farClip = 100.0f;                  /*!< Far clipping plane */
        bool m_isOrthographic = true;            /*!< Flag indicating orthographic (true) or perspective (false) projection */

        bool is_dirty = true; // to be removed
        float m_OrthoWidth = 1280.0f;            /*!< Orthographic projection width */
        float m_OrthoHeight = 750.0f;            /*!< Orthographic projection height */
    };
    #pragma endregion

    class __FLX_API Camera
    {
        FLX_REFL_SERIALIZABLE

            Matrix4x4 m_ortho_matrix = FlexEngine::Matrix4x4::Identity;
        Matrix4x4 m_perspective_matrix = FlexEngine::Matrix4x4::Identity;
        Matrix4x4 m_view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Back, Vector3::Up); // Back is our facing direction due to right hand system
        Matrix4x4 m_proj_view_matrix = FlexEngine::Matrix4x4::Identity;
    public:
        bool is_active = true;    /*!< Indicates if the camera is currently active */
        CameraData m_data;        /*!< Camera configuration data */

        /*!
         * \brief Default constructor.
         *
         * Constructs a Camera with default CameraData values and sets the camera
         * as active. Automatically updates the camera matrices.
         */
        Camera();

        /*!
         * \brief Constructs an orthographic Camera.
         *
         * Initializes an orthographic camera using the provided position, orthographic
         * width and height, near and far clip values, and active state. The aspect ratio
         * is computed based on the provided width and height.
         *
         * \param t_pos The position of the camera.
         * \param t_orthoWidth The width for the orthographic projection.
         * \param t_orthoHeight The height for the orthographic projection.
         * \param t_nearClip The near clipping plane distance.
         * \param t_farClip The far clipping plane distance.
         * \param t_isActive (Optional) The active state of the camera. Defaults to true.
         */
        Camera(const Vector3& t_pos,
            float t_orthoWidth,
            float t_orthoHeight,
            float t_nearClip,
            float t_farClip,
            bool t_isActive = true);

        /*!
         * \brief Constructs a perspective Camera.
         *
         * Initializes a perspective camera using the provided position, target, up vector,
         * field of view, aspect ratio, near and far clip values, and active state.
         *
         * \param t_pos The position of the camera.
         * \param t_target The target point the camera is looking at.
         * \param t_up The up vector for the camera's orientation.
         * \param t_fieldOfView The field of view angle in degrees.
         * \param t_aspectRatio The aspect ratio of the projection.
         * \param t_nearClip The near clipping plane distance.
         * \param t_farClip The far clipping plane distance.
         * \param t_isActive (Optional) The active state of the camera. Defaults to true.
         */
        Camera(const Vector3& t_pos,
            const Vector3& t_target,
            const Vector3& t_up,
            float t_fieldOfView,
            float t_aspectRatio,
            float t_nearClip,
            float t_farClip,
            bool t_isActive = true);

        /*!
         * \brief Constructs a Camera using CameraData.
         *
         * Initializes the Camera using a CameraData structure for flexible configuration.
         * The active state is set via the provided parameter. The camera matrices are
         * updated after construction.
         *
         * \param t_data The CameraData structure containing camera parameters.
         * \param t_isActive (Optional) The active state of the camera. Defaults to true.
         */
        Camera(const CameraData& t_data, bool t_isActive = true);

        /*!
         * \brief Destructor.
         */
        ~Camera() = default;

        /*!
         * \brief Sets the camera to use orthographic projection.
         *
         * Configures the camera with an orthographic projection using the specified
         * left, right, bottom, top, near, and far clipping plane values. Updates the
         * internal orthographic matrix and recalculates the projection-view matrix.
         *
         * \param left The left clipping plane.
         * \param right The right clipping plane.
         * \param bottom The bottom clipping plane.
         * \param top The top clipping plane.
         * \param near The near clipping plane distance.
         * \param far The far clipping plane distance.
         */
        void SetOrthographic(float left, float right, float bottom, float top, float near, float far);

        /*!
         * \brief Sets the camera to use perspective projection.
         *
         * Configures the camera with a perspective projection using the specified field
         * of view, aspect ratio, and near/far clipping plane values. Updates the internal
         * perspective matrix and recalculates the projection-view matrix.
         *
         * \param fieldOfView The field of view angle in degrees.
         * \param aspectRatio The aspect ratio of the projection.
         * \param near The near clipping plane distance.
         * \param far The far clipping plane distance.
         */
        void SetPerspective(float fieldOfView, float aspectRatio, float near, float far);

        /*!
         * \brief Retrieves the current projection matrix.
         *
         * \return A constant reference to the current projection matrix. This will be the
         *         orthographic matrix if the camera is in orthographic mode, or the perspective
         *         matrix otherwise.
         */
        const Matrix4x4& GetProjectionMatrix() const;

        /*!
         * \brief Retrieves the current view matrix.
         *
         * \return A constant reference to the view matrix.
         */
        const Matrix4x4& GetViewMatrix() const;

        /*!
         * \brief Retrieves the combined projection-view matrix.
         *
         * \return A constant reference to the combined projection-view matrix.
         */
        const Matrix4x4& GetProjViewMatrix() const;

        /*!
         * \brief Activates either orthographic or perspective mode.
         *
         * Sets the camera mode based on the provided flag and updates the camera matrices.
         *
         * \param isOrthographic Pass \c true to enable orthographic mode, or \c false to enable
         *                       perspective mode.
         */
        void SetMode(bool isOrthographic);

        /*!
         * \brief Updates the camera matrices.
         *
         * Updates the view matrix (using a LookAt calculation for perspective mode) and the
         * combined projection-view matrix based on the current projection mode.
         */
        void Update();

        /*!
         * \brief Checks if the camera is active.
         *
         * \return \c true if the camera is active, otherwise \c false.
         */
        bool getIsActive();

    private:
        /*!
         * \brief Recalculates the combined projection-view matrix.
         *
         * Updates the internal projection-view matrix based on whether the camera is in
         * orthographic or perspective mode.
         */
        void UpdateCameraMatrix();
    };
}
