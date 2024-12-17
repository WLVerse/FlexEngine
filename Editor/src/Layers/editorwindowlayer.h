#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class EditorWindowLayer : public FlexEngine::Layer
  {
  public:
    EditorWindowLayer() : Layer("Editor Window Layer") {}
    ~EditorWindowLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
