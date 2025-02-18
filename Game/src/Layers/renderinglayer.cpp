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
    OpenGLFrameBuffer::Unbind();

   #pragma region Transformation Calculations
    // Update Transform component to obtain the true world representation of the entity
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, Position, Rotation, Scale, Transform>())
    {
      auto sprite = element.GetComponent<Sprite>();
      auto position = element.GetComponent<Position>()->position;
      auto rotation = element.GetComponent<Rotation>()->rotation;
      auto scale = element.GetComponent<Scale>()->scale;
      auto transform = element.GetComponent<Transform>();

      // "Model scale" in this case refers to the scale of the object itself...
      Matrix4x4 model = Matrix4x4::Identity;
      
      // However, spritesheets have a different scale...
      if (element.HasComponent<Animator>() && FLX_STRING_GET(element.GetComponent<Animator>()->spritesheet_handle) != "")
      {
        auto& animator = *element.GetComponent<Animator>();
        auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(animator.spritesheet_handle));
        auto& sprite_info = FLX_ASSET_GET(Asset::Texture, asset_spritesheet.texture);

        model.Scale(Vector3(static_cast<float>(sprite_info.GetWidth() / asset_spritesheet.columns),
                            static_cast<float>(sprite_info.GetHeight() / asset_spritesheet.rows),
                            1.f));
        sprite->model_matrix = model;
      }
      else if (FLX_STRING_GET(sprite->sprite_handle) != "")
      {
        auto& sprite_info = FLX_ASSET_GET(Asset::Texture, FLX_STRING_GET(sprite->sprite_handle));
        model.Scale(Vector3(static_cast<float>(sprite_info.GetWidth()), 
                            static_cast<float>(sprite_info.GetHeight()), 
                            1.f));
        sprite->model_matrix = model;
      }

      Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, position);
      Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(rotation).ToRotationMatrix();
      Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, scale);

      transform->transform = translation_matrix * rotation_matrix * scale_matrix * sprite->model_matrix;
    }
    #pragma endregion 

    if (!CameraManager::has_main_camera) return;

   #pragma region Animator System

      // animator system updates the time for all animators
      // TODO: move this to a different layer
      for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Animator, Sprite>())
      {
        Animator& animator = *element.GetComponent<Animator>();

        if (!animator.should_play) continue;

        animator.frame_time += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

        auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(animator.spritesheet_handle));

        // calculate the total frames
        if (animator.total_frames != asset_spritesheet.columns * asset_spritesheet.rows)
          animator.total_frames = asset_spritesheet.columns * asset_spritesheet.rows;

        // TODO: debug why this is happening
        if (animator.current_frame >= asset_spritesheet.frame_times.size()) animator.current_frame = 0;

        // get the current frame time
        animator.current_frame_time = asset_spritesheet.frame_times[animator.current_frame];

        // handling of animations
        // move to the next frame
        // loop if looping
        // stop if not looping
        // return to default and continue looping if return_to_default is true
        if (animator.frame_time >= animator.current_frame_time)
        {
          // skip frames if needed
          while (animator.frame_time >= animator.current_frame_time)
          {
            animator.current_frame++;
            animator.frame_time -= animator.current_frame_time;
          }

          // loop
          if (animator.is_looping && animator.current_frame >= animator.total_frames)
          {
            animator.current_frame = 0;
          }

          // not looping
          if (!animator.is_looping && animator.current_frame >= animator.total_frames)
          {
            // return to default and continue looping
            if (animator.return_to_default)
            {
              animator.spritesheet_handle = animator.default_spritesheet_handle;
              animator.is_looping = true;
            }
            // stop at the last frame
            else
            {
              animator.current_frame = animator.total_frames - 1;
              animator.should_play = false;
            }
          }
        }
      }

  #pragma endregion

   FunctionQueue game_queue;

  #pragma region Sprite Renderer System

     // render all sprites
      for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Sprite, Position, Rotation, Scale>())
      {
        if (!element.GetComponent<Transform>()->is_active) continue;

        Sprite& sprite = *element.GetComponent<Sprite>();
        Renderer2DProps props;

        // overload for animator
        if (element.HasComponent<Animator>() && FLX_STRING_GET(element.GetComponent<Animator>()->spritesheet_handle) != "")
        {
          Animator& animator = *element.GetComponent<Animator>();

          props.asset = FLX_STRING_GET(animator.spritesheet_handle);
          props.texture_index = animator.current_frame;
          props.alpha = 1.0f; // Update pls
        }
        else
        {
          props.asset = FLX_STRING_GET(sprite.sprite_handle);
          props.texture_index = -1;
          props.alpha = sprite.opacity;
        }

        int index = 0;
        if (element.HasComponent<ZIndex>()) index = element.GetComponent<ZIndex>()->z;

        props.window_size = Vector2(CameraManager::GetMainGameCamera()->GetOrthoWidth(), CameraManager::GetMainGameCamera()->GetOrthoHeight());
      
        props.alignment = Renderer2DProps::Alignment_TopLeft;
        props.world_transform = element.GetComponent<Transform>()->transform;

        game_queue.Insert({ [props]() {OpenGLRenderer::DrawTexture2D(props, CameraManager::GetMainGameCameraID()); }, "", index });
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

        int index = 0;
        if (element.HasComponent<ZIndex>()) index = element.GetComponent<ZIndex>()->z;

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

        game_queue.Insert({ [sample]()
                            {
                              OpenGLRenderer::DrawTexture2D(sample, CameraManager::GetMainGameCameraID());
                            },
                            "", index });
      }
  #pragma endregion

    game_queue.Flush();

    OpenGLFrameBuffer::Unbind();
  }
} // namespace Game
