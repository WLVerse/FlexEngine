// WLVERSE [https://wlverse.web.app]
// launch.cpp
// 
// Launches the application.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include <FlexEngine.h>
#include <entrypoint.h>

#include "Layers.h"

namespace Game
{
  class FlexApplication : public FlexEngine::Application
  {
  public:
    FlexApplication()
    {
      FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<BaseLayer>());
    }
  };

}

namespace FlexEngine
{
  Application* CreateApplication()
  {
    return new Game::FlexApplication();
  }
}
