// WLVERSE [https://wlverse.web.app] 
// 
// camera.h
// 
// Class declaration for interfacing with the camera. Orthographic for now.
// This camera uses a right hand system, following openGL. That is to say, camera looks towards negative Z.
// 
// AUTHORS 
// [100%] Yew Chong (yewchong.k\@digipen.edu) 
// - Main Author 
// 
// Copyright (c) 2024 DigiPen, All rights reserved

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

        bool is_dirty = true;
        float m_OrthoWidth = 1280.0f;
        float m_OrthoHeight = 750.0f;
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
        bool is_active = true;
        CameraData m_data;

        Camera();
        Camera(const Vector3& t_pos,
            float t_orthoWidth,
            float t_orthoHeight,
            float t_nearClip,
            float t_farClip,
            bool t_isActive = true);
        Camera(const Vector3& t_pos,
            const Vector3& t_target,
            const Vector3& t_up,
            float t_fieldOfView,
            float t_aspectRatio,
            float t_nearClip,
            float t_farClip,
            bool t_isActive = true);
        Camera(const CameraData& t_data, bool t_isActive = true);
        ~Camera() = default;

        // Set orthographic projection
        void SetOrthographic(float left, float right, float bottom, float top, float near, float far);

        // Set perspective projection
        void SetPerspective(float fieldOfView, float aspectRatio, float near, float far);

        // Getters for matrices
        const Matrix4x4& GetProjectionMatrix() const;
        const Matrix4x4& GetViewMatrix() const;
        const Matrix4x4& GetProjViewMatrix() const;

        // Activate perspective or orthographic mode
        void SetMode(bool isOrthographic);

        // Updates the view and projection view matrices
        void Update();

        bool getIsActive();

    private:
        void UpdateCameraMatrix();
    };
}
