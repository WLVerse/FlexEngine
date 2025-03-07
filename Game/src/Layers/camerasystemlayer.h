// WLVERSE [https://wlverse.web.app]
// camerasystemlayer.h / cameraservicelayer.h
// 
// Layer that handles the updates to validating cameras to update in CamManager
//
// AUTHORS
// [100%] Soh Wei Jie (weijie_soh.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{

  class CameraSystemLayer : public FlexEngine::Layer
  {
  public:
    CameraSystemLayer();
    ~CameraSystemLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
      // Main camera entity.
      FlexECS::Entity m_mainCameraEntity;

      // Camera Shake effect variables.
      bool m_shakeActive = false;
      float m_shakeDuration = 0.0f;
      float m_shakeElapsed = 0.0f;
      float m_shakeIntensity = 0.0f;
      Vector3 m_originalCameraPos;

      // Camera Zoom effect variables.
      bool m_zoomActive = false;
      float m_zoomDuration = 0.0f;
      float m_zoomElapsed = 0.0f;
      float m_zoomTarget = 0.0f;
      float m_zoomInitial = 0.0f;

      const float m_baseAspectRatio = 16.0f / 9.0f;
      const float m_minOrthoWidth = 10.0f;         
  };

}