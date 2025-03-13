#pragma once
/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// menulayer.h / .cpp
//
// Declares the MenuLayer class, which is responsible for managing the
// main menu interface, including UI initialization, input handling,
// and transition between menu states.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class MenuLayer : public FlexEngine::Layer
  {
  public:
    MenuLayer() : Layer("Menu Layer") {}
    ~MenuLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
