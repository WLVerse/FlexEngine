#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void BaseLayer::OnAttach()
  {
    // First, create a window
    FLX_COMMAND_OPEN_WINDOW(
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
    );

    // Second, load assets
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<LoadLayer>());
  }

  void BaseLayer::OnDetach()
  {
    FLX_COMMAND_CLOSE_WINDOW("Game");
  }

  void BaseLayer::Update()
  {
    if (Input::GetKeyDown(GLFW_KEY_1))
    {
      FMODWrapper::Core::PlaySound("attack", FLX_ASSET_GET(Asset::Sound, AssetKey("/audio/attack.mp3")));
    }
  }
}
