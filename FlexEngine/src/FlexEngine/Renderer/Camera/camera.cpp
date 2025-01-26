// WLVERSE [https://wlverse.web.app] 
// camera.cpp
// 
// The current planned implementation as of 11/12 is an orthographic camera for now.
// 
// AUTHORS 
// [100%] Yew Chong (yewchong.k\@digipen.edu) 
// - Main Author 
// 
// Copyright (c) 2024 DigiPen, All rights reserved

#include "Renderer/Camera/camera.h"

namespace FlexEngine
{
    #pragma region Reflection
    FLX_REFL_REGISTER_START(Camera::CamData)
        FLX_REFL_REGISTER_PROPERTY(position)
        FLX_REFL_REGISTER_PROPERTY(target)
        FLX_REFL_REGISTER_PROPERTY(up)
        FLX_REFL_REGISTER_PROPERTY(right)
        FLX_REFL_REGISTER_PROPERTY(fieldOfView)
        FLX_REFL_REGISTER_PROPERTY(aspectRatio)
        FLX_REFL_REGISTER_PROPERTY(nearClip)
        FLX_REFL_REGISTER_PROPERTY(farClip)
        FLX_REFL_REGISTER_PROPERTY(m_isOrthographic)
        FLX_REFL_REGISTER_PROPERTY(m_OrthoWidth)
        FLX_REFL_REGISTER_PROPERTY(m_OrthoHeight)
        FLX_REFL_REGISTER_END;

    #pragma endregion

    Camera::Camera(float left, float right, float bottom, float top, float near, float far)
    {
        SetOrthographic(left, right, bottom, top, near, far);
        UpdateCameraMatrix();
    }

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

    void Camera::SetMode(bool isOrthographic)
    {
        m_data.m_isOrthographic = isOrthographic;
        UpdateCameraMatrix();
    }

    void Camera::Update()
    {
        m_view_matrix = Matrix4x4::LookAt(m_data.position, m_data.target, m_data.up);
        UpdateCameraMatrix();
    }

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

    const Matrix4x4& Camera::GetProjectionMatrix() const
    {
        return m_data.m_isOrthographic ? m_ortho_matrix : m_perspective_matrix;
    }

    const Matrix4x4& Camera::GetViewMatrix() const
    {
        return m_view_matrix;
    }

    const Matrix4x4& Camera::GetProjViewMatrix() const
    {
        return m_proj_view_matrix;
    }

    bool Camera::getIsActive()
    {
        return m_data.is_active;
    }
}