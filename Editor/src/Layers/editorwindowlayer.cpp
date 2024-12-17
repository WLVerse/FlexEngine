#include "States.h"
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
          1600, 900,// + 30, // 30 pixels for the title bar
          {
            FLX_DEFAULT_WINDOW_HINTS,
            { GLFW_DECORATED, true },
            { GLFW_RESIZABLE, true }
          }
        )
      )
    );

    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Window_AddLayer,
        "Editor",
        std::make_shared<EditorBaseLayer>()
      )
    );
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Window_AddLayer,
        "Editor",
        std::make_shared<ScriptingLayer>()
      )
    );
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Window_AddOverlay,
        "Editor",
        std::make_shared<StatisticsPanelLayer>()
      )
    );

    //Application::GetCurrentWindow()->GetLayerStack().AddLayer(std::make_shared<EditorBaseLayer>());
    //Application::GetCurrentWindow()->GetLayerStack().AddLayer(std::make_shared<ScriptingLayer>());
    //Application::GetCurrentWindow()->GetLayerStack().AddOverlay(std::make_shared<StatisticsPanelLayer>());
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
