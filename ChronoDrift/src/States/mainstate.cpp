#include "mainstate.h"

#include "States.h"
#include "Layers.h"

namespace ChronoDrift
{

  void MainState::OnEnter()
  {
    window = Application::OpenWindow(
      {
        "Chrono Drift - FlexEngine",
        1280, 720 + 30, // 30 pixels for the title bar
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, false },
          { GLFW_RESIZABLE, false }
        }
      }
    );
    AlignImGuiContext(window);

    window->PushLayer(std::make_shared<BaseLayer>());
  }

  void MainState::OnExit()
  {
    Application::CloseWindow(window);
    window.reset();
  }

  void MainState::Update()
  {
    window->Update();
  }

}