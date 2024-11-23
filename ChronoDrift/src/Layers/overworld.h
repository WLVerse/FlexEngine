#pragma once
#include <FlexEngine.h>

#include "BattleSystems/battlesystem.h"
#include "CharacterMoves/moves.h"
using namespace FlexEngine;

namespace ChronoDrift
{
  class OverworldLayer : public FlexEngine::Layer
  {
  public:
    OverworldLayer() : Layer("Battle Layer") {}
    ~OverworldLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    void SetupWorld();

    static float m_ScaleDebugTest;
    static Vector3 m_RotateDebugTest;

  };



}
