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
    // Structures for camera effects.
    struct ShakeEffect
    {
        float duration;
        float elapsed;
        float intensity;
        bool lerp;
    };

    struct ZoomEffect
    {
        // For persistent zoom, 'duration' is used;
        // for auto-return zoom, these extra fields are used.
        float duration;         // For persistent zoom (non-auto-return)
        float elapsed;          // Elapsed time for the effect
        float targetOrthoWidth; // The desired orthographic width
        float initialOrthoWidth;// The width at the start of the effect
        bool autoReturn;        // Whether to auto-return to baseline
        // Auto-return specific:
        float lerpDuration;     // Duration of ramp-up and ramp-down phases
        float holdDuration;     // Duration at the target width
        float totalDuration;    // Total duration for auto-return effect
    };

    class __FLX_API Camera
    {
        FLX_REFL_SERIALIZABLE

            Matrix4x4 m_ortho_matrix = FlexEngine::Matrix4x4::Identity;
        Matrix4x4 m_perspective_matrix = FlexEngine::Matrix4x4::Identity;
        Matrix4x4 m_view_matrix = Matrix4x4::Identity; // Back is our facing direction due to right hand system
        Matrix4x4 m_proj_view_matrix = FlexEngine::Matrix4x4::Identity;

    public:
        float m_ortho_width = 0.f;
        float m_ortho_height = 0.f;
        bool is_active = true;

        std::vector<ShakeEffect> m_shakeEffects;
        ZoomEffect m_zoomEffect;
        bool m_zoomActive = false;
        float m_zoomBase = 1600.f;       // Baseline ortho width (can be updated externally)
        float m_minOrthoWidth = 100.f;     // Minimum allowed width
        float m_baseAspectRatio = 1600.f / 900.f; // Aspect ratio (width / height)
        Vector3 m_originalCameraPos = Vector3::Zero; // Baseline position for shake effect

        //Set position as well
        Camera(const Vector3& t_pos = Vector3::Zero,
            float t_orthoWidth = 1600.f,
            float t_orthoHeight = 900.f,
            float t_nearClip = -2.0f,
            float t_farClip = 2.0f,
            bool t_isActive = true);

        ~Camera() = default;

        // Set orthographic projection
        void SetOrthographic(float left, float right, float bottom, float top, float near = -2.0f, float far = 2.0f);
        void SetViewMatrix(Vector3 pos);

        Vector3 GetPosition();

        // Getters for matrices
        const Matrix4x4& GetProjectionMatrix() const;
        const Matrix4x4& GetViewMatrix() const;
        const Matrix4x4& GetProjViewMatrix() const;

        // Updates the view and projection view matrices
        void Update();
        void Update(float dt);

        bool getIsActive();
        float GetOrthoWidth();
        float GetOrthoHeight();

        #pragma region Camera Effects
        // Update effects each frame.
        // This method will update shake and zoom effects based on deltaTime,
        // adjust the camera's view and projection matrices accordingly.
        void UpdateEffects(float dt);

        // Enqueue a shake effect.
        void EnqueueShakeEffect(float duration, float intensity, bool lerp);

        // Enqueue a zoom effect.
        // For persistent zoom, set autoReturn to false.
        // For auto-return zoom, provide lerpDuration and holdDuration.
        void EnqueueZoomEffect(float duration, float targetOrthoWidth, bool autoReturn, float lerpDuration = 0.0f, float holdDuration = 0.0f);

        // Check if there are any active shake effects.
        bool HasActiveShakeEffects() const { return !m_shakeEffects.empty(); }

        // Check if a zoom effect is active.
        bool IsZoomActive() const { return m_zoomActive; }
        #pragma endregion
    private:
        void UpdateCameraMatrix();

        Vector3 GenerateShakeOffset(float intensity);
    };
}
