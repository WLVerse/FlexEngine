// WLVERSE [https://wlverse.web.app]
// 
// .h
// 
// Base layer for the splash screen.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor
{

  Asset::Texture texture, icon;

  void SplashLayer::OnAttach()
  {
    // load assets
    texture.Load(Path::current("assets/images/flexengine/flexengine_splash.png"));

    // set window icon
    icon.Load(Path::current("assets/images/flexengine/flexengine-256.png"));
    Application::GetCurrentWindow()->SetIcon(icon);
  }

  void SplashLayer::OnDetach()
  {
  }

  void SplashLayer::Update()
  {
    // render the splash screen
    OpenGLRenderer::DrawSimpleTexture2D(
      texture,
      Vector2(0.0f, 0.0f),
      Vector2(900.0f, 350.0f),
      Vector2(900.0f, 350.0f),
      Renderer2DProps::Alignment_TopLeft
    );

    // load the base layer
    // the only reason why this is here is so that the render command above can run first
    static bool added_base_layer = false;
    if (!added_base_layer)
    {
      FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<BaseLayer>());
      added_base_layer = true;
    }

    // code here will not be executed until the base layer is loaded
    if (Application::MessagingSystem::Receive<bool>("Is base layer loaded?"))
    {
      Log::Debug("Base layer loaded! Loading EditorBaseLayer");
      FLX_COMMAND_ADD_WINDOW_LAYER(
        "Editor",
        std::make_shared<EditorBaseLayer>()
      );

      // Scripting layer
      //FLX_COMMAND_ADD_WINDOW_LAYER(
      //  "Editor",
      //  std::make_shared<ScriptingLayer>()
      //);

      // Statistics panel for debugging
      FLX_COMMAND_ADD_WINDOW_LAYER(
        "Editor",
        std::make_shared<StatisticsPanelLayer>()
      );
      
      // Remove the splash window layer, which will also close the window
      //FLX_COMMAND_REMOVE_APPLICATION_LAYER(Application::GetLayerStack().GetLayer("Splash Window Layer"));
    }
  }

}
