#include "States.h"
#include "Layers.h"

namespace Editor
{

  Renderer2DProps props;

  void SplashLayer::OnAttach()
  {
    //props.shader = R"(/shaders/texture)";
    props.texture = R"(/images/flexengine/flexengine_splash.png)";
    //props.color = Vector3(1.0f, 0.0f, 1.0f);
    //props.color_to_add = Vector3(0.0f, 0.0f, 0.0f);
    //props.color_to_multiply = Vector3(1.0f, 1.0f, 1.0f);
    //props.position = Vector2(0.0f, 0.0f);
    props.scale = { 900.0f, 350.0f };
    props.window_size = Vector2(900.0f, 350.0f);
    props.alignment = Renderer2DProps::Alignment_TopLeft;
  }

  void SplashLayer::OnDetach()
  {

  }

  void SplashLayer::Update()
  {
    // render splash screen
    OpenGLRenderer::DrawTexture2D(props);
  }

}
