#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class SplashScreenLayer : public FlexEngine::Layer
  {
  public:
    SplashScreenLayer() : Layer("Splash Screen Layer") {}
    ~SplashScreenLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
