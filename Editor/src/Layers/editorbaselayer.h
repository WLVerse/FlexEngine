// WLVERSE [https://wlverse.web.app]
// editorbaselayer.h
// 
// Base layer for the editor.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class EditorBaseLayer : public FlexEngine::Layer
  {
    const Path default_save_directory = Path::current("saves");
    const std::string default_save_name = "default";
    Path current_save_directory = default_save_directory;
    std::string current_save_name = default_save_name;

    ImGuiID dockspace_main_id = 0;

    FlexEngine::OpenGLFrameBufferManager* framebufferManager = nullptr;
  public:
    EditorBaseLayer() : Layer("Editor Base Layer") {}
    ~EditorBaseLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  };

}
