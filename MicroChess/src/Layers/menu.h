#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace MicroChess
{

  class MenuLayer : public FlexEngine::Layer
  {
    std::vector<FlexECS::Entity> buttons;

  public:
    MenuLayer() : Layer("Menu Layer") {}
    ~MenuLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    void SetupMenu();
  };

}
