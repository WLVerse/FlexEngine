/*!************************************************************************
* WLVERSE [https://wlverse.web.app]
* openglspriterenderer.cpp
*
* This file implements the OpenGLSpriteRenderer class, which is responsible
* for handling 2D sprite rendering within the game engine. It provides
* functions for rendering sprites, applying post-processing effects,
* and managing the necessary OpenGL resources such as shaders and
* framebuffers.
*
* Key functionalities include:
* - Rendering 2D sprites with texture binding and transformations.
* - Supporting post-processing effects such as Gaussian Blur and Bloom.
* - Providing wrapper functions for commonly used OpenGL operations,
*   ensuring streamlined usage across the codebase.
*
* The renderer is built with a focus on performance and flexibility,
* allowing for easy customization and extension of rendering capabilities.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*   - Developed the core rendering functionalities and post-processing
*     pipeline, ensuring compatibility with the game engine's architecture.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#include "openglspriterenderer.h"

#include "FlexEngine/AssetManager/assetmanager.h" // FLX_ASSET_GET
#include "FlexEngine/DataStructures/freequeue.h"

namespace FlexEngine
{
    #pragma region Static Variables
    // static member initialization
    uint32_t OpenGLSpriteRenderer::m_draw_calls = 0;
    uint32_t OpenGLSpriteRenderer::m_draw_calls_last_frame = 0;
    uint32_t OpenGLSpriteRenderer::m_maxInstances = 3000; //Should be more than enough
    bool OpenGLSpriteRenderer::m_depth_test = false;
    bool OpenGLSpriteRenderer::m_blending = false;

    std::vector<MeshBuffer> OpenGLSpriteRenderer::m_vbos;
    std::vector<GLuint> OpenGLSpriteRenderer::m_batchSSBOs;

    const CameraManager* FlexEngine::OpenGLSpriteRenderer::m_CamM_Instance = nullptr;
    #pragma endregion

    #pragma region Wrapper Func
    /*!***************************************************************************
    * \brief
    * Retrieves the total number of draw calls made.
    *
    * \return Total draw calls as a uint32_t.
    *****************************************************************************/
    uint32_t OpenGLSpriteRenderer::GetDrawCalls() { return m_draw_calls; }
    /*!***************************************************************************
    * \brief
    * Retrieves the number of draw calls made in the last frame.
    *
    * \return Total draw calls from the last frame as a uint32_t.
    *****************************************************************************/
    uint32_t OpenGLSpriteRenderer::GetDrawCallsLastFrame() { return m_draw_calls_last_frame; }

    /*!***************************************************************************
    * \brief
    * Checks if depth testing is enabled.
    *
    * \return True if depth testing is enabled, otherwise false.
    *****************************************************************************/
    bool OpenGLSpriteRenderer::IsDepthTestEnabled() { return m_depth_test; }
    /*!***************************************************************************
    * \brief
    * Enables depth testing for rendering.
    *****************************************************************************/
    void OpenGLSpriteRenderer::EnableDepthTest()
    {
        m_depth_test = true;
        glEnable(GL_DEPTH_TEST);
    }
    /*!***************************************************************************
    * \brief
    * Disables depth testing for rendering.
    *****************************************************************************/
    void OpenGLSpriteRenderer::DisableDepthTest()
    {
        m_depth_test = false;
        glDisable(GL_DEPTH_TEST);
    }
    /*!***************************************************************************
    * \brief
    * Checks if blending is enabled.
    *
    * \return True if blending is enabled, otherwise false.
    *****************************************************************************/
    bool OpenGLSpriteRenderer::IsBlendingEnabled() { return m_blending; }
    /*!***************************************************************************
    * \brief
    * Enables blending for rendering.
    *****************************************************************************/
    void OpenGLSpriteRenderer::EnableBlending()
    {
        m_blending = true;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    /*!***************************************************************************
    * \brief
    * Disables blending for rendering.
    *****************************************************************************/
    void OpenGLSpriteRenderer::DisableBlending()
    {
        m_blending = false;
        glDisable(GL_BLEND);
    }
    
    /*!***************************************************************************
    * \brief
    * Clears the color buffer with a specified color.
    *
    * \param color The color to clear the buffer to, represented as a Vector4.
    *****************************************************************************/
    void OpenGLSpriteRenderer::ClearColor(const Vector4& color)
    {
        glClearColor(color.x, color.y, color.z, color.w);
        m_draw_calls_last_frame = m_draw_calls;
        m_draw_calls = 0;
    }
    #pragma endregion

    /*!***************************************************************************
    * \brief
    * Retrieves the VAO ID for a specified VBO type.
    *
    * \param type The type of VBO for which to retrieve the VAO ID.
    * \return The corresponding VAO ID as a GLuint.
    *****************************************************************************/
    GLuint OpenGLSpriteRenderer::GetVAO_ID(Renderer2DProps::VBO_Type type) { return m_vbos[type].vao; }

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    #pragma region Initialization
    /*!***************************************************************************
    * \brief
    * Creates a VAO and VBO with the specified vertices.
    *
    * Pls enter in the following sample vertices order:
    * float vertices[] = {
    * // Position        // TexCoords
    * -0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // Bottom-left
    *  0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // Bottom-right
    *  0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // Top-right
    *  0.5f,  0.5f, 0.0f,   0.0f, 1.0f, // Top-right
    * -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // Top-left
    * -0.5f, -0.5f, 0.0f,   1.0f, 0.0f  // Bottom-left
    * };
    *
    * \param vao Reference to the VAO ID to be created.
    * \param vbo Reference to the VBO ID to be created.
    * \param vertices Pointer to an array of vertex data.
    * \param vertexCount The number of vertices in the array.
    *****************************************************************************/
    void OpenGLSpriteRenderer::InitQuadVAO_VBO(GLuint& vao, GLuint& vbo, const float* vertices, int vertexCount)
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(0);

        // free in freequeue
        FreeQueue::Push(
          [=]()
        {
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        }
        );
    }
    
    /*!***************************************************************************
    * \brief
    * Initializes Shader Storage Buffer Objects (SSBOs) for batch rendering.
    *****************************************************************************/
    void OpenGLSpriteRenderer::InitBatchSSBO()
    {
        // Set up the SSBO for instance data
        GLuint t_tempSSBO;
        //Transformation
        glGenBuffers(1, &t_tempSSBO);
        m_batchSSBOs.emplace_back(t_tempSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Matrix4x4), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, t_tempSSBO);
        //Color_to_add
        glGenBuffers(1, &t_tempSSBO);
        m_batchSSBOs.emplace_back(t_tempSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, t_tempSSBO);
        //Color_to_multiply
        glGenBuffers(1, &t_tempSSBO);
        m_batchSSBOs.emplace_back(t_tempSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, t_tempSSBO);
        //Uv - animation
        glGenBuffers(1, &t_tempSSBO);
        m_batchSSBOs.emplace_back(t_tempSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, t_tempSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, m_maxInstances * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, t_tempSSBO);

        FreeQueue::Push(
        [=]()
        {
            for (GLuint iter : m_batchSSBOs)
                glDeleteBuffers(1, &iter);
        }
        );
    }
    /*!***************************************************************************
    * \brief
    * Initializes the VBOs.
    *
    * \param windowSize The size of the rendering window as a Vector2.
    *****************************************************************************/
    void OpenGLSpriteRenderer::Init(const Vector2& windowSize, const CameraManager* CamManager)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Create VAOs and VBOs (CAN BE DONE BETTER)
        struct VertexData {
            MeshBuffer buffer;
            const float* vertices;
            int count;
        };

        float quadVertices[] = {
            // Positions         // TexCoords
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
            -0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f
        };
        float quadInvertedVertices[] = {
            // Positions         // TexCoords
            -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 1.0f, 1.0f
        };
        float lineVertices[] =
        {
            // Positions         // TexCoords
            -0.5f, -0.5f, 0.0f, 50.0f, 0.0f,
             0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 48.0f,
             0.5f,  0.5f, 0.0f, 0.0f, 48.0f,
            -0.5f,  0.5f, 0.0f, 48.0f, 48.0f,
            -0.5f, -0.5f, 0.0f, 48.0f, 0.0f
        };
        float postProcessingVertices[] =
        {
            // Positions         // TexCoords
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f
        };

        VertexData vertexData[] =
        {
            {MeshBuffer(), quadVertices, sizeof(quadVertices) / sizeof(float)},
            {MeshBuffer(), quadInvertedVertices, sizeof(quadInvertedVertices) / sizeof(float)},
            {MeshBuffer(), lineVertices, sizeof(lineVertices) / sizeof(float)},
            {MeshBuffer(), postProcessingVertices, sizeof(postProcessingVertices) / sizeof(float)}
        };

        for (auto& data : vertexData)
        {
            InitQuadVAO_VBO(data.buffer.vao, data.buffer.vbo, data.vertices, data.count);
            m_vbos.push_back(data.buffer);
        }

        Log::Info("All VAOs & VBOs are set up.");

        /////////////////////////////////////////////////////////////////////////////////////
        // Create SSBOs
        InitBatchSSBO();
        Log::Info("All SSBOs are set up.");

        /////////////////////////////////////////////////////////////////////////////////////
        // Linking shaders
        OpenGLPostProcessing::Init(m_vbos[Renderer2DProps::VBO_Type::VBO_PProcessing].vao);

        /////////////////////////////////////////////////////////////////////////////////////
        // Create relevant FBO 
        OpenGLFrameBuffer::Init(windowSize);

        /////////////////////////////////////////////////////////////////////////////////////
        // Link Camera Manager
        m_CamM_Instance = CamManager;
    }
    #pragma endregion

    #pragma region Draw
    /*!***************************************************************************
    * \brief
    * Draws a 2D texture using the specified properties.
    *
    * \param props The properties for rendering the texture, including shader,
    *              texture, color, and transformations.
    * 
    * LEGACY - Avoid using / use batch renderer
    *****************************************************************************/
    void OpenGLSpriteRenderer::DrawTexture2D(const Renderer2DProps& props)
    {
        // Guard
        if (props.vbo_id >= m_vbos.size() || props.vbo_id < 0)
            Log::Fatal("Vbo_id is invalid. Pls Check or revert to 0.");
        if (props.shader == "" || props.transform == Matrix4x4::Zero || m_CamM_Instance->GetMainCamera() == INVALID_ENTITY_ID)
            return;

        // Bind all
        glBindVertexArray(m_vbos[props.vbo_id].vao);

        // Apply Shader
        auto& asset_shader = FLX_ASSET_GET(Asset::Shader, props.shader);
        asset_shader.Use();

        // Apply Texture
        if (props.texture != "")
        {
            asset_shader.SetUniform_bool("u_use_texture", true);
            auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.texture);
            asset_texture.Bind(asset_shader, "u_texture", 0);
        }
        else
        {
            asset_shader.SetUniform_bool("u_use_texture", false);
            asset_shader.SetUniform_vec3("u_color", props.color_to_add);
        }

        asset_shader.SetUniform_vec3("u_color_to_add", props.color_to_add);
        asset_shader.SetUniform_vec3("u_color_to_multiply", props.color_to_multiply);

        // Transformation & Orthographic Projection
        //Vector2 camPos = (CameraManager::GetMainCamera() != -1) ? (Vector2)CameraManager::GetCameraData(CameraManager::GetMainCamera())->position : Vector2::Zero;
        //static const Matrix4x4 view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
        //Matrix4x4 projection_view = Matrix4x4::Orthographic(
        //  camPos.x, camPos.x + props.window_size.x,
        //  camPos.y + props.window_size.y, camPos.y,
        //  -2.0f, 2.0f
        //) * view_matrix;
        asset_shader.SetUniform_mat4("u_projection_view", m_CamM_Instance->GetCameraData(
            OpenGLFrameBuffer::CheckSameFrameBuffer(OpenGLFrameBuffer::m_gameFBO) ? m_CamM_Instance->GetMainCamera() : m_CamM_Instance->GetEditorCamera())->proj_viewMatrix);
        asset_shader.SetUniform_mat4("u_model", props.transform);
        // Draw
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;

        glBindVertexArray(0);
    }

    //For Drawing the generate image on screen
    void OpenGLSpriteRenderer::DrawTexture2D(GLuint TextureID, const Renderer2DProps& props)
    {
        // Guard
        if (props.vbo_id >= m_vbos.size() || props.vbo_id < 0)
            Log::Fatal("Vbo_id is invalid. Pls Check or revert to 0.");
        if (props.shader == "" || props.transform == Matrix4x4::Zero)
            return;

        // Bind all
        glBindVertexArray(m_vbos[props.vbo_id].vao);

        // Apply Shader
        auto& asset_shader = FLX_ASSET_GET(Asset::Shader, props.shader);
        asset_shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureID); // Original scene texture

        // Apply Texture
        asset_shader.SetUniform_bool("u_use_texture", true);
        asset_shader.SetUniform_int("u_texture", 0);
        asset_shader.SetUniform_vec3("u_color_to_add", props.color_to_add);
        asset_shader.SetUniform_vec3("u_color_to_multiply", props.color_to_multiply);

        // Transformation & Orthographic Projection -> Must be hardcoded due to no need of change
        asset_shader.SetUniform_mat4("u_model", props.transform);
        static const Matrix4x4 view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
        Matrix4x4 projection_view = Matrix4x4::Orthographic(
          0.0f, props.window_size.x,
          props.window_size.y, 0.0f,
          -2.0f, 2.0f
        ) * view_matrix;
        asset_shader.SetUniform_mat4("u_projection_view", projection_view);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;

        glBindVertexArray(0);
    }
    
    /*!***************************************************************************
    * \brief
    * Draws an animated 2D texture with the given properties and texture coordinates.
    *
    * \param props The rendering properties, including shaders, textures, and transformations.
    * \param uv The texture coordinates for the animation frame.
    * 
    * LEGACY - Avoid using / use batch renderer
    *****************************************************************************/
    void OpenGLSpriteRenderer::DrawAnim2D(const Renderer2DProps& props, const Vector4 uv)
    {
        // Guard
        if (props.vbo_id >= m_vbos.size() || props.vbo_id < 0)
            Log::Fatal("Vbo_id is invalid. Pls Check or revert to 0.");
        if (props.shader == "" || props.transform == Matrix4x4::Zero || m_CamM_Instance->GetMainCamera() == INVALID_ENTITY_ID)
            return;

        // Bind all
        glBindVertexArray(m_vbos[props.vbo_id].vao);

        // Apply Shader
        auto& asset_shader = FLX_ASSET_GET(Asset::Shader, props.shader);
        asset_shader.Use();

        // Apply Texture
        if (props.texture != "")
        {
            asset_shader.SetUniform_bool("u_use_texture", true);
            auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.texture);
            //std::cout << props.texture << "\n";
            asset_texture.Bind(asset_shader, "u_texture", 0);
        }
        else
        {
            asset_shader.SetUniform_bool("u_use_texture", false);
            asset_shader.SetUniform_vec3("u_color", props.color_to_add);
        }

        asset_shader.SetUniform_vec3("u_color_to_add", props.color_to_add);
        asset_shader.SetUniform_vec3("u_color_to_multiply", props.color_to_multiply);
        float u_min = uv.x;
        float v_min = uv.y;
        float u_max = uv.z;
        float v_max = uv.w;
        asset_shader.SetUniform_vec2("u_UvMin", Vector2{ u_min, v_min });
        asset_shader.SetUniform_vec2("u_UvMax", Vector2{ u_max, v_max });

        // Transformation & Orthographic Projection
        asset_shader.SetUniform_mat4("u_projection_view", m_CamM_Instance->GetCameraData(
            OpenGLFrameBuffer::CheckSameFrameBuffer(OpenGLFrameBuffer::m_gameFBO) ? m_CamM_Instance->GetMainCamera() : m_CamM_Instance->GetEditorCamera()
        )->proj_viewMatrix);
        asset_shader.SetUniform_mat4("u_model", props.transform);
        // Draw
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;

        glBindVertexArray(0);
    }

    /*!***************************************************************************
    * \brief
    * Draws a batch of 2D textures using the specified properties and batch data.
    *
    * \param props The rendering properties, including shaders, textures, and transformations.
    * \param data The batch instance data including transformation and color information.
    *****************************************************************************/
    void OpenGLSpriteRenderer::DrawBatchTexture2D(const Renderer2DProps& props, const Sprite_Batch_Inst& data)
    {
        // Guard
        if (data.m_transformationData.size() != data.m_colorAddData.size() ||
            data.m_colorAddData.size() != data.m_colorMultiplyData.size())
        {
            Log::Fatal("Instance batch data block is invalid (Check if all vectors are of same size)");
        }
        else if (data.m_transformationData.size() < 1)
        {
            Log::Debug("Instance batch data block is empty. Should not run render on this texture");
            return;
        }
        if (props.shader == "" || 
            (OpenGLFrameBuffer::CheckSameFrameBuffer(OpenGLFrameBuffer::m_gameFBO) && m_CamM_Instance->GetMainCamera() == INVALID_ENTITY_ID) ||
            (OpenGLFrameBuffer::CheckSameFrameBuffer(OpenGLFrameBuffer::m_editorFBO) && m_CamM_Instance->GetEditorCamera() == INVALID_ENTITY_ID))
            return;
        GLsizei dataSize = (GLsizei)data.m_transformationData.size();

        // Bind all
        glBindVertexArray(m_vbos[props.vbo_id].vao);

        // Apply Shader
        auto& asset_shader = FLX_ASSET_GET(Asset::Shader, "\\shaders\\batchtexture");
        asset_shader.Use();

        // Apply Texture
        if (props.texture != "")
        {
            asset_shader.SetUniform_bool("u_use_texture", true);
            auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.texture);
            asset_texture.Bind(asset_shader, "u_texture", 0);
        }
        else
        {
            asset_shader.SetUniform_bool("u_use_texture", false);
        }

        //Apply SSBOs
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_batchSSBOs[0]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(Matrix4x4), data.m_transformationData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_batchSSBOs[1]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(Vector3), data.m_colorAddData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_batchSSBOs[2]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(Vector3), data.m_colorMultiplyData.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_batchSSBOs[3]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dataSize * sizeof(Vector4), data.m_UVmap.data());
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        // Orthographic Projection
        asset_shader.SetUniform_mat4("u_projection_view", m_CamM_Instance->GetCameraData(
            OpenGLFrameBuffer::CheckSameFrameBuffer(OpenGLFrameBuffer::m_gameFBO) ? m_CamM_Instance->GetMainCamera() : m_CamM_Instance->GetEditorCamera()
            )->proj_viewMatrix);
        // Draw
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, dataSize);
        m_draw_calls++;

        glBindVertexArray(0);
    }

    /*!***************************************************************************
    * \brief
    * Draws the post-processing layer after all other rendering operations.
    *****************************************************************************/
    void OpenGLSpriteRenderer::DrawPostProcessingLayer()
    {
        OpenGLFrameBuffer::SetPostProcessingFrameBuffer(); // Initial Frame Buffer Setup

        //CURRENT POST-PROCESS(PP) BEING HANDLED:
        // 1) Reflection / Gloss
        {
            //Pending
        }
        // 2) Bloom
        {
            // Step 1: Set to bloom frame buffer -> you do not want to mess with the other effects textures
            OpenGLFrameBuffer::SetBloomFrameBuffer();

            // Step 2: Brightness Extraction
            OpenGLPostProcessing::ApplyBrightnessPass(0.55f);

            // Step 3: Gaussian Blur
            OpenGLPostProcessing::ApplyGaussianBlur(4, 10.0f, 12);

            // Step 4: Final Composition
            OpenGLPostProcessing::ApplyBloomFinalComposition(0.8f);
        }
        // 3) Blur / Focal Adjust
        {
            //Pending
        }
        // 4) Lens Flare
        {
            //Pending
        }

        // Clean-up
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, drawBuffers);
        glBindVertexArray(0);
    }
    #pragma endregion
}