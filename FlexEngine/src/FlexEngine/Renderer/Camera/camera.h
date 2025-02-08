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
    class __FLX_API Camera
    {
        FLX_REFL_SERIALIZABLE

        Matrix4x4 m_ortho_matrix = FlexEngine::Matrix4x4::Identity;
        Matrix4x4 m_perspective_matrix = FlexEngine::Matrix4x4::Identity;
        Matrix4x4 m_view_matrix = Matrix4x4::Identity; // Back is our facing direction due to right hand system
        Matrix4x4 m_proj_view_matrix = FlexEngine::Matrix4x4::Identity;
        
    public:
        float m_ortho_width;
        float m_ortho_height;
        bool is_active = true;

        // Does nothing, do not use this. For serialization only.
        Camera();
        
        //Set position as well
        Camera(const Vector3& t_pos,
            float t_orthoWidth,
            float t_orthoHeight,
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

        bool getIsActive();
        float GetOrthoWidth();
        float GetOrthoHeight();

    private:
        void UpdateCameraMatrix();
    };
}
