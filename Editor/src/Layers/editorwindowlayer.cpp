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
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::OpenWindow,
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
      )
    );

    // This currently only stores the asset manager
    // It must be run after the window is created
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Application_AddLayer,
        std::make_shared<BaseLayer>()
      )
    );

    // This is the base layer for the editor
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Window_AddLayer,
        "Editor",
        std::make_shared<EditorBaseLayer>()
      )
    );

    // Scripting layer
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Window_AddLayer,
        "Editor",
        std::make_shared<ScriptingLayer>()
      )
    );

    // Statistics panel for debugging
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Window_AddOverlay,
        "Editor",
        std::make_shared<StatisticsPanelLayer>()
      )
    );
  }

  void EditorWindowLayer::OnDetach()
  {
    Application::CloseWindow("Editor");
  }

  void EditorWindowLayer::Update()
  {
    Application::GetCurrentWindow()->Update();
  }

}
