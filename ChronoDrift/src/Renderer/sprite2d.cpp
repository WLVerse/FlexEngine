/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// sprite2d.cpp
//
// This file is responsible for handling 2D sprite rendering within the game. 
// It focuses on batch rendering, enabling the efficient processing of large
// numbers of sprites in a single draw call. The system minimizes OpenGL state
// changes by organizing draw operations into a queue that is flushed once
// all sprites have been prepared.
//
// Key functionalities include:
// - Hierarchical transformation system, where child entities inherit
//   transformations from their parents, enabling complex object groupings
//   and movement.
// - Efficient batch rendering pipeline to reduce draw calls, leveraging
//   modern OpenGL techniques such as VAOs, VBOs, and shader management.
// - Managing the necessary OpenGL resources (shaders, framebuffers, textures)
//   and ensuring optimized memory usage during the rendering process.
// - Ensuring precise alignment of sprites via local transformations and
//   alignment options, with smooth integration into the parent-child matrix
//   relationship.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//   - Developed the hierarchical transformation and batch rendering system
//     to optimize large-scale 2D rendering workflows, including post-processing
//     and shader integration.
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "Renderer/sprite2d.h"

#include "Components/rendering.h"

namespace ChronoDrift
{
    #pragma region Update Matrix / Parent & Child

    /*!***************************************************************************
    * \brief
    * Updates the transformation matrix for a given entity based on its local
    * position, scale, and optional rotation. The matrix is combined with the
    * parent's transformation matrix.
    *
    * \param currEntity The entity whose transformation matrix needs to be updated.
    * \param parent_entity_matrix The transformation matrix of the parent entity
    * (defaults to identity matrix).
    *****************************************************************************/
    void UpdateTransformationMatrix(FlexECS::Entity& currEntity, const Matrix4x4 parent_entity_matrix = Matrix4x4::Identity)
    {
        auto& local_transform = currEntity.GetComponent<Transform>()->transform;
        if (!currEntity.GetComponent<Transform>()->is_dirty) return;

        auto& local_position = currEntity.GetComponent<Position>()->position;
        auto& local_scale = currEntity.GetComponent<Scale>()->scale;
        // Get rotation component if it exists
        Rotation* local_rotation = nullptr;
        if (currEntity.TryGetComponent<Rotation>(local_rotation))
            local_rotation = currEntity.GetComponent<Rotation>();

        //Alignment of sprite
        static Vector2 sprite_alignment = Vector2::Zero;
        // Get sprite component if it exists
        Sprite* checkSprite = nullptr;
        if (currEntity.TryGetComponent<Sprite>(checkSprite))
        {
            switch (currEntity.GetComponent<Sprite>()->alignment)
            {
            case Renderer2DProps::Alignment::Alignment_TopLeft: sprite_alignment = Vector2(local_scale.x * 0.5f, local_scale.y * 0.5f); break;
            case Renderer2DProps::Alignment::Alignment_TopRight: sprite_alignment = Vector2(-local_scale.x * 0.5f, local_scale.y * 0.5f); break;
            case Renderer2DProps::Alignment::Alignment_BottomLeft: sprite_alignment = Vector2(local_scale.x * 0.5f, -local_scale.y * 0.5f); break;
            case Renderer2DProps::Alignment::Alignment_BottomRight: sprite_alignment = Vector2(-local_scale.x * 0.5f, -local_scale.y * 0.5f); break;
            default: case Renderer2DProps::Alignment::Alignment_Center: sprite_alignment = Vector2::Zero; break;
            }
        }

        // calculate the transform
        Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, Vector3(-(local_position.x + sprite_alignment.x), local_position.y + sprite_alignment.y, 0.0f));
        Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(local_rotation != nullptr ? local_rotation->rotation : Vector3::Zero).ToRotationMatrix();
        Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, local_scale);

        // Apply parent entity's matrix
        local_transform = parent_entity_matrix * (translation_matrix * rotation_matrix * scale_matrix);
    }

    void UpdateCamMatrix(FlexECS::Entity& currCam)
    {
        if (!currCam.GetComponent<Transform>()->is_dirty) return;

        auto& local_position = currCam.GetComponent<Position>()->position;
        // Get rotation component if it exists
        Rotation* local_rotation = nullptr;
        if (currCam.TryGetComponent<Rotation>(local_rotation))
            local_rotation = currCam.GetComponent<Rotation>();

        //Update CamData
        if (!currCam.GetComponent<Transform>()->is_dirty) return; //TODO Check is this necessary
        auto& local_camData = currCam.GetComponent<Camera>()->camera;
        local_camData.position = local_position;
        Camera2D::UpdateProjectionMatrix(local_camData);
        Camera2D::UpdateViewMatrix(local_camData);
        
        CameraManager::UpdateData(currCam.Get(), local_camData);
    }

    /*!***************************************************************************
    * \brief
    * Updates the transformation matrix of 2D sprites within the scene. It ensures
    * proper alignment and processing of entities in the scene, particularly
    * their position and orientation in the hierarchy.
    *****************************************************************************/
    void UpdateAllEntitiesMatrix()
    {
        //DEBUG CHECKS IF IMAGE IS FROZEN OR NOT SHOWING
        // 1. DID YOU SET IS_DIRTY = true;
        // 2. DO YOU ADD A TRANSFORM COMPONENT TO YOUR OBJECT
        // 3. IS YOUR OBJECTS BEING CALLED IN THE LOOP TO BE PROCESSED? //Uncomment logs to check or ask wei jie

        static std::vector<FlexECS::Entity*> t_entitystack;
        t_entitystack.clear();  // Clear stack at the beginning of each update
        // Unordered set to track processed entities (avoid redundant updates)
        static std::unordered_set<FlexECS::EntityID> t_processedEntities;
        t_processedEntities.clear();

        for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->CachedQuery<IsActive, Transform>())
        {
            // Check if this entity has already been processed
            if (t_processedEntities.find(entity.Get()) != t_processedEntities.end() || !entity.GetComponent<IsActive>()->is_active) continue; //Skip

            // Traverse up the hierarchy and collect parent entities in a stack
            FlexECS::Entity* t_currentEntity = &entity;
            // Track whether any entity in the stack is dirty
            bool entity_isdirty = false;
            //Update the parent order
            while (true)
            {
                // Push the entity into the stack to process later
                t_entitystack.push_back(t_currentEntity);
                entity_isdirty = t_currentEntity->GetComponent<Transform>()->is_dirty ? true : entity_isdirty;

                // Get the parent of the current entity
                Parent* t_parententity = nullptr;
                if ((*t_currentEntity).TryGetComponent<Parent>(t_parententity))
                {
                    t_parententity = (*t_currentEntity).GetComponent<Parent>();

                    if (t_parententity->parent == FlexECS::Entity::Null) break;

                    // Move up to the parent entity
                    t_currentEntity = &t_parententity->parent;
                }
                else 
                    break;
            }
            // At this point, `t_entitystack` contains the chain of entities from the child up to the root

            Matrix4x4 globaltransform = Matrix4x4::Identity;  // Start with identity matrix for the root entity
            for (auto it = t_entitystack.rbegin(); it != t_entitystack.rend(); ++it)
            {
                // Check if the parent has already been processed, don't update matrix
                if (t_processedEntities.find((*it)->Get()) == t_processedEntities.end())
                {
                    // If any entity in the hierarchy is dirty, mark all as dirty
                    (*it)->GetComponent<Transform>()->is_dirty = entity_isdirty ? true : (*it)->GetComponent<Transform>()->is_dirty;
                    
                    //Update current obj transform
                    UpdateTransformationMatrix(**it, globaltransform);
                    Camera* if_cam = nullptr;
                    if ((*it)->TryGetComponent<Camera>(if_cam)) 
                        UpdateCamMatrix(**it);   

                    // Mark the entity as processed
                    t_processedEntities.insert((*it)->Get());
                }
                // Update the parent's global transform to pass it down to the next child
                globaltransform = (*it)->GetComponent<Transform>()->transform;

                //Log::Debug(FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(*(*it)->GetComponent<EntityName>()));
            }
            // Log::Debug(" ");
            t_entitystack.clear();
        }
        //Log::Debug("****************************************************************");

        //Ensure all entities is no longer dirty
        for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->CachedQuery<IsActive, Transform>())
        {
            if (!entity.GetComponent<IsActive>()->is_active) continue;

            entity.GetComponent<Transform>()->is_dirty = false;
        }
    }

    #pragma endregion

    #pragma region Rendering Processes

    void RenderNormalEntities()
    {
        FunctionQueue pp_render_queue, non_pp_render_queue;

        for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Shader, Sprite>())
        {
            if (!entity.GetComponent<IsActive>()->is_active) continue;

            auto& z_index = entity.GetComponent<ZIndex>()->z;
            Matrix4x4 transform = entity.GetComponent<Transform>()->transform;
            auto& shader = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Shader>()->shader);
            auto sprite = entity.GetComponent<Sprite>();

            Renderer2DProps props;
            props.shader = shader;
            props.transform = transform;
            props.texture = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(sprite->texture);
            props.color_to_add = sprite->color_to_add;
            props.color_to_multiply = sprite->color_to_multiply;
            props.alignment = static_cast<Renderer2DProps::Alignment>(sprite->alignment);
            props.vbo_id = sprite->vbo_id;

            pp_render_queue.Insert({ [props]() { OpenGLSpriteRenderer::DrawTexture2D(props); }, "", z_index });
        }

        bool depth_test = OpenGLRenderer::IsDepthTestEnabled();
        if (depth_test) OpenGLRenderer::DisableDepthTest();

        bool blending = OpenGLRenderer::IsBlendingEnabled();
        if (!blending) OpenGLRenderer::EnableBlending();

        OpenGLFrameBuffer::SetEditorFrameBuffer();
        OpenGLFrameBuffer::ClearFrameBuffer();

        pp_render_queue.Flush();
        OpenGLSpriteRenderer::DrawPostProcessingLayer();
        non_pp_render_queue.Flush();

        if (depth_test) OpenGLRenderer::EnableDepthTest();
        if (!blending) OpenGLRenderer::DisableBlending();
    }

    void RenderBatchedEntities()
    {
        std::unordered_map<std::string, Sprite_Batch_Inst> batchMap;

        for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Shader, Sprite>())
        {
            if (!entity.GetComponent<IsActive>()->is_active) continue;

            Matrix4x4 transform = entity.GetComponent<Transform>()->transform;
            auto sprite = entity.GetComponent<Sprite>();

            std::string batchKey = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(sprite->texture);
            if (batchMap.find(batchKey) == batchMap.end())
            {
                batchMap[batchKey] = Sprite_Batch_Inst();
                batchMap[batchKey].m_vboid = sprite->vbo_id;
            }

            batchMap[batchKey].m_transformationData.push_back(transform);
            batchMap[batchKey].m_colorAddData.push_back(sprite->color_to_add);
            batchMap[batchKey].m_colorMultiplyData.push_back(sprite->color_to_multiply);
        }

        FunctionQueue anim_render_queue;

        for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Shader, Animation>())
        {
            if (!entity.GetComponent<IsActive>()->is_active) continue;

            auto& z_index = entity.GetComponent<ZIndex>()->z;
            Matrix4x4 transform = entity.GetComponent<Transform>()->transform;
            auto& shader = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Shader>()->shader);
            auto anim = entity.GetComponent<Animation>();

            Renderer2DProps props;
            props.shader = shader;
            props.transform = transform;
            props.texture = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(anim->spritesheet);
            anim->m_animationTimer += FlexEngine::Application::GetCurrentWindow()->GetDeltaTime();
            if (anim->m_animationTimer >= anim->m_animationDurationPerFrame)
            {
                anim->m_animationTimer = 0;
                anim->m_currentSpriteIndex = ++anim->m_currentSpriteIndex % anim->max_sprites;

                int current_sprite_row = anim->m_currentSpriteIndex / anim->cols;
                int current_sprite_col = anim->m_currentSpriteIndex % anim->cols;
                anim->m_currUV.z = 1.f / anim->cols;
                anim->m_currUV.w = 1.f / anim->rows;
                anim->m_currUV.x = anim->m_currUV.z * current_sprite_col;
                anim->m_currUV.y = anim->m_currUV.w * current_sprite_row;
            }

            props.color_to_add = anim->color_to_add;
            props.color_to_multiply = anim->color_to_multiply;

            anim_render_queue.Insert({ [props, anim]() { OpenGLSpriteRenderer::DrawAnim2D(props, anim->m_currUV); }, "", z_index });
        }

        bool depth_test = OpenGLRenderer::IsDepthTestEnabled();
        if (depth_test) OpenGLRenderer::DisableDepthTest();

        bool blending = OpenGLRenderer::IsBlendingEnabled();
        if (!blending) OpenGLRenderer::EnableBlending();

        OpenGLFrameBuffer::SetEditorFrameBuffer();
        OpenGLFrameBuffer::ClearFrameBuffer();

        Renderer2DProps props;

        for (auto& [key, batchData] : batchMap)
        {
            props.texture = key;
            props.vbo_id = batchData.m_vboid;
            OpenGLSpriteRenderer::DrawBatchTexture2D(props, batchData);
        }

        anim_render_queue.Flush();
        OpenGLSpriteRenderer::DrawPostProcessingLayer();

        if (depth_test) OpenGLRenderer::EnableDepthTest();
        if (!blending) OpenGLRenderer::DisableBlending();
    }

    void RenderTextEntities()
    {
        FunctionQueue text_render_queue;
        constexpr float FONT_SCALE_FACTOR = 3.0f;

        for (auto& txtentity : FlexECS::ecs_manager.GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Shader, Text>())
        {
            if (!txtentity.GetComponent<IsActive>()->is_active) continue;

            Matrix4x4& transform = txtentity.GetComponent<Transform>()->transform;
            auto shader = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(txtentity.GetComponent<Shader>()->shader);
            auto* textComponent = txtentity.GetComponent<Text>();
            auto font = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(textComponent->fonttype);
            
            Renderer2DText sample;
            sample.m_words = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(textComponent->text);
            sample.m_shader = shader;
            sample.m_fonttype = font;
            sample.m_transform = transform;
            sample.m_window_size = Vector2(static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()), static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()));
            sample.m_alignment = {
                static_cast<Renderer2DText::AlignmentX>(textComponent->alignment.first),
                static_cast<Renderer2DText::AlignmentY>(textComponent->alignment.second)
            };
            sample.m_color = textComponent->color;

            //Refocus font
            if (textComponent && textComponent->refocus) 
            {
                // Adjust font size
                auto& assetFont = FLX_ASSET_GET(Asset::Font, sample.m_fonttype);
                assetFont.SetFontSize(static_cast<int>(sample.m_transform.m00 * FONT_SCALE_FACTOR));
                transform /= FONT_SCALE_FACTOR;
                Log::Debug("Font: " + sample.m_fonttype + " has been resized to " + std::to_string(sample.m_transform.m00 * FONT_SCALE_FACTOR));
                // Reset refocus flag
                textComponent->refocus = false;
            }

            text_render_queue.Insert({ [sample]() { OpenGLTextRenderer::DrawText2D(sample, true); }, "", 0 });
        }

        bool depth_test = OpenGLRenderer::IsDepthTestEnabled();
        if (depth_test) OpenGLRenderer::DisableDepthTest();

        bool blending = OpenGLRenderer::IsBlendingEnabled();
        if (!blending) OpenGLRenderer::EnableBlending();

        OpenGLFrameBuffer::SetEditorFrameBuffer();
        text_render_queue.Flush();
        OpenGLFrameBuffer::SetDefaultFrameBuffer();

        if (depth_test) OpenGLRenderer::EnableDepthTest();
        if (!blending) OpenGLRenderer::DisableBlending();
    }

    void ForceRenderToScreen()
    {
        bool depth_test = OpenGLRenderer::IsDepthTestEnabled();
        if (depth_test) OpenGLRenderer::DisableDepthTest();

        bool blending = OpenGLRenderer::IsBlendingEnabled();
        if (!blending) OpenGLRenderer::EnableBlending();

        OpenGLFrameBuffer::SetDefaultFrameBuffer();

        Renderer2DProps data;
        data.shader = "\\shaders\\texture";
        int width = FlexEngine::Application::GetCurrentWindow()->GetWidth();
        int height = FlexEngine::Application::GetCurrentWindow()->GetHeight();
        data.transform = Matrix4x4(static_cast<float>(width), 0, 0, 0,
                                   0, static_cast<float>(height), 0, 0,
                                   0,0,1.f,0,
                                   -static_cast<float>(width) / 2.f, static_cast<float>(height) / 2.f,0,1.f);
        data.vbo_id = Renderer2DProps::VBO_BasicInverted;
        OpenGLSpriteRenderer::DrawTexture2D(OpenGLFrameBuffer::GetCreatedTexture(OpenGLFrameBuffer::CreatedTextureID::CID_finalRender), data);

        if (depth_test) OpenGLRenderer::EnableDepthTest();
        if (!blending) OpenGLRenderer::DisableBlending();
    }
    #pragma endregion

    /*!***************************************************************************
    * \brief
    * Renders all the 2D sprites in the scene. This function handles the necessary
    * steps for rendering, including setting up shader properties, handling
    * post-processing, and batch rendering for efficiency.
    *****************************************************************************/
    void RenderSprite2D()
    {
        //Update Transformation Matrix of All Entities
        UpdateAllEntitiesMatrix();

        WindowProps window_props = Application::GetCurrentWindow()->GetProps();
        Renderer2DProps props;
        props.window_size = { static_cast<float>(window_props.width), static_cast<float>(window_props.height) };

        ////////////////////////////////////////////////////////////////////////////////
        // Potential Issues
        ////////////////////////////////////////////////////////////////////////////////
        // 1. the order of post-processed objects is rendered first, then non-post-processed (For the sake of text box)

        //RenderNormalEntities();
        RenderBatchedEntities();
        RenderTextEntities();

        //What Rocky wants (TO DELETE)
        #ifndef _DEBUG
        ForceRenderToScreen();
        #endif
    }
}