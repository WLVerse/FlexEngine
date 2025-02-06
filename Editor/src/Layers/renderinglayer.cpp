/*!************************************************************************
 * RENDERING LAYER [https://wlverse.web.app]
 * renderinglayer.cpp - EDITOR
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
#include "editor.h"

namespace Editor
{
    /*!
     * \brief Called when the RenderingLayer is attached to the editor.
     *
     * This function is invoked when the editor rendering layer is attached.
     * It currently does not perform any specific operations but serves as
     * a placeholder for future initialization tasks.
     */
    void RenderingLayer::OnAttach()
    {
    }

    /*!
     * \brief Called when the RenderingLayer is detached from the editor.
     *
     * This function is invoked when the editor rendering layer is detached.
     * It currently does not perform any specific cleanup operations but is
     * reserved for potential future resource deallocation or state reset.
     */
    void RenderingLayer::OnDetach()
    {
    }

    /*!
     * \brief Updates the editor's rendering operations each frame.
     *
     * The Update function performs the following tasks:
     * - Sets the current framebuffer for both the "Scene" and "Game" views,
     *   clearing them before rendering.
     * - Updates the Transform components of entities by recalculating their
     *   transformation matrices based on their Position, Rotation, and Scale.
     * - Advances the time value for Animator components, which in turn updates
     *   animated sprites.
     * - Queues sprite rendering commands for both the editor and game viewports.
     *   This handles both static sprites and those using animator data.
     * - Queues text rendering commands for active text entities, including setting
     *   properties like font type, color, and alignment.
     * - Flushes the command queues for both views and unbinds the framebuffer after
     *   rendering.
     *
     * TODO:
     * - Refine the animator time reset and ensure proper animation looping.
     * - Update the text transform logic to more closely resemble the camera class.
     */
    void RenderingLayer::Update()
    {
        Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Game");
        OpenGLRenderer::ClearFrameBuffer();

        // Update Transform component
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

        #pragma region Animator System

        // Animator system updates the time for all animators
        // TODO: move this to a different layer if necessary
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Animator, Sprite>())
        {
            Animator& animator = *element.GetComponent<Animator>();

            if (animator.should_play)
            {
                // TODO: Consider resetting time appropriately when needed.
                animator.time += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
            }
        }

        #pragma endregion

        #pragma region Sprite Renderer System
        FunctionQueue editor_queue, game_queue;

        // Render all sprites for both editor and game views
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, Position, Rotation, Scale>())
        {
            Sprite& sprite = *element.GetComponent<Sprite>();
            Position& pos = *element.GetComponent<Position>();
            Rotation& rotation = *element.GetComponent<Rotation>();
            Scale& scale = *element.GetComponent<Scale>();

            Renderer2DProps props;

            // Overload for animator if present
            if (element.HasComponent<Animator>())
            {
                Animator& animator = *element.GetComponent<Animator>();
                auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(animator.spritesheet_handle));

                props.asset = FLX_STRING_GET(animator.spritesheet_handle);
                props.texture_index = (int)(animator.time * asset_spritesheet.columns) % asset_spritesheet.columns;
                props.alpha = 1.0f; // Update placeholder alpha value as needed
            }
            else
            {
                props.asset = FLX_STRING_GET(sprite.sprite_handle);
                props.texture_index = -1;
                props.alpha = sprite.opacity;
            }

            int index = 0;
            if (element.HasComponent<ZIndex>())
                index = element.GetComponent<ZIndex>()->z;

            props.position = Vector2(pos.position.x, pos.position.y);
            props.rotation = Vector3(rotation.rotation.x, rotation.rotation.y, rotation.rotation.z);
            props.scale = Vector2(scale.scale.x, scale.scale.y);

            const WindowProps& _wp = Application::GetCurrentWindow()->GetProps();
            props.window_size = Vector2((float)_wp.width, (float)_wp.height);

            props.alignment = Renderer2DProps::Alignment_TopLeft;

            // Queue the sprite for the game view
            game_queue.Insert({ [props]() {
              OpenGLRenderer::DrawTexture2D(props, CameraManager::GetMainGameCameraID());
            }, "", index });

            // Queue the sprite for the editor view
            editor_queue.Insert({ [props]() {
              OpenGLRenderer::DrawTexture2D(props, Editor::GetInstance().m_editorCamera);
            }, "", index });
        }

        #pragma endregion

        #pragma region Text Renderer System

        // Render text elements for both editor and game views
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
            // TODO: Update text transform logic to match camera class behavior.
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

            // Queue text rendering commands for both views
            game_queue.Insert({ [sample]() {OpenGLRenderer::DrawTexture2D(sample, CameraManager::GetMainGameCameraID()); }, "", index });
            editor_queue.Insert({ [sample]() {OpenGLRenderer::DrawTexture2D(sample, Editor::GetInstance().m_editorCamera); }, "", index });
        }
        #pragma endregion

        // Flush the command queues to perform rendering
        Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
        editor_queue.Flush();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Game");
        game_queue.Flush();

        // Unbind the framebuffer after rendering is complete
        OpenGLFrameBuffer::Unbind();
    }

    /*!
     * \brief Retrieves a sorted render queue of entities for batching.
     *
     * This function collects active entities with Sprite and Text components,
     * retrieving their associated asset strings from the internal storage.
     * The entities are then sorted by their z-index value (defaulting to 0 if not present).
     *
     * \return A vector of pairs, where each pair consists of the asset string and
     *         the corresponding entity.
     */
    std::vector<std::pair<std::string, FlexECS::Entity>> RenderingLayer::GetRenderQueue()
    {
        std::vector<std::pair<std::string, FlexECS::Entity>> sortedEntities;

        // --- Add relevant entities to sort by z-index ---

        // For Sprite components
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Sprite>())
        {
            if (!entity.GetComponent<Transform>()->is_active)
                continue;
            sortedEntities.emplace_back(
                FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Sprite>()->sprite_handle),
                entity
            );
        }

        // For Text components
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, Text>())
        {
            if (!entity.GetComponent<Transform>()->is_active)
                continue;
            sortedEntities.emplace_back(
                FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Text>()->fonttype),
                entity
            );
        }

        // Sort entities based on their z-index (defaulting to 0 if not specified)
        std::sort(sortedEntities.begin(), sortedEntities.end(),
            [](auto& a, auto& b) {
            int zA = a.second.HasComponent<ZIndex>() ? a.second.GetComponent<ZIndex>()->z : 0;
            int zB = b.second.HasComponent<ZIndex>() ? b.second.GetComponent<ZIndex>()->z : 0;
            return zA < zB;
        });

        return sortedEntities;
    }
} // namespace Editor
