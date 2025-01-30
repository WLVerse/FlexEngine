// WLVERSE [https://wlverse.web.app]
// renderinglayer.cpp
// 
// Rendering layer for the editor.
// 
// Very rough implementation of hotloading a rendering DLL.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

#include <Physics/physicssystem.h>

namespace Editor
{

  void RenderingLayer::OnAttach()
  {

  }

  void RenderingLayer::OnDetach()
  {

  }

  void RenderingLayer::Update()
  {
    static Camera camera(0.0f, 1600.0f, 900.0f, 0.0f, -2.0f, 2.0f);

    #pragma region Animator System

    // animator system processes the more complex data into usable data for the sprite renderer
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Animator>())
    {
      // always prefer reference when it's in the query itself,
      // or if it's a component that is guaranteed to exist
      // TODO: pls adopt and enforce
      Animator& animator = *element.GetComponent<Animator>();
      auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(animator.spritesheet_file));

      if (animator.should_play)
      {
        // TODO: reset time somewhere?
        animator.time += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      }

      int index = (int)(animator.time * asset_spritesheet.columns) % asset_spritesheet.columns;

      // Override sprite component
      Sprite* sprite = element.GetComponent<Sprite>();
      if (sprite != nullptr)
      {
        sprite->sprite_handle = animator.spritesheet_file;
        sprite->handle = index;
      }
      else Log::Fatal("Somehow, a animator exists without a sprite component attached to it. This should be impossible with editor creation.");
    }

    #pragma endregion

    #pragma region Sprite Renderer System

    // render all sprites
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite>())
    {
      Sprite* sprite = element.GetComponent<Sprite>();

      Renderer2DProps props;

      props.asset = FLX_STRING_GET(sprite->sprite_handle);
      props.texture_index = sprite->handle;
      props.position = Vector2(0.0f, 0.0f);
      props.scale = Vector2(384.0f / 8.f, 96.0f);
      //props.scale = Vector2(384.0f / asset_spritesheet.columns, 96.0f);

      const WindowProps& _wp = Application::GetCurrentWindow()->GetProps();
      props.window_size = Vector2((float)_wp.width, (float)_wp.height);

      props.alignment = Renderer2DProps::Alignment_TopLeft;

      OpenGLRenderer::DrawTexture2D(camera, props);
    }

    #pragma endregion

  }

}
