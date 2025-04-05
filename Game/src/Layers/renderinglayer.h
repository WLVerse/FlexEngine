// WLVERSE [https://wlverse.web.app]
// renderinglayer.h
//
// Rendering layer for the game.
// Handles transformation updates, animation, video playback, sprite/text rendering, batching,
// and both local and global post-processing.
// Implements hotloading of the rendering DLL for live updates.
//
// AUTHORS
// [25%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author (Animator && framework)
// [45%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Sub Author (PP & Batching)
// [30%] Rocky (rocky.sutarius\@digipen.edu)
//   - Sub Author (Video Player)
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
#include "postprocessing.h"

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
