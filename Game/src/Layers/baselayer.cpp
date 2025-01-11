#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void BaseLayer::OnAttach()
  {
    // First, create a window
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::OpenWindow,
        "Game",
        WindowProps(
          "Chrono Drift",
          1600, 900,
          {
            FLX_DEFAULT_WINDOW_HINTS,
            { GLFW_DECORATED, true },
            { GLFW_RESIZABLE, true }
          }
        )
      )
    );

    // Second, load assets
    Application::QueueCommand(
      Application::CommandData(
        Application::Command::Application_AddLayer, "Load Layer", std::make_shared<LoadLayer>()
      )
    );
  }

  void BaseLayer::OnDetach()
  {
  }

  void BaseLayer::Update()
  {
    std::cout << "Update" << std::endl;
    if (Input::GetKeyDown(GLFW_KEY_1))
    {
      FMODWrapper::Core::PlaySound("attack", FLX_ASSET_GET(Asset::Sound, AssetKey("/audio/attack.mp3")));
    }
  }
}
