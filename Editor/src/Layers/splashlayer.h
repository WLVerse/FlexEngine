// WLVERSE [https://wlverse.web.app]
// splashlayer.h
// 
// Base layer for the splash screen.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class SplashLayer : public FlexEngine::Layer
  {
  public:
    SplashLayer() : Layer("Splash Layer") {}
    ~SplashLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  };

}
