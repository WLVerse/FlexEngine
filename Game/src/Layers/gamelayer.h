#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class GameLayer : public FlexEngine::Layer
  {
  public:
    GameLayer() : Layer("Game Layer") {}
    ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
