// WLVERSE [https://wlverse.web.app]
// physicslayer.h
//
// Physics layer for the editor. 
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

  class PhysicsLayer : public FlexEngine::Layer
  {
  public:
    PhysicsLayer() : Layer("Physics Layer") {}
    ~PhysicsLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}