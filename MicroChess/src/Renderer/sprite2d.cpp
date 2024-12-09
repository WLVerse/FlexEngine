#include "Renderer/sprite2d.h"

#include "Components/rendering.h"

namespace MicroChess
{

  void RendererSprite2D()
  {
    WindowProps window_props = Application::GetCurrentWindow()->GetProps();
    Renderer2DProps props;
    props.window_size = { static_cast<float>(window_props.width), static_cast<float>(window_props.height) };

    FunctionQueue render_queue;

    // Render all entities
    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, ZIndex, Position, Scale, Shader, Sprite>())
    {
      if (!entity.GetComponent<IsActive>()->is_active) continue;

      Vector2 global_position = Vector2::Zero;
      Vector2 global_scale = Vector2::One;

      if (entity.HasComponent<Parent>())
      {
        auto& parent = entity.GetComponent<Parent>()->parent;
        if (parent.HasComponent<Position>())
        {
          global_position = parent.GetComponent<Position>()->position;
        }
        if (parent.HasComponent<Scale>())
        {
          global_scale = parent.GetComponent<Scale>()->scale;
        }
      }

      auto& z_index = entity.GetComponent<ZIndex>()->z;
      auto& position = entity.GetComponent<Position>()->position;
      auto& scale = entity.GetComponent<Scale>()->scale;
      auto& shader = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Shader>()->shader);
      auto sprite = entity.GetComponent<Sprite>();

      props.shader = shader;
      props.position = global_position + position;
      props.scale = global_scale * scale;
      props.texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(sprite->texture);
      props.color = sprite->color;
      props.color_to_add = sprite->color_to_add;
      props.color_to_multiply = sprite->color_to_multiply;
      props.alignment = static_cast<Renderer2DProps::Alignment>(sprite->alignment);

      render_queue.Insert({ [props]() { OpenGLRenderer::DrawTexture2D(props); }, "", z_index });
    }

    // push settings

    bool depth_test = OpenGLRenderer::IsDepthTestEnabled();
    if (depth_test) OpenGLRenderer::DisableDepthTest();

    bool blending = OpenGLRenderer::IsBlendingEnabled();
    if (!blending) OpenGLRenderer::EnableBlending();

    // render

    render_queue.Flush();

    // pop settings

    if (depth_test) OpenGLRenderer::EnableDepthTest();
    if (!blending) OpenGLRenderer::DisableBlending();
  }

}