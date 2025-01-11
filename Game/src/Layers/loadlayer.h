#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class LoadLayer : public FlexEngine::Layer
  {
  public:
    LoadLayer() : Layer("Load Layer") {}
    ~LoadLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
