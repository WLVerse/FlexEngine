#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    extern std::string file_name;

  class BaseLayer : public FlexEngine::Layer
  {
  public:
    BaseLayer() : Layer("Base Layer") {}
    ~BaseLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
