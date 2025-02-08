#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  std::shared_ptr<GameLayer> gameLayer = nullptr;
  std::shared_ptr<CutsceneLayer> cutsceneLayer = nullptr;
  std::shared_ptr<MenuLayer> menuLayer = nullptr;
  std::shared_ptr<TownLayer> townLayer = nullptr;

  std::shared_ptr<CameraSystemLayer> camSystemLayer = nullptr;

  void BaseLayer::OnAttach()
  {
    // First, create a window
    FLX_COMMAND_OPEN_WINDOW(
      "Game",
      WindowProps(
        "Chrono Drift",
        1900, 1080,
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, true },
          { GLFW_RESIZABLE, true }
        }
      )
    );

    // Second, load assets
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<AssetLayer>());
    
    // Third, add the engine behavior layers
    FLX_COMMAND_ADD_WINDOW_LAYER("Game",std::make_shared<PhysicsLayer>());
    FLX_COMMAND_ADD_WINDOW_LAYER("Game",std::make_shared<RenderingLayer>());
    FLX_COMMAND_ADD_WINDOW_LAYER("Game",std::make_shared<AudioLayer>());
    FLX_COMMAND_ADD_WINDOW_LAYER("Game",std::make_shared<ScriptingLayer>());

    // Start with the menu layer
    //menuLayer = std::make_shared<MenuLayer>();
    //FLX_COMMAND_ADD_WINDOW_LAYER("Game", menuLayer);
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<BattleLayer>());

    // Camera system goes last to capture the loaded scene
    camSystemLayer = std::make_shared<CameraSystemLayer>();
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", camSystemLayer);
  }

  void BaseLayer::OnDetach()
  {
    FLX_COMMAND_CLOSE_WINDOW("Game");
  }

  void BaseLayer::Update()
  {
    Application::GetCurrentWindow()->Update();

    // Test to switch to cutscene layer
    if (Application::MessagingSystem::Receive<bool>("Start Cutscene") && menuLayer != nullptr)
    {
        FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", menuLayer);
        //camSystemLayer->UnregisterCams();
        menuLayer = nullptr;

        cutsceneLayer = std::make_shared<CutsceneLayer>();
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", cutsceneLayer);
        //camSystemLayer->RegisterCams();
    }

    // Test to switch to town layer
    if (Application::MessagingSystem::Receive<bool>("Start Game") && cutsceneLayer != nullptr)
    {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", cutsceneLayer);
      cutsceneLayer = nullptr;
      menuLayer = nullptr;

      townLayer = std::make_shared<TownLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", townLayer);
    }

    // Town to Menu layer
    if (Application::MessagingSystem::Receive<bool>("Enter Battle") && townLayer != nullptr) {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", townLayer);
      //camSystemLayer->UnregisterCams();
      townLayer = nullptr;

      menuLayer = std::make_shared<MenuLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", menuLayer);
    }

    // Game to menu layer
    if (Input::GetKeyDown(GLFW_KEY_ESCAPE) && gameLayer != nullptr)
    {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", gameLayer);
      //camSystemLayer->UnregisterCams();
      gameLayer = nullptr;

      menuLayer = std::make_shared<MenuLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", menuLayer);
      //camSystemLayer->RegisterCams();
    }
  }
}
