// WLVERSE [https://wlverse.web.app]
// renderinglayer.h
// 
// Rendering layer for the editor.
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

  class RenderingLayer : public FlexEngine::Layer
  {
  public:
    RenderingLayer() : Layer("Rendering Layer") {}
    ~RenderingLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
