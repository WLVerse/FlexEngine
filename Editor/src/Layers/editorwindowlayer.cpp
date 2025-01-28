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
    // This is the main window.
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

    // This is the base layer for the editor
    //FLX_COMMAND_ADD_WINDOW_LAYER(
    //  "Editor",
    //  std::make_shared<EditorBaseLayer>()
    //);

    // Splash window
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<SplashWindowLayer>());
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
