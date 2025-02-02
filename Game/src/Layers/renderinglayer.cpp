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

namespace Game
{
  void RenderingLayer::OnAttach()
  {
    OpenGLRenderer::EnableBlending();
  }

  void RenderingLayer::OnDetach()
  {
    OpenGLRenderer::DisableBlending();
  }
  
  void RenderingLayer::Update()
  {
#pragma region Animator System

    // animator system updates the time for all animators
    // TODO: move this to a different layer
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Animator, Sprite>())
    {
      Animator& animator = *element.GetComponent<Animator>();

      if (animator.should_play)
      {
        // TODO: reset time somewhere?
        animator.time += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      }
    }

#pragma endregion

#pragma region Sprite Renderer System

    // render all sprites
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, Position, Rotation, Scale>())
    {
      Sprite& sprite = *element.GetComponent<Sprite>();
      Position& pos = *element.GetComponent<Position>();
      Rotation& rotation = *element.GetComponent<Rotation>();
      Scale& scale = *element.GetComponent<Scale>();

      Renderer2DProps props;

      // overload for animator
      if (element.HasComponent<Animator>())
      {
        Animator& animator = *element.GetComponent<Animator>();
        auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(animator.spritesheet_handle));

        props.asset = FLX_STRING_GET(animator.spritesheet_handle);
        props.texture_index = (int)(animator.time * asset_spritesheet.columns) % asset_spritesheet.columns;
      }
      else
      {
        props.asset = FLX_STRING_GET(sprite.sprite_handle);
        props.texture_index = -1;
      }

      props.position = Vector2(pos.position.x, pos.position.y);
      props.rotation = Vector3(rotation.rotation.x, rotation.rotation.y, rotation.rotation.z);
      props.scale = Vector2(scale.scale.x, scale.scale.y);

      const WindowProps& _wp = Application::GetCurrentWindow()->GetProps();
      props.window_size = Vector2((float)_wp.width, (float)_wp.height);

      props.alignment = Renderer2DProps::Alignment_TopLeft;

      OpenGLRenderer::DrawTexture2D(props, CameraManager::GetMainGameCameraID());
    }

#pragma endregion

#pragma region Text Renderer System

    // Text
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
    {
      if (!element.GetComponent<Transform>()->is_active) continue;

      const auto textComponent = element.GetComponent<Text>();

      Renderer2DText sample;
      sample.m_window_size = Vector2(
        static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()),
        static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight())
      );
      sample.m_words = FLX_STRING_GET(textComponent->text);
      sample.m_color = textComponent->color;
      sample.m_fonttype = FLX_STRING_GET(textComponent->fonttype);
      // TODO: Need to convert text to similar to camera class
      // Temp
      sample.m_transform = Matrix4x4(
        element.GetComponent<Scale>()->scale.x, 0.00, 0.00, 0.00, 0.00, element.GetComponent<Scale>()->scale.y, 0.00,
        0.00, 0.00, 0.00, element.GetComponent<Scale>()->scale.z, 0.00, element.GetComponent<Position>()->position.x,
        element.GetComponent<Position>()->position.y, element.GetComponent<Position>()->position.z, 1.00
      );
      sample.m_alignment = std::pair{ static_cast<Renderer2DText::AlignmentX>(textComponent->alignment.first),
                                      static_cast<Renderer2DText::AlignmentY>(textComponent->alignment.second) };
      sample.m_textboxDimensions = textComponent->textboxDimensions;

      OpenGLRenderer::DrawTexture2D(sample, CameraManager::GetMainGameCameraID());
    }

#pragma endregion
  }
}
