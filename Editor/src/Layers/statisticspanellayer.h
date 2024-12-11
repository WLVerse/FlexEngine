#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class StatisticsPanelLayer : public FlexEngine::Layer
  {
  public:
    StatisticsPanelLayer() : Layer("Statistics Panel Layer") {}
    ~StatisticsPanelLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
