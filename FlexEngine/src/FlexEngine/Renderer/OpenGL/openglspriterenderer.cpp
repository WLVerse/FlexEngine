/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// openglspriterenderer.cpp
//
// This file implements the OpenGLSpriteRenderer class, which is responsible
// for handling 2D sprite rendering within the game engine. It provides
// functions for rendering sprites, applying post-processing effects,
// and managing the necessary OpenGL resources such as shaders and
// framebuffers.
//
// Key functionalities include:
// - Rendering 2D sprites with texture binding and transformations.
// - Supporting post-processing effects such as Gaussian Blur and Bloom.
// - Providing wrapper functions for commonly used OpenGL operations,
//   ensuring streamlined usage across the codebase.
//
// The renderer is built with a focus on performance and flexibility,
// allowing for easy customization and extension of rendering capabilities.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//   - Developed the core rendering functionalities and post-processing
//     pipeline, ensuring compatibility with the game engine's architecture.
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#include "openglspriterenderer.h"

#include "FlexEngine/AssetManager/assetmanager.h" // FLX_ASSET_GET
#include "FlexEngine/DataStructures/freequeue.h"

namespace FlexEngine
{

    // static member initialization
    uint32_t OpenGLSpriteRenderer::m_draw_calls = 0;
    uint32_t OpenGLSpriteRenderer::m_draw_calls_last_frame = 0;
    bool OpenGLSpriteRenderer::m_depth_test = false;
    bool OpenGLSpriteRenderer::m_blending = false;
    
    std::vector<VertexBufferObject> OpenGLSpriteRenderer::m_vbos;

    GLuint OpenGLSpriteRenderer::samples = 8;
    float OpenGLSpriteRenderer::gamma = 2.2f;
    GLuint OpenGLSpriteRenderer::m_rectVAO = 0;
    GLuint OpenGLSpriteRenderer::m_rectVBO = 0;
    GLuint OpenGLSpriteRenderer::m_postProcessingFBO = 0;
    GLuint OpenGLSpriteRenderer::m_colorBuffer = 0;
    GLuint OpenGLSpriteRenderer::bloomTexture = 0;
    GLuint OpenGLSpriteRenderer::postProcessingTexture = 0;
    GLuint OpenGLSpriteRenderer::m_brightBuffer = 0;
    GLuint OpenGLSpriteRenderer::m_pingpongFBO[2] = {};
    GLuint OpenGLSpriteRenderer::m_pingpongBuffer[2] = {};

    //////////////////////////////////////////////////////////////////////////////////////////
    // WRAPPER FUNCTIONS
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
    * Enables post-processing effects for rendering.
    *****************************************************************************/
    void OpenGLSpriteRenderer::EnablePostProcessing() { glBindFramebuffer(GL_FRAMEBUFFER, m_postProcessingFBO);}
    /*!***************************************************************************
    * \brief
    * Disables post-processing effects for rendering.
    *****************************************************************************/
    void OpenGLSpriteRenderer::DisablePostProcessing() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
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
    * Clears the current framebuffer.
    *****************************************************************************/
    void OpenGLSpriteRenderer::ClearFrameBuffer() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }
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
    void OpenGLSpriteRenderer::CreateVAOandVBO(GLuint& vao, GLuint& vbo, const float* vertices, int vertexCount) 
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
    * Initializes the VBOs.
    *
    * \param windowSize The size of the rendering window as a Vector2.
    *****************************************************************************/
    void OpenGLSpriteRenderer::Init(const Vector2& windowSize)
    {
        VertexBufferObject basic;
        float vert[] = {
            // Position           // TexCoords
            -0.5f, -0.5f, 0.0f,   1.0f, 0.0f,  // Bottom-left
             0.5f, -0.5f, 0.0f,   0.0f, 0.0f,  // Bottom-right
             0.5f,  0.5f, 0.0f,   0.0f, 1.0f,  // Top-right
             0.5f,  0.5f, 0.0f,   0.0f, 1.0f,  // Top-right
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f,  // Top-left
            -0.5f, -0.5f, 0.0f,   1.0f, 0.0f   // Bottom-left
        };
        CreateVAOandVBO(basic.vao, basic.vbo, vert, sizeof(vert) / sizeof(float));
        m_vbos.push_back(basic);

        VertexBufferObject line;
        float vert_1[] = {
            // Position           // TexCoords
            -0.5f, -0.5f, 0.0f,   50.0f, 0.0f,  // Bottom-left
             0.5f, -0.5f, 0.0f,   0.0f, 0.0f,  // Bottom-right
             0.5f,  0.5f, 0.0f,   0.0f, 48.0f,  // Top-right
             0.5f,  0.5f, 0.0f,   0.0f, 48.0f,  // Top-right
            -0.5f,  0.5f, 0.0f,   48.0f, 48.0f,  // Top-left
            -0.5f, -0.5f, 0.0f,   48.0f, 0.0f   // Bottom-left
        };
        CreateVAOandVBO(line.vao, line.vbo, vert_1, sizeof(vert_1) / sizeof(float));
        m_vbos.push_back(line);

        ////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////
        {
            // EVERYTHING BELOW HERE IS STILL UNDER DEVELOPMENT
            auto& asset_shader = FLX_ASSET_GET(Asset::Shader, R"(/shaders/test)");
            asset_shader.Use();
            glUniform1i(glGetUniformLocation(asset_shader.Get(), "screenTexture"), 0);
            glUniform1i(glGetUniformLocation(asset_shader.Get(), "bloomTexture"), 1);
            glUniform1f(glGetUniformLocation(asset_shader.Get(), "gamma"), gamma);
            asset_shader = FLX_ASSET_GET(Asset::Shader, R"(/shaders/GaussianBlur)");
            //auto& temp = FLX_ASSET_GET(Asset::Shader, R"(/shaders/test)");
            asset_shader.Use();
            glUniform1i(glGetUniformLocation(asset_shader.Get(), "screenTexture"), 0);


            const float vertices[] = {
                // Position        // TexCoords
                -1.0f, -1.f, 0.0f,   0.0f, 0.0f, // Bottom-left
                 1.0f, -1.f, 0.0f,   1.0f, 0.0f, // Bottom-right
                 1.0f,  1.f, 0.0f,   1.0f, 1.0f, // Top-right
                 1.0f,  1.f, 0.0f,   1.0f, 1.0f, // Top-right
                -1.0f,  1.f, 0.0f,   0.0f, 1.0f, // Top-left
                -1.0f, -1.f, 0.0f,   0.0f, 0.0f  // Bottom-left
            };
            //Change to 1,
            //Disable for now
            {

                //// Enables the Depth Buffer
                //glEnable(GL_DEPTH_TEST);

                //// Enables Multisampling
                //glEnable(GL_MULTISAMPLE);

                //// Enables Cull Facing
                //glEnable(GL_CULL_FACE);
                //// Keeps front faces
                //glCullFace(GL_FRONT);
                //// Uses counter clock-wise standard
                //glFrontFace(GL_CCW);
            }

            // Set up VAO and VBO
            glGenVertexArrays(1, &m_rectVAO);
            glGenBuffers(1, &m_rectVBO);
            glBindVertexArray(m_rectVAO);
            glBindBuffer(GL_ARRAY_BUFFER, m_rectVBO);
            // Correct the usage of vertices in glBufferData
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            // Position attribute (3 floats: x, y, z)
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(5 * sizeof(float)), (void*)0); // 5 * sizeof(float) is correct for the stride.
            // Texture coordinates attribute (2 floats: u, v), starting after the position
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, static_cast<GLsizei>(5 * sizeof(float)), (void*)(3 * sizeof(float))); // Offset to the 4th element (3 floats).

            // Create Frame Buffer Object
            glGenFramebuffers(1, &m_postProcessingFBO);
            glBindFramebuffer(GL_FRAMEBUFFER, m_postProcessingFBO);

            // Create Framebuffer Texture
            float width, height;
            width = windowSize.x;
            height = windowSize.y;
            glGenTextures(1, &postProcessingTexture);
            glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

            // Create Second Framebuffer Texture
            glGenTextures(1, &bloomTexture);
            glBindTexture(GL_TEXTURE_2D, bloomTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bloomTexture, 0);

            // Tell OpenGL we need to draw to both attachments
            unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
            glDrawBuffers(2, attachments);

            // Error checking framebuffer
            auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
                Log::Fatal("Post-Processing Framebuffer error: " + fboStatus);

            // Create Ping Pong Framebuffers for repetitive blurring
            glGenFramebuffers(2, m_pingpongFBO);
            glGenTextures(2, m_pingpongBuffer);
            for (unsigned int i = 0; i < 2; i++)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[i]);
                glBindTexture(GL_TEXTURE_2D, m_pingpongBuffer[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(width), static_cast<GLsizei>(height), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongBuffer[i], 0);

                fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
                    Log::Fatal("Ping-Pong Framebuffer error: " + fboStatus);
            }

            //Unbind frame buffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    /*!***************************************************************************
    * \brief
    * Draws a 2D texture using the specified properties.
    *
    * \param props The properties for rendering the texture, including shader,
    *              texture, color, and transformations.
    *****************************************************************************/
    void OpenGLSpriteRenderer::DrawTexture2D(const Renderer2DProps& props)
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

        // Apply Texture
        if (props.texture != "")
        {
            asset_shader.SetUniform_bool("u_use_texture", true);
            auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.texture);
            asset_texture.Bind(asset_shader, "u_texture", 0);
        }
        else if (props.color != Vector3::Zero)
        {
            asset_shader.SetUniform_bool("u_use_texture", false);
            asset_shader.SetUniform_vec3("u_color", props.color);
        }
        else
        {
            Log::Fatal("No texture or color specified for texture shader.");
        }
        asset_shader.SetUniform_vec3("u_color_to_add", props.color_to_add);
        asset_shader.SetUniform_vec3("u_color_to_multiply", props.color_to_multiply);

        // Transformation & Orthographic Projection
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
    * Draws the post-processing layer after all other rendering operations.
    *****************************************************************************/
    void OpenGLSpriteRenderer::DrawPostProcessingLayer()
    {
        // Step 1: Full-screen quad vertices for post-processing (unchanged)
        static const float vertices[] = {
            -0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
             0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
             0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
             0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
            -0.5f, -0.5f, 0.0f,   1.0f, 0.0f
        };

        // Step 2: Render the texture (offscreen rendering)
        glBindFramebuffer(GL_FRAMEBUFFER, m_postProcessingFBO);

        // Step 3: Brightness Pass - Extract bright areas
        auto& brightness_shader = FLX_ASSET_GET(Asset::Shader, R"(/shaders/brightness)");
        brightness_shader.Use();
        brightness_shader.SetUniform_float("u_Threshold", 0.55f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bloomTexture); // Texture rendered in the previous step (scene texture)
        brightness_shader.SetUniform_int("scene", 0);

        // Render brightness to first ping-pong buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[0]); // Use ping-pong framebuffer for blur
        glBindVertexArray(m_rectVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;

        // Step 4: Gaussian Blur Pass
        auto& blur_shader = FLX_ASSET_GET(Asset::Shader, R"(/shaders/GaussianBlur)");
        blur_shader.Use();

        bool horizontal = true;
        int blur_passes = 2;

        for (int i = 0; i < blur_passes; ++i)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_pingpongFBO[horizontal]);
            blur_shader.SetUniform_int("horizontal", horizontal);

            if (i == 0) {
                glBindTexture(GL_TEXTURE_2D, m_pingpongBuffer[0]);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, m_pingpongBuffer[!horizontal]);
            }

            glBindVertexArray(m_rectVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            m_draw_calls++;
            horizontal = !horizontal;
        }

        // Step 5: Final Composition
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind default framebuffer

        auto& final_shader = FLX_ASSET_GET(Asset::Shader, R"(/shaders/test)");
        final_shader.Use();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcessingTexture); // Original scene texture
        final_shader.SetUniform_int("screenTexture", 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_pingpongBuffer[!horizontal]); // Final blurred texture
        final_shader.SetUniform_int("bloomTexture", 1);



        glBindVertexArray(m_rectVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;
        //}

        // Clean-up
        glBindVertexArray(0);
    }

}