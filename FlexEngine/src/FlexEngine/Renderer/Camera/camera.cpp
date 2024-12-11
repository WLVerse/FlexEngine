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

#include "camera.h"

Camera::Camera(float left, float right, float bottom, float top)
{
  SetProjection(left, right, bottom, top);
}

void Camera::SetProjection(float left, float right, float bottom, float top)
{
  m_ortho_matrix = FlexEngine::Matrix4x4::Orthographic(left, right, bottom, top, -2.0f, 2.0f);
}