#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoDrift
{

  class MainState : public FlexEngine::State
  {
    std::shared_ptr<FlexEngine::Window> window;

  public:
    MainState() = default;
    ~MainState() = default;

    void OnEnter() override;
    void Update() override;
    void OnExit() override;
  };

}