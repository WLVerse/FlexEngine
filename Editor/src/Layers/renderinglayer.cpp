// WLVERSE [https://wlverse.web.app]
// renderinglayer.cpp
//
// Rendering layer for the editor.
//
// Implements the main renderers for the editor and game.
// This layer loop runs in this order
// 1. Transformation Calculations
// 2. Update Animator System (ie delta time)
// 3. Sprite Renderer System (Sprites and Animations included)
// 4. Text Renderer System
// 
// The framebuffers used are "scene" and "game", and nothing is rendered to the default framebuffer. (Editor property)
//
// AUTHORS
// [90%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// [10%] Wen Loong (wenloong.l\@digipen.edu)
//   - Added animation rendering
//
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Layers.h"

#include <Physics/physicssystem.h>

namespace Editor
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

        if (!CameraManager::has_main_camera) return;

        Profiler::StartCounter("Graphics");

        Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Game");
        OpenGLRenderer::ClearFrameBuffer();

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

                model.Scale(Vector3(static_cast<float>(sprite_info.GetWidth()) / asset_spritesheet.columns,
                    static_cast<float>(sprite_info.GetHeight()) / asset_spritesheet.rows,
                    1));

                sprite->model_matrix = model;
            }
            else if (FLX_STRING_GET(sprite->sprite_handle) != "")
            {
                auto& sprite_info = FLX_ASSET_GET(Asset::Texture, FLX_STRING_GET(sprite->sprite_handle));
                model.Scale(Vector3(static_cast<float>(sprite_info.GetWidth()), static_cast<float>(sprite_info.GetHeight()), 1));
                sprite->model_matrix = model;
            }

            Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, position);
            Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(rotation).ToRotationMatrix();
            Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, scale);

            transform->transform = translation_matrix * rotation_matrix * scale_matrix * sprite->model_matrix;
        }
        #pragma endregion 

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

        #if 1
        FunctionQueue editor_queue, game_queue;

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

            game_queue.Insert({ [props]() {OpenGLRenderer::DrawTexture2D(*CameraManager::GetMainGameCamera(), props); }, "", index });
            editor_queue.Insert({ [props]() {OpenGLRenderer::DrawTexture2D(Editor::GetInstance().m_editorCamera, props); }, "", index });
        }
        #pragma endregion

        #pragma region Text Renderer System

        // Text
        #pragma region Text Renderer System

        // Text
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
        {
            if (!element.GetComponent<Transform>()->is_active) continue;

            const auto textComponent = element.GetComponent<Text>();

            Renderer2DText sample;
            sample.m_window_size = Vector2(
              static_cast<float>(CameraManager::GetMainGameCamera()->GetOrthoWidth()),
              static_cast<float>(CameraManager::GetMainGameCamera()->GetOrthoHeight())
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

            game_queue.Insert({ [sample]() {OpenGLRenderer::DrawTexture2D(*CameraManager::GetMainGameCamera(), sample); }, "", index });
            editor_queue.Insert({ [sample]() {OpenGLRenderer::DrawTexture2D(Editor::GetInstance().m_editorCamera, sample); }, "", index });
        }

        Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
        editor_queue.Flush();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Game");
        game_queue.Flush();
        #pragma endregion
        #else
        #pragma region Batch Sprite Renderer System

        FunctionQueue batch_render_queue;
        std::vector<std::pair<std::string, FlexECS::Entity>> sortedEntities;
        //Sprite
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Sprite, Position, Rotation, Scale>())
        {
            if (!entity.GetComponent<Transform>()->is_active) continue;

            if (entity.HasComponent<Animator>())
                sortedEntities.emplace_back(FLX_STRING_GET(entity.GetComponent<Animator>()->spritesheet_handle), entity);
            else
                sortedEntities.emplace_back(FLX_STRING_GET(entity.GetComponent<Sprite>()->sprite_handle), entity);
        }
        //Text
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Text, Position, Rotation, Scale>())
        {
            if (!entity.GetComponent<Transform>()->is_active) continue;

            sortedEntities.emplace_back(FLX_STRING_GET(entity.GetComponent<Text>()->fonttype), entity);
        }

        //SORT
        std::sort(sortedEntities.begin(), sortedEntities.end(),
            [](auto& a, auto& b) {
            int zA = a.second.HasComponent<ZIndex>() ? a.second.GetComponent<ZIndex>()->z : 0;
            int zB = b.second.HasComponent<ZIndex>() ? b.second.GetComponent<ZIndex>()->z : 0;
            return zA < zB; // Compare based on z-index or default value
        });

        //QUEUE
        Renderer2DSpriteBatch currentBatch;
        std::string currentTexture = "";

        for (auto& [batchKey, entity] : sortedEntities)
        {
            // Exception: Text -> To be done at later date (Remove this portion once properly implemented in future
            {
                if (entity.HasComponent<Text>())
                {
                    // Flush the current sprite batch if it isnt empty.
                    if (!currentBatch.m_zindex.empty())
                    {
                        AddBatchToQueue(batch_render_queue, currentTexture, currentBatch);
                        currentBatch = Renderer2DSpriteBatch(); // Reset the batch
                        currentTexture = ""; // Reset current texture key (or a default value)
                    }

                    const auto textComponent = entity.GetComponent<Text>();

                    Renderer2DText sample;
                    sample.m_window_size = Vector2(
                      static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()),
                      static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight())
                    );

                    int index = 0;
                    if (entity.HasComponent<ZIndex>()) index = entity.GetComponent<ZIndex>()->z;
                    sample.m_words = FLX_STRING_GET(textComponent->text);
                    sample.m_color = textComponent->color;
                    sample.m_fonttype = FLX_STRING_GET(textComponent->fonttype);

                    sample.m_transform = Matrix4x4(
                      entity.GetComponent<Scale>()->scale.x, 0.00, 0.00, 0.00, 0.00, entity.GetComponent<Scale>()->scale.y, 0.00,
                      0.00, 0.00, 0.00, entity.GetComponent<Scale>()->scale.z, 0.00, entity.GetComponent<Position>()->position.x,
                      entity.GetComponent<Position>()->position.y, entity.GetComponent<Position>()->position.z, 1.00
                    );
                    sample.m_alignment = std::pair{ static_cast<Renderer2DText::AlignmentX>(textComponent->alignment.first),
                                                    static_cast<Renderer2DText::AlignmentY>(textComponent->alignment.second) };
                    sample.m_textboxDimensions = textComponent->textboxDimensions;
                    sample.m_linespacing = 12.0f;
                    batch_render_queue.Insert({ [sample]()
                                        {
                                          OpenGLRenderer::DrawTexture2D(sample, CameraManager::GetMainGameCameraID());
                                        },
                                        "", index });
                    continue;
                }
            }

            if (batchKey != currentTexture)
            {
                AddBatchToQueue(batch_render_queue, currentTexture, currentBatch);
                currentBatch = Renderer2DSpriteBatch(); // Reset the batch
                currentTexture = batchKey;
            }
            AddEntityToBatch(entity, currentBatch);
        }
        // Add the last batch to the queue
        AddBatchToQueue(batch_render_queue, currentTexture, currentBatch);

        batch_render_queue.Flush();
        #pragma endregion
        #endif

        OpenGLFrameBuffer::Unbind(); // Remember to unbind the framebuffer so we can perform swapbuffer calls with default framebuffer

        Profiler::EndCounter("Graphics");
    }

    #pragma region Batch helper
    void RenderingLayer::AddBatchToQueue(FunctionQueue& queue, const std::string& texture, const Renderer2DSpriteBatch& batch)
    {
        if (!batch.m_zindex.empty())
        {
            Renderer2DProps props;
            props.asset = texture;
            //props.vbo_id = vbo_id;
            // Send camera data as parameter instead of ID.
            queue.Insert({ [props, batch]() { OpenGLRenderer::DrawBatchTexture2D(props, batch, *CameraManager::GetMainGameCamera()); }, "", batch.m_zindex.back() });
        }
    }

    void RenderingLayer::AddEntityToBatch(FlexECS::Entity& entity, Renderer2DSpriteBatch& batch)
    {
        auto z_index = entity.HasComponent<ZIndex>() ? entity.GetComponent<ZIndex>()->z : 0;
        batch.m_zindex.push_back(z_index);
        batch.m_transformationData.push_back(entity.GetComponent<Transform>()->transform);
        batch.m_opacity.push_back(entity.GetComponent<Sprite>()->opacity);

        //Checks for other components present that would influence batch
        //Color has been removed as it was not added in sprite component
        //Vector3 colorAdd, colorMul;
        //colorMul = Vector3::One;
        //if (entity.HasComponent<Button>())
        //{
        //    colorAdd += entity.GetComponent<Button>()->finalColorAdd;
        //    colorMul *= entity.GetComponent<Button>()->finalColorMul;
        //}

        if (entity.HasComponent<Animator>() && FLX_STRING_GET(entity.GetComponent<Animator>()->spritesheet_handle) != "")
        {
            auto anim = entity.GetComponent<Animator>();

            //batch.m_colorAddData.push_back(colorAdd + anim->color_to_add);
            //batch.m_colorMultiplyData.push_back(colorMul * anim->color_to_multiply);
            auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(entity.GetComponent<Animator>()->spritesheet_handle));
            batch.m_UVmap.push_back(asset_spritesheet.GetUV(anim->current_frame)); //No choice, must do here, cannot be in engine
        }
        else
        {
            //auto sprite = entity.GetComponent<Sprite>();
            //batch.m_colorAddData.push_back(colorAdd + sprite->color_to_add);
            //batch.m_colorMultiplyData.push_back(colorMul * sprite->color_to_multiply);
            batch.m_UVmap.push_back(Vector4(0, 0, 1, 1)); // Basic sprite UV
        }
    }
    #pragma endregion
} // namespace Editor
