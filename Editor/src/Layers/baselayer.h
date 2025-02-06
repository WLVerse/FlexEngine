// WLVERSE [https://wlverse.web.app]
// baselayer.h
//
// Base layer that runs before everything. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class BaseLayer : public FlexEngine::Layer
  {
  public:
    BaseLayer() : Layer("Base Layer") {}
    ~BaseLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}