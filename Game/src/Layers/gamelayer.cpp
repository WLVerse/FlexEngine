#include "Layers.h"

namespace Game
{
  void GameLayer::OnAttach()
  {
  }

  void GameLayer::OnDetach()
  {
  }

  void GameLayer::Update()
  {
#pragma region Scene Swapper System

    std::string scene_name = Application::MessagingSystem::Receive<std::string>("change scene to");
    if (scene_name != std::string())
    {
      LayerStack& ls = Application::GetCurrentWindow()->GetLayerStack();
      if (ls.GetLayer("Menu Layer")) FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", ls.GetLayer("Menu Layer"));
      if (ls.GetLayer("Town Layer")) FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", ls.GetLayer("Town Layer"));
      //if (ls.GetLayer("Battle Layer")) FLX_COMMAND_REMOVE_WINDOW_LAYER("Game", ls.GetLayer("Battle Layer"));

      if (scene_name == "Menu")
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<MenuLayer>());
      /*else if (scene_name == "Town")
        FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<TownLayer>());*/
      // 
  
      // else if (scene_name == "Battle")
      // FLX_COMMAND_ADD_WINDOW_LAYER("Game", std::make_shared<BattleLayer>());
    }

#pragma endregion
  }
} // namespace Game
