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
    constexpr float FONT_SCALE_FACTOR = 3.0f;

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
        if (currEntity.HasComponent<Rotation>())
            local_rotation = currEntity.GetComponent<Rotation>();

        //Alignment of sprite
        static Vector2 sprite_alignment = Vector2::Zero;
        // Get sprite component if it exists
        if (currEntity.HasComponent<Sprite>())
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

    void UpdateCamMatrix(FlexECS::Entity& currCam, CameraManager* CamManager)
    {
        //if (!currCam.GetComponent<Transform>()->is_dirty) return; //Seems to be the cause of some blue screen

        //TODO @WEIJIE Hierarchy movement of camera not working as intended -> Inspect (LOW Priority)
        Vector3 local_position = { currCam.GetComponent<Transform>()->transform.m30,currCam.GetComponent<Transform>()->transform.m31, currCam.GetComponent<Transform>()->transform.m32 };
        Vector2 local_scale = currCam.GetComponent<Scale>()->scale;
        //auto& local_position = currCam.GetComponent<Position>()->position;
        // Get rotation component if it exists
        //Rotation* local_rotation = nullptr;
        //if (currCam.HasComponent<Rotation>())
        //    local_rotation = currCam.GetComponent<Rotation>();

        //Update CamData
        auto& local_camData = currCam.GetComponent<Camera>()->camera;
        local_camData.position = local_position;
        local_camData.m_OrthoWidth = local_scale.x * 10;
        local_camData.m_OrthoHeight = local_scale.y * 10;
        Camera2D::UpdateProjectionMatrix(local_camData);
        Camera2D::UpdateViewMatrix(local_camData);
        
        CamManager->UpdateData(currCam.Get(), local_camData);
    }

    /*!***************************************************************************
    * \brief
    * Updates the transformation matrix of 2D sprites within the scene. It ensures
    * proper alignment and processing of entities in the scene, particularly
    * their position and orientation in the hierarchy.
    *****************************************************************************/
    void UpdateAllEntitiesMatrix(CameraManager* CamManager)
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

        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, Transform>())
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
                if ((*t_currentEntity).HasComponent<Parent>())
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
                    if ((*it)->HasComponent<Camera>()) 
                        UpdateCamMatrix(**it, CamManager);

                    // Mark the entity as processed
                    t_processedEntities.insert((*it)->Get());
                }
                // Update the parent's global transform to pass it down to the next child
                globaltransform = (*it)->GetComponent<Transform>()->transform;

                //Log::Debug(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*(*it)->GetComponent<EntityName>()));
            }
            // Log::Debug(" ");
            t_entitystack.clear();
        }
        //Log::Debug("****************************************************************");

        //Ensure all entities is no longer dirty
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, Transform>())
        {
            if (!entity.GetComponent<IsActive>()->is_active) continue;

            entity.GetComponent<Transform>()->is_dirty = false;
        }
    }

    #pragma endregion

    #pragma region Batch helper
    void AddBatchToQueue(FunctionQueue& queue, const std::string& texture, const Sprite_Batch_Inst& batch, GLuint vbo_id)
    {
        if (!batch.m_zindex.empty())
        {
            Renderer2DProps props;
            props.texture = texture;
            props.vbo_id = vbo_id;
            queue.Insert({ [props, batch]() { OpenGLSpriteRenderer::DrawBatchTexture2D(props, batch); }, "", batch.m_zindex.back() });
        }
    }

    void AddEntityToBatch(FlexECS::Entity& entity, Sprite_Batch_Inst& batch, const std::string& type)
    {
        auto z_index = entity.GetComponent<ZIndex>()->z;
        batch.m_zindex.push_back(z_index);
        batch.m_transformationData.push_back(entity.GetComponent<Transform>()->transform);

        //Checks for other components present that would influence batch
        Vector3 colorAdd, colorMul;
        colorMul = Vector3::One;
        if (entity.HasComponent<Button>())
        {
            colorAdd += entity.GetComponent<Button>()->finalColorAdd;
            colorMul *= entity.GetComponent<Button>()->finalColorMul;
        }

        if (type == "Sprite")
        {
            auto sprite = entity.GetComponent<Sprite>();
            batch.m_colorAddData.push_back(colorAdd + sprite->color_to_add);
            batch.m_colorMultiplyData.push_back(colorMul * sprite->color_to_multiply);
            batch.m_UVmap.push_back(Vector4(0, 0, 1, 1)); // Basic sprite UV
            batch.m_vboid = sprite->vbo_id;
        }
        else if (type == "Animation")
        {
            auto anim = entity.GetComponent<Animation>();
            batch.m_colorAddData.push_back(colorAdd + anim->color_to_add);
            batch.m_colorMultiplyData.push_back(colorMul * anim->color_to_multiply);
            batch.m_UVmap.push_back(anim->m_currUV);
        }
    }
    #pragma endregion

    #pragma region Rendering Processes

    void UpdateAnimationInScene(double GameTimeSpeedModifier)
    {
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Shader, Animation>())
        {
            if (!entity.GetComponent<IsActive>()->is_active) continue;

            auto anim = entity.GetComponent<Animation>();

            // YC: Since your code has no guard, I will add it here
            if (anim->is_paused|| anim->max_sprites == 0 || anim->cols == 0 || anim->rows == 0 || anim->spritesheet == 0)
            {
              Log::Warning("Animation was detected to not be configured properly");
              continue;
            }

            anim->m_animationTimer += anim->m_animation_speed * FlexEngine::Application::GetCurrentWindow()->GetDeltaTime();
            if (anim->m_animationTimer >= 1.0/GameTimeSpeedModifier) 
            {
                anim->m_animationTimer = 0;
                int totalSprites = ++anim->m_currentSpriteIndex %= anim->max_sprites;
                anim->m_currUV = {
                    (1.f / anim->cols) * (totalSprites % anim->cols),
                    (1.f / anim->rows) * (totalSprites / anim->cols),
                    (1.f / anim->cols)* (totalSprites % anim->cols) + 1.f / anim->cols,
                    (1.f / anim->rows)* (totalSprites / anim->cols) + 1.f / anim->rows
                };
            }
        }
    }

    void RenderNormalEntities(bool want_PP = true)
    {
        FunctionQueue pp_render_queue, non_pp_render_queue;

        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Shader, Sprite>())
        {
            if (!entity.GetComponent<IsActive>()->is_active) continue;

            auto& z_index = entity.GetComponent<ZIndex>()->z;
            Matrix4x4 transform = entity.GetComponent<Transform>()->transform;
            auto& shader = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Shader>()->shader);
            auto sprite = entity.GetComponent<Sprite>();

            Renderer2DProps props;
            props.shader = shader;
            props.transform = transform;
            props.texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(sprite->texture);
            props.color_to_add = sprite->color_to_add;
            props.color_to_multiply = sprite->color_to_multiply;
            props.alignment = static_cast<Renderer2DProps::Alignment>(sprite->alignment);
            props.vbo_id = sprite->vbo_id;

            pp_render_queue.Insert({ [props]() { OpenGLSpriteRenderer::DrawTexture2D(props); }, "", z_index });
        }

        pp_render_queue.Flush();
        #ifndef GAME
        if(want_PP) OpenGLSpriteRenderer::DrawPostProcessingLayer();
        #endif
        non_pp_render_queue.Flush();
    }

    void RenderBatchedEntities(bool want_PP = true)
    {
        FunctionQueue batch_render_queue;
        std::vector<std::pair<std::string, FlexECS::Entity>> sortedEntities;
        //Sprite
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Sprite>()) 
        {
            if (entity.GetComponent<IsActive>()->is_active) 
            {
                sortedEntities.emplace_back(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Sprite>()->texture),
                                            entity);
            }
        }
        //Animation
        for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Animation>())
        {
            if (entity.GetComponent<IsActive>()->is_active)
            {
                sortedEntities.emplace_back(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Animation>()->spritesheet),
                                            entity);
            }
        }

        //SORT
        std::sort(sortedEntities.begin(), sortedEntities.end(),
            [](auto& a, auto& b) {
            int zA = a.second.GetComponent<ZIndex>()->z; 
            int zB = b.second.GetComponent<ZIndex>()->z; 
            return zA < zB; // Compare z-index
        });

        //QUEUE
        Sprite_Batch_Inst currentBatch;
        std::string currentTexture = "";

        for (auto& [batchKey, entity] : sortedEntities)
        {
            if (batchKey != currentTexture)
            {
                AddBatchToQueue(batch_render_queue, currentTexture, currentBatch, currentBatch.m_vboid);
                currentBatch = Sprite_Batch_Inst(); // Reset the batch
                currentTexture = batchKey;
            }
            if (entity.HasComponent<Sprite>()) AddEntityToBatch(entity, currentBatch, "Sprite");
            else if (entity.HasComponent<Animation>()) AddEntityToBatch(entity, currentBatch, "Animation");
        }
        // Add the last batch to the queue
        AddBatchToQueue(batch_render_queue, currentTexture, currentBatch, currentBatch.m_vboid);

        batch_render_queue.Flush();
        #ifndef GAME
        if (want_PP) OpenGLSpriteRenderer::DrawPostProcessingLayer();
        #endif
    }

    void RenderTextEntities()
    {
        FunctionQueue text_render_queue;

        for (auto& txtentity : FlexECS::Scene::GetActiveScene()->CachedQuery<IsActive, ZIndex, Transform, Text>())
        {
            if (!txtentity.GetComponent<IsActive>()->is_active) continue;

            Matrix4x4& transform = txtentity.GetComponent<Transform>()->transform;
            //auto shader = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(txtentity.GetComponent<Shader>()->shader);
            auto* textComponent = txtentity.GetComponent<Text>();
            auto font = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(textComponent->fonttype);
            
            Renderer2DText sample;
            sample.m_words = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(textComponent->text);
            sample.m_shader = "\\shaders\\freetypetext"; // Will have to change it to not hardcoded along with batching
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

        text_render_queue.Flush();
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
        data.window_size = Vector2(static_cast<float>(width), static_cast<float>(height));
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
        //UpdateAllEntitiesMatrix(CamManager);

        WindowProps window_props = Application::GetCurrentWindow()->GetProps();
        Renderer2DProps props;
        props.window_size = { static_cast<float>(window_props.width), static_cast<float>(window_props.height) };

        ////////////////////////////////////////////////////////////////////////////////
        // Potential Issues
        ////////////////////////////////////////////////////////////////////////////////
        // 1. the order of post-processed objects is rendered first, then non-post-processed (For the sake of text box)

        //TODO @WEIJIE 
        // 1. Bloom fix in fullscreen
        // 2. Merge text with batch queue (merge text renderer to sprite renderer)
        // 3. Change the shader name to not be hardcoded for the different rendering process
        //    - either auto add shader component -> auto set correct shader
        // 4. Have the cameras have little icons to show their placement and boundary
        bool depth_test = OpenGLRenderer::IsDepthTestEnabled();
        if (depth_test) OpenGLRenderer::DisableDepthTest();

        bool blending = OpenGLRenderer::IsBlendingEnabled();
        if (!blending) OpenGLRenderer::EnableBlending();

        OpenGLFrameBuffer::SetGameFrameBuffer();
        OpenGLFrameBuffer::ClearFrameBuffer();

        #pragma region Draw Scene Entities
        //RenderNormalEntities();
        RenderBatchedEntities(true); //Dont show PP yet until m4
        RenderTextEntities();
        #pragma endregion

        //Following is how to proceed with actual rendering onto screen
        #ifdef GAME
        {
            //Render directly for gameplay
            OpenGLFrameBuffer::SetDefaultFrameBuffer();
            ForceRenderToScreen();
        }
        #else //Render editor view
        {
            OpenGLFrameBuffer::SetEditorFrameBuffer();
            OpenGLFrameBuffer::ClearFrameBuffer();
            RenderBatchedEntities(false);
            RenderTextEntities();

            //Render Cam Boundaries
            for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
            {
                if(!entity.GetComponent<IsActive>()->is_active || 
                    (Editor::GetInstance().GetSelectedEntity().Get() != entity.Get())) 
                    continue;
                const Vector3& max = Vector3(-entity.GetComponent<Position>()->position.x + entity.GetComponent<Camera>()->camera.m_OrthoWidth / 2.0f, entity.GetComponent<Position>()->position.y + entity.GetComponent<Camera>()->camera.m_OrthoHeight / 2.0f);
                const Vector3& min = Vector3(-entity.GetComponent<Position>()->position.x - entity.GetComponent<Camera>()->camera.m_OrthoWidth / 2.0f, entity.GetComponent<Position>()->position.y - entity.GetComponent<Camera>()->camera.m_OrthoHeight / 2.0f);
                //construct lines for AABB
                Vector3 topleft = min;
                Vector3 topright = { max.x, min.y };
                Vector3 botleft = { min.x, max.y };
                Vector3 botright = max;

                FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), topleft, topright, Vector3::Up);
                FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), topright, botright, Vector3::Up);
                FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), botright, botleft, Vector3::Up);
                FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), botleft, topleft, Vector3::Up);
            }
        }
        #endif

        //Refresh settings
        if (depth_test) OpenGLRenderer::EnableDepthTest();
        if (!blending) OpenGLRenderer::DisableBlending();
        OpenGLFrameBuffer::SetDefaultFrameBuffer();
    }
}