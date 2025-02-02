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
#include "FlexEngine/application.h"

namespace FlexEngine
{
    Camera::Camera() : m_data(), is_active(false) { UpdateCameraMatrix(); }
    
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
        SetOrthographic(m_data.position.x - m_data.m_OrthoWidth/2, 
            m_data.position.x + m_data.m_OrthoWidth / 2, 
            m_data.position.y + m_data.m_OrthoHeight / 2,
            m_data.position.y - m_data.m_OrthoHeight / 2, 
            m_data.nearClip,
            m_data.farClip); //?
        UpdateCameraMatrix();
    }


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


    Camera::Camera(const CameraData& t_data, bool t_isActive) : m_data(t_data), is_active(t_isActive)
    {
        //
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
        if (m_data.m_isOrthographic) //Orthographic
        {
            SetOrthographic(m_data.position.x - m_data.m_OrthoWidth / 2,
                            m_data.position.x + m_data.m_OrthoWidth / 2,
                            m_data.position.y - m_data.m_OrthoHeight / 2,
                            m_data.position.y + m_data.m_OrthoHeight / 2,
                            m_data.nearClip,
                            m_data.farClip); //? Must do like this
        }
        else //Perspective
        {
            m_view_matrix = Matrix4x4::LookAt(m_data.position, m_data.target, m_data.up);

        }
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
        return is_active;
    }

    Vector2 Camera::ScreenToWorldPoint(const Vector2& screen_point)
    {
      const auto& props = Application::GetCurrentWindow()->GetProps();
      return { (screen_point.x + m_data.position.x) - props.width / 2,
               (screen_point.y + m_data.position.y) - props.height / 2 };
    }

    Vector2 Camera::WorldToScreenPoint(const Vector2& worldPoint)
    {
      const auto& props = Application::GetCurrentWindow()->GetProps();
      return { worldPoint.x + props.width / 2 - m_data.position.x,
               worldPoint.y + props.height / 2 - m_data.position.y };
    }
}