#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class WinLayer : public FlexEngine::Layer
  {
  public:
    WinLayer() : Layer("Win Layer") {}
    ~WinLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
