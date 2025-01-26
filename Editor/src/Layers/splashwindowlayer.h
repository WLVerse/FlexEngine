// WLVERSE [https://wlverse.web.app]
// splashwindowlayer.h
// 
// Splash window management layer.
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

  class SplashWindowLayer : public FlexEngine::Layer
  {
  public:
    SplashWindowLayer() : Layer("Splash Window Layer") {}
    ~SplashWindowLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
