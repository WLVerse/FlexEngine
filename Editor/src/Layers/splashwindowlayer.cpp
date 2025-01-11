#include "Layers.h"

namespace Editor
{

  void SplashWindowLayer::OnAttach()
  {
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::OpenWindow,
        "Splash Screen",
        WindowProps(
          "FlexEngine Splash Screen",
          900, 350,
          {
            FLX_DEFAULT_WINDOW_HINTS,
            { GLFW_DECORATED, false },
            { GLFW_RESIZABLE, false }
          }
        )
      )
    );

    // display splash screen first
    // note that this layer does not rely on anything in the base layer
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Window_AddLayer,
        "Splash Screen",
        std::make_shared<SplashLayer>()
      )
    );

    // this is just for debugging
    //Application::QueueCommand(
    //  Application::CommandData(
    //    Application::Command::Window_AddOverlay,
    //    "Splash Screen",
    //    std::make_shared<StatisticsPanelLayer>()
    //  )
    //);
  }

  void SplashWindowLayer::OnDetach()
  {
    Application::CloseWindow("Splash Screen");
  }

  void SplashWindowLayer::Update()
  {
    Application::GetCurrentWindow()->Update();
  }

}
