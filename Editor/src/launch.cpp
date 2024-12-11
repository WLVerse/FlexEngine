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

#include "States.h"

namespace Editor
{
  class FlexApplication : public FlexEngine::Application
  {
  public:
    FlexApplication()
    {
      FlexEngine::ApplicationStateManager::SetState<SplashState>();
    }
  };

}

namespace FlexEngine
{
  Application* CreateApplication()
  {
    return new Editor::FlexApplication();
  }
}
