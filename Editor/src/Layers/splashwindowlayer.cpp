// WLVERSE [https://wlverse.web.app]
// splashwindowlayer.cpp
// 
// Splash window management layer.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor
{

  void SplashWindowLayer::OnAttach()
  {
    FLX_COMMAND_OPEN_WINDOW(
      "Splash",
      WindowProps(
        "FlexEngine Splash",
        900, 350,
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, false },
          { GLFW_RESIZABLE, false }
        }
      )
    );

    FLX_COMMAND_ADD_WINDOW_LAYER(
      "Splash",
      std::make_shared<SplashLayer>()
    );
  }

  void SplashWindowLayer::OnDetach()
  {
    FLX_COMMAND_CLOSE_WINDOW("Splash");
  }

  void SplashWindowLayer::Update()
  {
    Application::GetCurrentWindow()->Update();
  }

}
