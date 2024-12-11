#include "States.h"
#include "Layers.h"

namespace Editor
{

  void SplashState::OnEnter()
  {
    window = Application::OpenWindow(
      {
        "FlexEngine Splash Screen",
        900, 350,
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, false },
          { GLFW_RESIZABLE, false }
        }
      }
    );
    AlignImGuiContext(window);

    window->SetTargetFPS(30);
    window->SetVSync(false);

    window->PushLayer(std::make_shared<BaseLayer>());
    window->PushLayer(std::make_shared<SplashLayer>());

    window->PushOverlay(std::make_shared<StatisticsPanelLayer>());
  }

  void SplashState::OnExit()
  {
    Application::CloseWindow(window);
    window.reset();
  }

  void SplashState::Update()
  {
    window->Update();
  }

}