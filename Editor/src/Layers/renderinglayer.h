// WLVERSE [https://wlverse.web.app]
// renderinglayer.h
//
// Rendering layer for the editor.
//
// Implements the main renderers for the editor and game.
// This layer loop runs in this order
// 1. Transformation Calculations
// 2. Update Animator System (ie delta time)
// 3. Sprite Renderer System (Sprites and Animations included)
// 4. Text Renderer System
// 5. PP System
// The framebuffers used are "scene" and "game", and nothing is rendered to the default framebuffer. (Editor property)
//
// AUTHORS
// [60%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// [10%] Wen Loong (wenloong.l\@digipen.edu)
//   - Added animation rendering
//
// [30%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Added batching && PP
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

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

    void AddBatchToQueue(FunctionQueue& queue, const std::string& texture, const Renderer2DSpriteBatch& batch, bool isEditor);
    void AddEntityToBatch(FlexECS::Entity& entity, Renderer2DSpriteBatch& batch);
  };

}