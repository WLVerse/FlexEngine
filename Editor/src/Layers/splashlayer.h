#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class SplashLayer : public FlexEngine::Layer
  {
  public:
    SplashLayer() : Layer("Splash Layer") {}
    ~SplashLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
