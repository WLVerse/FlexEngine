#pragma once
#include <FlexEngine.h>
#include "Scripting/ScriptingSystem.h"

using namespace FlexEngine;

namespace ChronoDrift
{
  class ScriptingLayer : public FlexEngine::Layer
  {
  public:
    ScriptingLayer() : Layer("Scripting Layer") {}
    ~ScriptingLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    ScriptingSystem scriptingSystem;
  };
}
