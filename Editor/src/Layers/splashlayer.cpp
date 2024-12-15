#include "States.h"
#include "Layers.h"

namespace Editor
{

  Renderer2DProps props;
  Camera camera(0.0f, 900.0f, 0.0f, 350.0f, -1.0f, 1.0f);
  Asset::Texture texture;

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

    texture.Load(Path::current("assets/images/flexengine/flexengine_splash.png"));
  }

  void SplashLayer::OnDetach()
  {

  }

  void SplashLayer::Update()
  {
    // render splash screen
    camera.Update();
    OpenGLRenderer::DrawTexture2D(camera, props);

    // imgui window display an image
    //ImGui::Begin("Splash Screen");
    //ImGui::Image(IMGUI_IMAGE(texture));
    //ImGui::End();
  }

}
