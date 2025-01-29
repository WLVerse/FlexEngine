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
    : left{left}, right{ right }, bottom{ bottom }, top{ top }, near{ near }, far{ far }
  {
    this->SetProjection(left, right, bottom, top, near, far);
    UpdateCameraMatrix();
  }

  void Camera::SetProjection(float l, float r, float b, float t, float n, float f)
  {
    m_ortho_matrix = FlexEngine::Matrix4x4::Orthographic(l, r, b, t, n, f);
    left = l; right = r; bottom = b; top = t; near = n; far = f;
  }

  // Since proj view matrix is cached, it rqeuires manual update
  void Camera::Update()
  {
    UpdateCameraMatrix();
  }
}