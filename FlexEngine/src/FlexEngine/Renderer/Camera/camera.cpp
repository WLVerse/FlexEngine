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
  Camera::Camera(float left, float right, float bottom, float top, float near, float far)
  {
    this->SetProjection(left, right, bottom, top, near, far);
    UpdateCameraMatrix();
  }

  void Camera::SetProjection(float left, float right, float bottom, float top, float near, float far)
  {
    m_ortho_matrix = FlexEngine::Matrix4x4::Orthographic(left, right, bottom, top, near, far);
  }

  // Since proj view matrix is cached, it rqeuires manual update
  void Camera::Update()
  {
    UpdateCameraMatrix();
  }
}