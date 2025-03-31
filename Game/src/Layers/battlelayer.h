// WLVERSE [https://wlverse.web.app]
// audiolayer.h
//
// Audio layer for the editor.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class BattleLayer : public FlexEngine::Layer
  {
  public:
    BattleLayer()
      : Layer("Battle Layer")
    {
    }

    ~BattleLayer() = default;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

    void UpdateCombatCamera();
  };

} // namespace Game
