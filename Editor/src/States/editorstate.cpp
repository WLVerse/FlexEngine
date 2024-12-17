#include "States.h"
#include "Layers.h"

namespace Editor
{

  void EditorState::OnEnter()
  {
    FLX_FLOW_BEGINSCOPE();

    window = Application::OpenWindow(
      {
        "FlexEngine Editor",
        1600, 900,// + 30, // 30 pixels for the title bar
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, true },
          { GLFW_RESIZABLE, true }
        }
      }
    );
    AlignImGuiContext(window);

    window->SetTargetFPS();
    window->SetVSync(false);

    //auto& layerstack = window->GetLayerStack();
    //
    //layerstack.AddLayer(std::make_shared<EditorBaseLayer>());
    //layerstack.AddLayer(std::make_shared<ScriptingLayer>());
    //
    //layerstack.AddOverlay(std::make_shared<StatisticsPanelLayer>());

    ApplicationStateManager::SetState<TestState>();
  }

  void EditorState::OnExit()
  {
    Application::CloseWindow(window);
    window.reset();

    FLX_FLOW_ENDSCOPE();
  }

  void EditorState::Update()
  {
    window->Update();
  }

}