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

namespace Editor
{
  class FlexApplication : public FlexEngine::Application
  {
  public:
    FlexApplication()
    {
      FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<SplashWindowLayer>());

      // Add the editor window layer which manages the editor window
      //FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<EditorWindowLayer>());
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
