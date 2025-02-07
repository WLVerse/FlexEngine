// WLVERSE [https://wlverse.web.app]
// statisticspanellayer.h
//
// Statistics panel for debugging. 
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

  class StatisticsPanelLayer : public FlexEngine::Layer
  {
  public:
    StatisticsPanelLayer() : Layer("Statistics Panel Layer") {}
    ~StatisticsPanelLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}