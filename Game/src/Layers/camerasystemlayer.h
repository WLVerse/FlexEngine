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
    CameraSystemLayer() : Layer("Camera System Layer") {}
    ~CameraSystemLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
