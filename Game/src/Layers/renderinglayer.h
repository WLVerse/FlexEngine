// WLVERSE [https://wlverse.web.app]
// renderinglayer.h
// 
// Rendering layer for the game.
//
// AUTHORS
// [50%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// [50%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Sub Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{

  class RenderingLayer : public FlexEngine::Layer
  {
  public:
    RenderingLayer() : Layer("Rendering Layer") {}
    ~RenderingLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

    void AddBatchToQueue(FunctionQueue& queue, const std::string& texture, const Renderer2DSpriteBatch& batch);
    void AddEntityToBatch(FlexECS::Entity& entity, Renderer2DSpriteBatch& batch);
  };

}
