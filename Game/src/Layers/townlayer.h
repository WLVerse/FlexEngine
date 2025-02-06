// WLVERSE [https://wlverse.web.app]
// townlayer.h
// 
// Header file for town layer where user can control main character to walk around the town scene
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once
#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{

  class TownLayer : public FlexEngine::Layer
  {
  private:
    FlexECS::Entity main_character;
  public:
    TownLayer() : Layer("Town Scene Layer") {}
    ~TownLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  };

}