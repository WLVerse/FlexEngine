#pragma once

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
