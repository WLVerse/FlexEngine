#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class LoseLayer : public FlexEngine::Layer
  {
  public:
    LoseLayer() : Layer("Lose Layer") {}
    ~LoseLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
