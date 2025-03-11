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
    Camera::Camera(const Vector3& t_pos,
        float t_orthoWidth,
        float t_orthoHeight,
        float t_nearClip,
        float t_farClip,
        bool t_isActive) : is_active(t_isActive)
    {
        SetOrthographic(-t_orthoWidth / 2, t_orthoWidth / 2, -t_orthoHeight / 2, t_orthoHeight / 2, t_nearClip, t_farClip);
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

    Vector3 Camera::GetPosition()
    {
        // Using the view matrix we can get a position by just inversing the translation, because position is inversed view
        Matrix4x4 inverse_view = m_view_matrix;
        inverse_view.Inverse();
        return Vector3(inverse_view[3]);
    }

    void Camera::Update()
    {
        // Recalculate ortho matrix changes, then update the final matrix
        SetOrthographic(-m_ortho_width / 2, m_ortho_width / 2, -m_ortho_height / 2, m_ortho_height / 2, -2.f, 2.f);
        UpdateEffects(0.1f);
        UpdateCameraMatrix();
    }

    void Camera::Update(float dt)
    {
        // Recalculate ortho matrix changes, then update the final matrix
        SetOrthographic(-m_ortho_width / 2, m_ortho_width / 2, -m_ortho_height / 2, m_ortho_height / 2, -2.f, 2.f);
        UpdateEffects(dt);
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

    #pragma region Camera Effects
    // Helper: Generate a random shake offset based on the given intensity.
    Vector3 Camera::GenerateShakeOffset(float intensity)
    {
        float offsetX = ((std::rand() % 2000) / 1000.0f - 1.0f) * intensity;
        float offsetY = ((std::rand() % 2000) / 1000.0f - 1.0f) * intensity;
        return Vector3(offsetX, offsetY, 0.0f);
    }

    void Camera::EnqueueShakeEffect(float duration, float intensity, bool lerp)
    {
        // Capture the baseline position if no shake is active.
        if (m_shakeEffects.empty())
        {
            m_originalCameraPos = this->GetPosition();
        }
        m_shakeEffects.push_back({ duration, 0.0f, intensity, lerp });
    }

    void Camera::EnqueueZoomEffect(float duration, float targetOrthoWidth, bool autoReturn, float lerpDuration, float holdDuration)
    {
        // Capture baseline if no zoom is active.
        if (!m_zoomActive)
        {
            m_zoomBase = m_ortho_width;
        }
        if (autoReturn)
        {
            m_zoomEffect = { 0.0f, 0.0f, targetOrthoWidth, m_ortho_width, true, lerpDuration, holdDuration, 2 * lerpDuration + holdDuration };
        }
        else
        {
            m_zoomEffect = { duration, 0.0f, targetOrthoWidth, m_ortho_width, false, 0.0f, 0.0f, 0.0f };
        }
        m_zoomActive = true;
    }

    void Camera::UpdateEffects(float dt)
    {
        // ---- Process Shake Effects ----
        Vector3 cumulativeShake(0.0f);
        for (int i = static_cast<int>(m_shakeEffects.size()) - 1; i >= 0; --i)
        {
            auto& effect = m_shakeEffects[i];
            effect.elapsed += dt;
            float intensity = effect.intensity;
            if (effect.lerp)
            {
                float progress = std::min(effect.elapsed / effect.duration, 1.0f);
                float factor = (progress < 0.5f) ? (progress * 2.0f) : ((1.0f - progress) * 2.0f);
                intensity *= factor;
            }
            cumulativeShake += GenerateShakeOffset(intensity);

            if (effect.elapsed >= effect.duration)
            {
                // Remove completed shake effect.
                m_shakeEffects.erase(m_shakeEffects.begin() + i);
            }
        }
        // Update view matrix with shake offset.
        SetViewMatrix(m_originalCameraPos + cumulativeShake);

        // ---- Process Zoom Effects ----
        if (m_zoomActive)
        {
            float deltaZoom = 0.0f;
            m_zoomEffect.elapsed += dt;
            if (m_zoomEffect.autoReturn)
            {
                if (m_zoomEffect.elapsed < m_zoomEffect.lerpDuration)
                {
                    // Ramp-up phase.
                    float t = m_zoomEffect.elapsed / m_zoomEffect.lerpDuration;
                    deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * t;
                }
                else if (m_zoomEffect.elapsed < m_zoomEffect.lerpDuration + m_zoomEffect.holdDuration)
                {
                    // Hold phase.
                    deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth);
                }
                else if (m_zoomEffect.elapsed < m_zoomEffect.totalDuration)
                {
                    // Ramp-down phase.
                    float t = (m_zoomEffect.elapsed - (m_zoomEffect.lerpDuration + m_zoomEffect.holdDuration)) / m_zoomEffect.lerpDuration;
                    deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * (1.0f - t);
                }
                else
                {
                    // Effect completed.
                    deltaZoom = 0.0f;
                    m_zoomActive = false;
                }
            }
            else
            {
                float progress = std::min(m_zoomEffect.elapsed / m_zoomEffect.duration, 1.0f);
                deltaZoom = (m_zoomEffect.targetOrthoWidth - m_zoomEffect.initialOrthoWidth) * progress;
                if (m_zoomEffect.elapsed >= m_zoomEffect.duration)
                {
                    deltaZoom = 0.0f;
                    m_zoomBase = m_zoomEffect.targetOrthoWidth;
                    m_zoomActive = false;
                }
            }

            // Calculate new orthographic dimensions.
            float effectiveOrthoWidth = std::max(m_zoomBase + deltaZoom, m_minOrthoWidth);
            float effectiveOrthoHeight = effectiveOrthoWidth / m_baseAspectRatio;
            SetOrthographic(-effectiveOrthoWidth / 2, effectiveOrthoWidth / 2,
                            -effectiveOrthoHeight / 2, effectiveOrthoHeight / 2);
        }

        UpdateCameraMatrix();
    }
    #pragma endregion
}