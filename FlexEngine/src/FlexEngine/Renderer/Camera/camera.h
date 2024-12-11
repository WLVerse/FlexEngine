// WLVERSE [https://wlverse.web.app] 
// camera.h
// 
// Class declaration for interfacing with the camera.
// 
// AUTHORS 
// [100%] Yew Chong (yewchong.k\@digipen.edu) 
// - Main Author 
// 
// Copyright (c) 2024 DigiPen, All rights reserved

#include "FlexEngine/FlexMath/matrix4x4.h"

class Camera
{
  // expected result of computation is projection * view in this order.
  FlexEngine::Matrix4x4 m_ortho_matrix = FlexEngine::Matrix4x4::Identity; // There is no projection matrix as this is a 2D camera, we only have orthographic.
  FlexEngine::Matrix4x4 m_view_matrix = {  1, 0,  0, 0,
                                           0, 1,  0, 0,
                                           0, 0, -1, 0,    // Flip the Z index using openGL's right-handed coordinate system
                                           0, 0,  0, 1 };  

public:
  Camera(float left, float right, float bottom, float top);
  ~Camera() = default;

  void SetProjection(float left, float right, float bottom, float top);
  const FlexEngine::Matrix4x4& GetProjectionMatrix() const { return m_ortho_matrix; }
  const FlexEngine::Matrix4x4& GetProjViewMatrix() const { return m_ortho_matrix * m_view_matrix; }
  const FlexEngine::Matrix4x4& GetViewMatrix() const { return m_view_matrix; }
};

// Temporary camera static for testing
static Camera main_camera(0.0f, 1280.f, 750.f, 0.0f);