/*!************************************************************************
 * RENDERING LAYER [https://wlverse.web.app]
 * renderinglayer.cpp - GAME
 *
 * This file implements the RenderingLayer class for the editor. The layer is
 * responsible for handling rendering operations, including hotloading a rendering
 * DLL and drawing various scene components such as sprites, text, and animated
 * objects.
 *
 * Key functionalities include:
 * - Enabling and disabling OpenGL blending on layer attach/detach.
 * - Updating the transform components of entities.
 * - Handling animator updates for animated sprites.
 * - Queuing and rendering sprites and text using the OpenGLRenderer.
 *
 * The design provides a modular approach to rendering, allowing for hotloading and
 * quick updates to the rendering DLL.
 *
 * AUTHORS
 * [70%] Chan Wen Loong (wenloong.c@digipen.edu)
 *   - Main Author
 * [30%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Sub Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/
#include "Layers.h"
#include <Physics/physicssystem.h>

namespace Game
{
    /*!
     * \brief Called when the RenderingLayer is attached to the engine.
     *
     * This function enables OpenGL blending, which is necessary for rendering
     * transparent textures and other blending effects.
     */
    void RenderingLayer::OnAttach()
    {
        OpenGLRenderer::EnableBlending();
    }

    /*!
     * \brief Called when the RenderingLayer is detached from the engine.
     *
     * This function disables OpenGL blending to clean up any rendering state
     * that was set during attachment.
     */
    void RenderingLayer::OnDetach()
    {
        OpenGLRenderer::DisableBlending();
    }

    /*!
     * \brief Updates the rendering operations for the editor.
     *
     * The Update function performs several key tasks each frame:
     * - **Transform Update**: Iterates through entities with Position, Rotation,
     *   Scale, and Transform components to update their transformation matrices.
     * - **Animator System**: Updates animator components to progress animations.
     * - **Sprite Renderer System**: Queues sprite drawing commands using a function
     *   queue based on entity properties and z-index ordering.
     * - **Text Renderer System**: Queues text drawing commands with specified
     *   properties such as font, color, and alignment.
     *
     * After processing all renderable components, the function flushes the render
     * queue and unbinds the OpenGL framebuffer.
     */
    void RenderingLayer::Update()
    {
        // ------------------ Transform Component Update ------------------
        // For each entity with Position, Rotation, Scale, and Transform components,
        // compute the transformation matrix.
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, Rotation, Scale, Transform>())
        {
            auto position = element.GetComponent<Position>()->position;
            auto rotation = element.GetComponent<Rotation>()->rotation;
            auto scale = element.GetComponent<Scale>()->scale;
            auto transform = element.GetComponent<Transform>();

            Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, position);
            Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(rotation).ToRotationMatrix();
            Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, scale);

            transform->transform = translation_matrix * rotation_matrix * scale_matrix;
        }

        // ------------------ Animator System ------------------
        // Updates the time value for all active animators.
        // TODO: Consider moving this to a dedicated animator layer.
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Animator, Sprite>())
        {
            Animator& animator = *element.GetComponent<Animator>();

            if (animator.should_play)
            {
                // Increment the animator time based on frame delta time.
                animator.time += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            }
        }

        // ------------------ Sprite Renderer System ------------------
        FunctionQueue game_queue;

        // Iterate through entities with Sprite, Position, Rotation, and Scale components.
        // Only active transforms are considered.
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, Position, Rotation, Scale>())
        {
            if (!element.GetComponent<Transform>()->is_active)
                continue;

            Sprite& sprite = *element.GetComponent<Sprite>();
            Position& pos = *element.GetComponent<Position>();
            Rotation& rotation = *element.GetComponent<Rotation>();
            Scale& scale = *element.GetComponent<Scale>();

            Renderer2DProps props;

            // If the entity has an Animator, calculate texture index based on animation time.
            if (element.HasComponent<Animator>())
            {
                Animator& animator = *element.GetComponent<Animator>();
                auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(animator.spritesheet_handle));

                props.asset = FLX_STRING_GET(animator.spritesheet_handle);
                props.texture_index = static_cast<int>(animator.time * asset_spritesheet.columns) % asset_spritesheet.columns;
                props.alpha = 1.0f; // Placeholder value; update as needed.
            }
            else
            {
                props.asset = FLX_STRING_GET(sprite.sprite_handle);
                props.texture_index = -1;
                props.alpha = sprite.opacity;
            }

            // Determine draw order based on z-index.
            int index = 0;
            if (element.HasComponent<ZIndex>())
                index = element.GetComponent<ZIndex>()->z;

            props.position = Vector2(pos.position.x, pos.position.y);
            props.rotation = Vector3(rotation.rotation.x, rotation.rotation.y, rotation.rotation.z);
            props.scale = Vector2(scale.scale.x, scale.scale.y);

            const WindowProps& _wp = Application::GetCurrentWindow()->GetProps();
            props.window_size = Vector2(static_cast<float>(_wp.width), static_cast<float>(_wp.height));

            props.alignment = Renderer2DProps::Alignment_TopLeft;

            // Queue the sprite drawing command.
            game_queue.Insert({ [props]() {
                OpenGLRenderer::DrawTexture2D(props, CameraManager::GetMainGameCameraID());
            }, "", index });
        }

        // ------------------ Text Renderer System ------------------
        // Iterate through entities with a Text component and queue text rendering.
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
        {
            if (!element.GetComponent<Transform>()->is_active)
                continue;

            const auto textComponent = element.GetComponent<Text>();

            Renderer2DText sample;
            sample.m_window_size = Vector2(
                static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()),
                static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight())
            );

            int index = 0;
            if (element.HasComponent<ZIndex>())
                index = element.GetComponent<ZIndex>()->z;

            sample.m_words = FLX_STRING_GET(textComponent->text);
            sample.m_color = textComponent->color;
            sample.m_fonttype = FLX_STRING_GET(textComponent->fonttype);
            // TODO: Convert text to use a transformation similar to the camera class.
            // Temporary transform matrix based on Scale and Position components.
            sample.m_transform = Matrix4x4(
                element.GetComponent<Scale>()->scale.x, 0.00, 0.00, 0.00,
                0.00, element.GetComponent<Scale>()->scale.y, 0.00, 0.00,
                0.00, 0.00, element.GetComponent<Scale>()->scale.z, 0.00,
                element.GetComponent<Position>()->position.x, element.GetComponent<Position>()->position.y,
                element.GetComponent<Position>()->position.z, 1.00
            );
            sample.m_alignment = std::pair{
                static_cast<Renderer2DText::AlignmentX>(textComponent->alignment.first),
                static_cast<Renderer2DText::AlignmentY>(textComponent->alignment.second)
            };
            sample.m_textboxDimensions = textComponent->textboxDimensions;

            // Queue the text drawing command.
            game_queue.Insert({ [sample]() {
                OpenGLRenderer::DrawTexture2D(sample, CameraManager::GetMainGameCameraID());
            }, "", index });
        }

        // Flush all queued render commands.
        game_queue.Flush();

        // Unbind the framebuffer after rendering.
        OpenGLFrameBuffer::Unbind();
    }
}