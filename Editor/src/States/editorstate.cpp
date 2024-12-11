#include "States.h"
#include "Layers.h"

namespace Editor
{

  void EditorState::OnEnter()
  {
    window = Application::OpenWindow(
      {
        "FlexEngine - Editor by Wen Loong",
        1280, 720,// + 30, // 30 pixels for the title bar
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, true },
          { GLFW_RESIZABLE, false }
        }
      }
    );
    AlignImGuiContext(window);

    //window->PushLayer(std::make_shared<EditorLayer>());
  }

  void EditorState::OnExit()
  {
    Application::CloseWindow(window);
    window.reset();
  }

  void EditorState::Update()
  {
    window->Update();
  }

}