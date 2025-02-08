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
#include "FlexMath/matrix4x4.h"

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
      SetOrthographic(-t_orthoWidth / 2, t_orthoWidth/2, -t_orthoHeight/2, t_orthoHeight/2, t_nearClip, t_farClip);
      SetViewMatrix(t_pos);
    }

    void Camera::SetOrthographic(float left, float right, float bottom, float top, float near, float far)
    {
        m_ortho_matrix = Matrix4x4::Orthographic(left, right, bottom, top, near, far);
        m_ortho_width = right - left;
        m_ortho_height = top - bottom;
        UpdateCameraMatrix();
    }

    void Camera::SetViewMatrix(Vector3 pos)
    {
      Matrix4x4 view = Matrix4x4::Identity;
      view.Translate(-pos);
      m_view_matrix = view;
    }



    void Camera::Update()
    {
      UpdateCameraMatrix();
    }

    void Camera::UpdateCameraMatrix()
    {
      m_proj_view_matrix = m_ortho_matrix * m_view_matrix;
    }




    const Matrix4x4& Camera::GetProjectionMatrix() const
    {
        //return m_data.m_isOrthographic ? m_ortho_matrix : m_perspective_matrix;
        return m_ortho_matrix;
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

    float Camera::GetOrthoWidth()
    {
      return m_ortho_width;
    }

    float Camera::GetOrthoHeight()
    {
      return m_ortho_height;
    }
}