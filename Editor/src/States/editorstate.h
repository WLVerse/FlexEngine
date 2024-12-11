#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class EditorState : public FlexEngine::State
  {
    std::shared_ptr<FlexEngine::Window> window;

  public:
    EditorState() = default;
    ~EditorState() = default;

    void OnEnter() override;
    void Update() override;
    void OnExit() override;
  };

}