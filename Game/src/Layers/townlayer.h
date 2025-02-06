#pragma once
#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{

  class TownLayer : public FlexEngine::Layer
  {
  private:
    FlexECS::Entity main_character;
  public:
    TownLayer() : Layer("Town Scene Layer") {}
    ~TownLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  };

}