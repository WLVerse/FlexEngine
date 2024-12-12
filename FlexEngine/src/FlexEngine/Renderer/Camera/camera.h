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
    Matrix4x4 m_ortho_matrix = FlexEngine::Matrix4x4::Identity;
    Matrix4x4 m_view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Back, Vector3::Up); // Back is our facing direction due to right hand system
    Matrix4x4 m_proj_view_matrix = FlexEngine::Matrix4x4::Identity;

  public:
    Camera(float left, float right, float bottom, float top, float near, float far);
    ~Camera() = default;

    void SetProjection(float left, float right, float bottom, float top, float near, float far);
    const Matrix4x4& GetProjectionMatrix() const { return m_ortho_matrix; }
    const Matrix4x4& GetViewMatrix() const { return m_view_matrix; }
    const Matrix4x4& GetProjViewMatrix() const { return m_proj_view_matrix; }
    
    void MoveCamera(Vector3 translation) { translation.x *= -1; m_view_matrix.Translate(translation); } // Negative 1 as camera move left = world moves right

    /*
      \brief Updates matrices, specifically, the projection view matrix.
    */
    void Update();
  };
}
