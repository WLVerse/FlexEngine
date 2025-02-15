#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
  class AssetLayer : public FlexEngine::Layer
  {
  public:
    AssetLayer() : Layer("Load Layer") {}
    ~AssetLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
