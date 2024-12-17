#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class SplashWindowLayer : public FlexEngine::Layer
  {
  public:
    SplashWindowLayer() : Layer("Splash Window Layer") {}
    ~SplashWindowLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
