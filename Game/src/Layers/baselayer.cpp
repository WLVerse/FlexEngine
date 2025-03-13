// WLVERSE [https://wlverse.web.app]
// battlelayer.cpp
//
// base layer for the game.
//
// AUTHORS
// [40%] Ng Jia Cheng (n.jiacheng\@digipen.edu)
//   - Co Author
// Copyright (c) 2025 DigiPen, All rights reserved.
#include "FlexEngine.h"
#include "Layers.h"

namespace Game
{
   std::string file_name = "";
   std::string town_version = "";
  std::shared_ptr<GameLayer> gameLayer = nullptr;
  std::shared_ptr<CutsceneLayer> cutsceneLayer = nullptr;
  std::shared_ptr<MenuLayer> menuLayer = nullptr;
  std::shared_ptr<TownLayer> townLayer = nullptr;
  std::shared_ptr<BattleLayer> battleLayer = nullptr;

  std::shared_ptr<CameraSystemLayer> camSystemLayer = nullptr;

  void BaseLayer::OnAttach()
  {
    // First, create a window
    FLX_COMMAND_OPEN_WINDOW(
      "Game", WindowProps(
                "Chrono Drift", 1900, 1080,
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
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<PhysicsLayer>());
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<RenderingLayer>());
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<AudioLayer>());
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<ScriptingLayer>());
// Start with the menu layer
#if 1
    menuLayer = std::make_shared<MenuLayer>();
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", menuLayer);
#else
    FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<BattleLayer>());
#endif

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
      menuLayer = nullptr;

      cutsceneLayer = std::make_shared<CutsceneLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", cutsceneLayer);
    }

    // Cutscene to tutorial
    if (Application::MessagingSystem::Receive<bool>("Start Game") && cutsceneLayer != nullptr)
    {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", cutsceneLayer);
      cutsceneLayer = nullptr;
      
      file_name = "/data/tutorial_battle.flxbattle";
      battleLayer = std::make_shared<BattleLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", battleLayer);
    }

    // Game Lose to tutorial
    if (Application::MessagingSystem::Receive<bool>("Tutorial lose to Tutorial") && battleLayer != nullptr)
    {
        FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
        battleLayer = nullptr;

        file_name = "/data/tutorial_battle.flxbattle";
        battleLayer = std::make_shared<BattleLayer>();
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", battleLayer);
    }

    // Tutorial to town
    if (Application::MessagingSystem::Receive<bool>("Tutorial win to Town") && battleLayer != nullptr)
    {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
      battleLayer = nullptr;
      
      town_version = "assets/saves/town_v4.flxscene";
      townLayer = std::make_shared<TownLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", townLayer);
    }

    // Town to first encounter
    if (Application::MessagingSystem::Receive<bool>("Enter Battle 1") && townLayer != nullptr)
    {
        FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", townLayer);
        townLayer = nullptr;

        file_name = "/data/robot_battle.flxbattle";
        battleLayer = std::make_shared<BattleLayer>();
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", battleLayer);
    }

    // first encounter lose to first encounter again
    if (Application::MessagingSystem::Receive<bool>("Battle 1 lose to Battle 1") && battleLayer != nullptr)
    {
        FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
        battleLayer = nullptr;

        file_name = "/data/robot_battle.flxbattle";
        battleLayer = std::make_shared<BattleLayer>();
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", battleLayer);
    }

    // First encounter to town
    if (Application::MessagingSystem::Receive<bool>("Battle 1 win to Town") && battleLayer != nullptr)
    {
        FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
        battleLayer = nullptr;

        town_version = "assets/saves/town_v4_2.flxscene";
        townLayer = std::make_shared<TownLayer>();
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", townLayer);
    }

    // Town to Boss
    if (Application::MessagingSystem::Receive<bool>("Enter Boss") && townLayer != nullptr)
    {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", townLayer);
      townLayer = nullptr;

      file_name = "/data/boss_battle.flxbattle"; //enables check for townlayer upon win
      battleLayer = std::make_shared<BattleLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", battleLayer);
    }

    // boss lose to boss again
    if (Application::MessagingSystem::Receive<bool>("Battle Boss lose to Battle Boss") && battleLayer != nullptr)
    {
        FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
        battleLayer = nullptr;

        file_name = "/data/boss_battle.flxbattle"; //enables check for townlayer upon win
        battleLayer = std::make_shared<BattleLayer>();
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", battleLayer);
    }

    // boss to menu
    if (Application::MessagingSystem::Receive<bool>("Battle Boss win to Menu") && battleLayer != nullptr)
    {
        FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
        battleLayer = nullptr;

        menuLayer = std::make_shared<MenuLayer>();
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", menuLayer);
    }

    #if 0
    if (Application::MessagingSystem::Receive<bool>("Game win to menu"))
    {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
      battleLayer = nullptr;

      menuLayer = std::make_shared<MenuLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", menuLayer);
    }

    if (Application::MessagingSystem::Receive<bool>("Game lose to menu"))
    {
      FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", battleLayer);
      battleLayer = nullptr;

      menuLayer = std::make_shared<MenuLayer>();
      FLX_COMMAND_ADD_WINDOW_LAYER("Game", menuLayer);
    }
    #endif

    #if 0
    if (Input::GetKeyDown(GLFW_KEY_L))
        Application::MessagingSystem::Send("CameraShakeStart", std::pair<double, double>{ 2, 10.0 });
    if (Input::GetKeyDown(GLFW_KEY_P))
        Application::MessagingSystem::Send("CameraShakeLerpStart", std::pair<double, double>{ 2, 10.0 });
    if (Input::GetKeyDown(GLFW_KEY_O))
        Application::MessagingSystem::Send("CameraZoomAutoReturnStart", std::tuple<double, double, double>{ 1, 2.0, rand() % 800 + 1000 });
    if (Input::GetKeyDown(GLFW_KEY_U))
        Application::MessagingSystem::Send("CameraZoomStart", std::pair<double, double>{ 2, rand() % 800 + 1000 });
    #endif
  }
} // namespace Game
