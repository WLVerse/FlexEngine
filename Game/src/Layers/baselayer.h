#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
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
