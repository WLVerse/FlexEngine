// WLVERSE [https://wlverse.web.app]
// editorwindowlayer.cpp
// 
// Editor window management layer.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor
{

  void EditorWindowLayer::OnAttach()
  {
    FLX_COMMAND_OPEN_WINDOW(
      "Editor",
      WindowProps(
        "FlexEngine Editor",
        1600, 900,
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, true },
          { GLFW_RESIZABLE, true }
        }
      )
    );

    // This currently only stores the asset manager
    // It must be run after the window is created
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<BaseLayer>());

    // This is the base layer for the editor
    FLX_COMMAND_ADD_WINDOW_LAYER(
      "Editor",
      std::make_shared<EditorBaseLayer>()
    );

    // Scripting layer
    FLX_COMMAND_ADD_WINDOW_LAYER(
      "Editor",
      std::make_shared<ScriptingLayer>()
    );

    // Statistics panel for debugging
    FLX_COMMAND_ADD_WINDOW_LAYER(
      "Editor",
      std::make_shared<StatisticsPanelLayer>()
    );
  }

  void EditorWindowLayer::OnDetach()
  {
    FLX_COMMAND_CLOSE_WINDOW("Editor");
  }

  void EditorWindowLayer::Update()
  {
    Application::GetCurrentWindow()->Update();
  }

}
