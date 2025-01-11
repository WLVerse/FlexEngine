#include "Layers.h"

namespace Editor
{

  // internal variables
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
    // render splash screen
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
      Application::QueueCommand(
        Application::CommandData(
          Application::Command::Application_AddLayer,
          std::make_shared<BaseLayer>()
        )
      );
      added_base_layer = true;
    }

    // code here will not be executed until the base layer is loaded
    if (Application::MessagingSystem::Receive<bool>("Is base layer loaded?"))
    {
      #if 0
      Log::Info("Loaded base layer, switching to editor.");
      #else
      Application::QueueCommand(
        Application::CommandData(
          Application::Command::Application_AddLayer,
          std::make_shared<EditorWindowLayer>()
        )
      );
      Application::QueueCommand(
        Application::CommandData(
          Application::Command::Application_RemoveLayer,
          Application::GetLayerStack().GetLayer("Splash Window Layer")
        )
      );
      #endif
    }
  }

}
