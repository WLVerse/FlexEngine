#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class SplashState : public FlexEngine::State
  {
    std::shared_ptr<FlexEngine::Window> window;

  public:
    SplashState() = default;
    ~SplashState() = default;

    void OnEnter() override;
    void Update() override;
    void OnExit() override;
  };

}