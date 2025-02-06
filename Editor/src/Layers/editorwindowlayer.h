// WLVERSE [https://wlverse.web.app]
// editorwindowlayer.h
//
// Editor window management layer. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

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