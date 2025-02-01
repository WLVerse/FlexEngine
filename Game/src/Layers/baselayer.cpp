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
    
    // Third, add the engine behavior layers
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<RenderingLayer>());
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<PhysicsLayer>());
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<AudioLayer>());
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<ScriptingLayer>());
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<GameLayer>());
  }

  void BaseLayer::OnDetach()
  {
    FLX_COMMAND_CLOSE_WINDOW("Game");
  }

  void BaseLayer::Update()
  {

  }
}
