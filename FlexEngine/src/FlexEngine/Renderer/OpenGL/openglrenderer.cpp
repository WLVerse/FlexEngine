/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * openglrenderer.cpp
 *
 * This file declares the OpenGLRenderer class and related structures used for
 * 2D rendering operations within the engine. It includes definitions for
 * rendering properties, text rendering configurations, and various helper
 * functions for drawing textures and text using OpenGL.
 *
 * Key functionalities include:
 * - Configuration structures for 2D rendering properties and text.
 * - Static functions for managing OpenGL state, such as depth testing and blending.
 * - Helper functions to render textures and text on a unit square mesh.
 *
 * AUTHORS
 * [50%] Chan Wen Loong (wenloong.c@digipen.edu)
 *   - Main Author
 * [50%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Main Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/
#include "openglrenderer.h"

#include "assetmanager.h" // FLX_ASSET_GET
#include "DataStructures/freequeue.h"

#include "FlexEngine/FlexMath/quaternion.h"

namespace FlexEngine
{
    // Static member initialization
    uint32_t OpenGLRenderer::m_draw_calls = 0;
    uint32_t OpenGLRenderer::m_draw_calls_last_frame = 0;
    bool OpenGLRenderer::m_depth_test = false;
    bool OpenGLRenderer::m_blending = false;

    /*!
     * @brief Retrieves the number of draw calls issued in the current frame.
     *
     * @return The current frame's draw call count.
     */
    uint32_t OpenGLRenderer::GetDrawCalls()
    {
        return m_draw_calls;
    }

    /*!
     * @brief Retrieves the number of draw calls issued in the last frame.
     *
     * @return The last frame's draw call count.
     */
    uint32_t OpenGLRenderer::GetDrawCallsLastFrame()
    {
        return m_draw_calls_last_frame;
    }

    /*!
     * @brief Checks if depth testing is enabled.
     *
     * @return True if depth testing is enabled, false otherwise.
     */
    bool OpenGLRenderer::IsDepthTestEnabled()
    {
        return m_depth_test;
    }

    /*!
     * @brief Enables OpenGL depth testing.
     */
    void OpenGLRenderer::EnableDepthTest()
    {
        m_depth_test = true;
        glEnable(GL_DEPTH_TEST);
    }

    /*!
     * @brief Disables OpenGL depth testing.
     */
    void OpenGLRenderer::DisableDepthTest()
    {
        m_depth_test = false;
        glDisable(GL_DEPTH_TEST);
    }

    /*!
     * @brief Checks if blending is enabled.
     *
     * @return True if blending is enabled, false otherwise.
     */
    bool OpenGLRenderer::IsBlendingEnabled()
    {
        return m_blending;
    }

    /*!
     * @brief Enables OpenGL blending.
     */
    void OpenGLRenderer::EnableBlending()
    {
        m_blending = true;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /*!
     * @brief Disables OpenGL blending.
     */
    void OpenGLRenderer::DisableBlending()
    {
        m_blending = false;
        glDisable(GL_BLEND);
    }

    /*!
     * @brief Clears the current framebuffer.
     *
     * This function clears the color and depth buffers of the current framebuffer.
     */
    void OpenGLRenderer::ClearFrameBuffer()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    /*!
     * @brief Sets the clear color for the framebuffer.
     *
     * @param color The color to use when clearing the framebuffer.
     */
    void OpenGLRenderer::ClearColor(const Vector4& color)
    {
        glClearColor(color.x, color.y, color.z, color.w);
        m_draw_calls_last_frame = m_draw_calls;
        m_draw_calls = 0;
    }

    /*!
     * @brief Issues a draw call for the specified number of vertices.
     *
     * @param size The number of vertices to draw.
     */
    void OpenGLRenderer::Draw(GLsizei size)
    {
        glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
        m_draw_calls++;
    }

    /*!
     * @brief Draws a 2D texture using the specified rendering properties and camera.
     *
     * This function renders a texture on a unit square mesh. It configures vertex arrays,
     * sets up texture coordinates (including spritesheet handling), and binds the appropriate
     * shader and texture before issuing the draw call.
     *
     * @param props Rendering properties including shader, asset, transformation parameters, and color settings.
     * @param camID The camera entity ID to use for rendering.
     */
    void OpenGLRenderer::DrawTexture2D(const Renderer2DProps& props, const FlexECS::EntityID camID)
    {
        // unit square vertices (position only)
        static const float vertices[] = {
            // Position
            -0.5f, -0.5f, 0.0f,   // Bottom-left
             0.5f, -0.5f, 0.0f,   // Bottom-right
             0.5f,  0.5f, 0.0f,   // Top-right
             0.5f,  0.5f, 0.0f,   // Top-right
            -0.5f,  0.5f, 0.0f,   // Top-left
            -0.5f, -0.5f, 0.0f,   // Bottom-left
        };

        static GLuint vao = 0, vbo = 0;

        if (vao == 0)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindVertexArray(0);

            // Register cleanup of VAO and VBO in FreeQueue
            FreeQueue::Push(
              [=]()
            {
                glDeleteBuffers(1, &vbo);
                glDeleteVertexArrays(1, &vao);
            }
            );
        }

        // Guard against invalid state
        if (vao == 0 || props.scale == Vector2::Zero) return;

        // Setup texture coordinates
        bool is_spritesheet = props.texture_index >= 0;

        float tex_coords[12] = {
          0.0f, 1.0f, // Bottom-left
          1.0f, 1.0f, // Bottom-right
          1.0f, 0.0f, // Top-right
          1.0f, 0.0f, // Top-right
          0.0f, 0.0f, // Top-left
          0.0f, 1.0f  // Bottom-left
        };

        if (is_spritesheet)
        {
            auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, props.asset);
            auto uv = asset_spritesheet.GetUV(props.texture_index);
            tex_coords[0] = uv.x;
            tex_coords[1] = uv.y;
            tex_coords[2] = uv.z;
            tex_coords[3] = uv.y;
            tex_coords[4] = uv.z;
            tex_coords[5] = uv.w;
            tex_coords[6] = uv.z;
            tex_coords[7] = uv.w;
            tex_coords[8] = uv.x;
            tex_coords[9] = uv.w;
            tex_coords[10] = uv.x;
            tex_coords[11] = uv.y;
        }

        GLuint vbo_uv = 0;
        glGenBuffers(1, &vbo_uv);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        glBindVertexArray(0);

        // Register cleanup of the UV buffer in FreeQueue
        FreeQueue::Push(
          [=]()
        {
            glDeleteBuffers(1, &vbo_uv);
        },
          "Free UV buffer"
        );

        // Bind vertex array and use the shader
        glBindVertexArray(vao);
        auto& asset_shader = AssetManager::Get<Asset::Shader>(props.shader);
        asset_shader.Use();

        // Bind texture or set color based on properties
        if (props.asset != "")
        {
            if (!is_spritesheet)
            {
                asset_shader.SetUniform_bool("u_use_texture", true);
                auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.asset);
                asset_texture.Bind(asset_shader, "u_texture", 0);
            }
            else
            {
                asset_shader.SetUniform_bool("u_use_texture", true);
                auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, props.asset);
                auto& asset_texture = FLX_ASSET_GET(Asset::Texture, asset_spritesheet.texture);
                asset_texture.Bind(asset_shader, "u_texture", 0);
            }
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
        asset_shader.SetUniform_float("u_alpha", props.alpha);

        // Adjust position based on alignment
        Vector2 position = Vector2(props.position.x, props.position.y);
        switch (props.alignment)
        {
        case Renderer2DProps::Alignment_TopLeft:
            position += props.scale * 0.5f;
            break;
        case Renderer2DProps::Alignment_Center:
        default:
            break;
        }

        // Setup model matrix with translation, rotation, and scale
        Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, props.position);
        Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(props.rotation).ToRotationMatrix();
        Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, props.scale);
        asset_shader.SetUniform_mat4("u_model", translation_matrix * rotation_matrix * scale_matrix);

        // Setup projection-view matrix based on the specified camera
        asset_shader.SetUniform_mat4("u_projection_view",
          CameraManager::HasCamera(camID) ? CameraManager::GetCamera(camID)->GetProjViewMatrix() :
          CameraManager::GetEditorCamera()->GetProjViewMatrix()
        );

        // Draw the unit square mesh
        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;

        // Error checking
        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            Log::Fatal("OpenGL Error: " + std::to_string(error));
        }

        glBindVertexArray(0);

        // Execute cleanup for the UV buffer
        FreeQueue::RemoveAndExecute("Free UV buffer");
    }

    /*!
     * @brief Draws 2D text as a texture using the specified text rendering properties and camera.
     *
     * This function renders text on a unit square mesh. It sets up the necessary buffers,
     * binds the text shader, and processes the string to render each glyph according to the
     * provided text properties and alignment.
     *
     * @param text Text rendering properties including shader, font type, color, transformation, and alignment.
     * @param camID The camera entity ID to use for rendering.
     */
    void OpenGLRenderer::DrawTexture2D(const Renderer2DText& text, const FlexECS::EntityID camID)
    {
        static GLuint vao = 0, vbo = 0;

        if (vao == 0)
        {
            // Configure VAO/VBO for text quads
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // 6 vertices per quad

            // Configure vertex attributes
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            FreeQueue::Push(
              [=]()
            {
                glDeleteVertexArrays(1, &vao);
                glDeleteBuffers(1, &vbo);
            }
            );
        }

        // Guard against invalid state
        if (vao == 0 || text.m_shader == "") return;

        if (text.m_fonttype.empty())
        {
            Log::Info("Text Renderer: Unknown font type! Please check what you wrote!");
            return;
        }

        auto& asset_shader = FLX_ASSET_GET(Asset::Shader, text.m_shader);
        asset_shader.Use();

        asset_shader.SetUniform_vec3("u_color", text.m_color);
        asset_shader.SetUniform_mat4("u_model", text.m_transform);
        asset_shader.SetUniform_mat4("projection",
            CameraManager::HasCamera(camID) ? CameraManager::GetCamera(camID)->GetProjViewMatrix() :
            CameraManager::GetEditorCamera()->GetProjViewMatrix()
        );

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(vao);
        auto& asset_font = FLX_ASSET_GET(Asset::Font, text.m_fonttype);

        // Lambda to render a single glyph
        auto renderGlyph = [&](const Asset::Glyph& glyph, const Vector3& position)
        {
            glBindTexture(GL_TEXTURE_2D, glyph.textureID);

            float xpos = position.x + glyph.bearing.x;
            float ypos = position.y - (glyph.bearing.y - glyph.size.y);
            float w = glyph.size.x;
            float h = -glyph.size.y;

            float quadVertices[6][4] =
            {
                {xpos, -ypos - h, 0.0f, 0.0f},
                {xpos, -ypos, 0.0f, 1.0f},
                {xpos + w, -ypos, 1.0f, 1.0f},
                {xpos, -ypos - h, 0.0f, 0.0f},
                {xpos + w, -ypos, 1.0f, 1.0f},
                {xpos + w, -ypos - h, 1.0f, 0.0f}
            };

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            m_draw_calls++;
        };

        // Function to calculate text box dimensions
        auto calculateTextBoxDimensions = [&](const std::string& words) -> std::pair<float, float>
        {
            float totalHeight = 0.0f, maxLineHeight = asset_font.GetGlyph('A').size.y;
            float maxWidth = 0.0f, currentLineWidth = 0.0f;

            for (char c : words)
            {
                if (c == '\n')
                {
                    maxWidth = std::max(maxWidth, currentLineWidth);
                    totalHeight += maxLineHeight;
                    currentLineWidth = 0.0f;
                    maxLineHeight = asset_font.GetGlyph('A').size.y;
                    continue;
                }

                const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                currentLineWidth += glyph.advance + static_cast<int>(text.m_letterspacing);
                maxLineHeight = std::max(maxLineHeight, glyph.size.y);

                if (currentLineWidth > text.m_textboxDimensions.x)
                {
                    maxWidth = std::max(maxWidth, currentLineWidth - glyph.advance);
                    totalHeight += maxLineHeight;
                    currentLineWidth = static_cast<float>(glyph.advance) + text.m_letterspacing;
                    maxLineHeight = glyph.size.y;

                    if (totalHeight + maxLineHeight > text.m_textboxDimensions.y)
                    {
                        totalHeight -= maxLineHeight;
                        break;
                    }
                }
            }

            if (currentLineWidth > 0)
            {
                maxWidth = std::max(maxWidth, currentLineWidth);
                totalHeight += maxLineHeight;
            }

            return { std::min(maxWidth, text.m_textboxDimensions.x), std::min(totalHeight, text.m_textboxDimensions.y) };
        };

        // Function to set alignment
        auto getAlignmentOffset = [&](std::pair<Renderer2DText::AlignmentX, Renderer2DText::AlignmentY> alignment, const std::string& words) -> Vector2
        {
            auto [lineWidth, totalHeight] = calculateTextBoxDimensions(words);

            float alignXOffset = (alignment.first == Renderer2DText::Alignment_Center) ? -lineWidth / 2.0f :
                (alignment.first == Renderer2DText::Alignment_Right) ? -lineWidth : 0.0f;

            float alignYOffset = (alignment.second == Renderer2DText::Alignment_Middle) ? -totalHeight / 2.0f :
                (alignment.second == Renderer2DText::Alignment_Bottom) ? -totalHeight : 0.0f;

            return { alignXOffset, alignYOffset };
        };

        // Lambda to split the input string into words
        auto splitIntoWords = [&](const std::string& input) -> std::vector<std::string>
        {
            std::vector<std::string> words;
            std::string currentWord;
            for (char c : input)
            {
                if (c == ' ' || c == '\n')
                {
                    if (!currentWord.empty())
                    {
                        words.push_back(currentWord);
                        currentWord.clear();
                    }
                    if (c == '\n')
                        words.push_back("\n");
                }
                else
                    currentWord += c;
            }
            if (!currentWord.empty())
                words.push_back(currentWord);

            return words;
        };

        // Refactored text rendering logic (Primitive Text Scrolling)
        {
            auto words = splitIntoWords(text.m_words);
            Vector2 pen_pos = getAlignmentOffset(text.m_alignment, text.m_words);
            float lineWidth = 0.0f, totalHeight = 0.0f, maxLineHeight = 0.0f;
            const Asset::Glyph& space = asset_font.GetGlyph(' ');
            std::string currentLine;

            auto renderLine = [&](const std::string& line)
            {
                pen_pos.x = getAlignmentOffset(text.m_alignment, line).x;
                for (char c : line)
                {
                    const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                    renderGlyph(glyph, pen_pos);
                    pen_pos.x += glyph.advance + text.m_letterspacing;
                }
            };

            for (const auto& word : words)
            {
                if (word == "\n")
                {
                    renderLine(currentLine);
                    pen_pos.y += maxLineHeight + text.m_linespacing;
                    lineWidth = 0.0f;
                    maxLineHeight = 0.0f;
                    currentLine.clear();
                    continue;
                }

                float wordWidth = 0.0f;
                float wordHeight = 0.0f;
                for (char c : word)
                {
                    const Asset::Glyph& glyph = asset_font.GetGlyph(c);
                    wordWidth += glyph.advance + text.m_letterspacing;
                    wordHeight = std::max(wordHeight, glyph.size.y);
                }

                if (lineWidth + wordWidth > text.m_textboxDimensions.x)
                {
                    renderLine(currentLine);
                    pen_pos.y += maxLineHeight + text.m_linespacing;
                    totalHeight += maxLineHeight + text.m_linespacing;
                    lineWidth = 0.0f;
                    maxLineHeight = 0.0f;

                    if (totalHeight + wordHeight > text.m_textboxDimensions.y)
                    {
                        currentLine.clear();
                        break;
                    }

                    currentLine = word;
                    lineWidth = wordWidth;
                    maxLineHeight = wordHeight;
                }
                else
                {
                    if (!currentLine.empty()) currentLine += " ";
                    currentLine += word;
                    lineWidth += wordWidth + space.advance + text.m_letterspacing;
                    maxLineHeight = std::max(maxLineHeight, wordHeight);
                }
            }

            if (!currentLine.empty()) renderLine(currentLine);
        }

        // Cleanup state
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    /*!
     * @brief Draws a 2D texture using the specified rendering properties and a provided camera configuration.
     *
     * This overloaded function renders a texture on a unit square mesh using the given camera's projection-view matrix.
     *
     * @param props Rendering properties for the texture.
     * @param cameraData Camera configuration data for rendering.
     */
    void OpenGLRenderer::DrawTexture2D(const Renderer2DProps& props, const Camera& cameraData)
    {
        // unit square vertices (position only)
        static const float vertices[] = {
            // Position
            -0.5f, -0.5f, 0.0f,   // Bottom-left
             0.5f, -0.5f, 0.0f,   // Bottom-right
             0.5f,  0.5f, 0.0f,   // Top-right
             0.5f,  0.5f, 0.0f,   // Top-right
            -0.5f,  0.5f, 0.0f,   // Top-left
            -0.5f, -0.5f, 0.0f,   // Bottom-left
        };

        static GLuint vao = 0, vbo = 0;

        if (vao == 0)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

            glBindVertexArray(0);

            FreeQueue::Push(
              [=]()
            {
                glDeleteBuffers(1, &vbo);
                glDeleteVertexArrays(1, &vao);
            }
            );
        }

        if (vao == 0 || props.scale == Vector2::Zero) return;

        bool is_spritesheet = props.texture_index >= 0;
        float tex_coords[12] = {
          0.0f, 1.0f, // Bottom-left
          1.0f, 1.0f, // Bottom-right
          1.0f, 0.0f, // Top-right
          1.0f, 0.0f, // Top-right
          0.0f, 0.0f, // Top-left
          0.0f, 1.0f  // Bottom-left
        };

        if (is_spritesheet)
        {
            auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, props.asset);
            auto uv = asset_spritesheet.GetUV(props.texture_index);
            tex_coords[0] = uv.x;
            tex_coords[1] = uv.y;
            tex_coords[2] = uv.z;
            tex_coords[3] = uv.y;
            tex_coords[4] = uv.z;
            tex_coords[5] = uv.w;
            tex_coords[6] = uv.z;
            tex_coords[7] = uv.w;
            tex_coords[8] = uv.x;
            tex_coords[9] = uv.w;
            tex_coords[10] = uv.x;
            tex_coords[11] = uv.y;
        }

        GLuint vbo_uv = 0;
        glGenBuffers(1, &vbo_uv);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_uv);
        glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

        glBindVertexArray(0);

        FreeQueue::Push(
          [=]()
        {
            glDeleteBuffers(1, &vbo_uv);
        },
          "Free UV buffer"
        );

        glBindVertexArray(vao);
        auto& asset_shader = AssetManager::Get<Asset::Shader>(props.shader);
        asset_shader.Use();

        if (props.asset != "")
        {
            if (!is_spritesheet)
            {
                asset_shader.SetUniform_bool("u_use_texture", true);
                auto& asset_texture = FLX_ASSET_GET(Asset::Texture, props.asset);
                asset_texture.Bind(asset_shader, "u_texture", 0);
            }
            else
            {
                asset_shader.SetUniform_bool("u_use_texture", true);
                auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, props.asset);
                auto& asset_texture = FLX_ASSET_GET(Asset::Texture, asset_spritesheet.texture);
                asset_texture.Bind(asset_shader, "u_texture", 0);
            }
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
        asset_shader.SetUniform_float("u_alpha", props.alpha);

        Vector2 position = Vector2(props.position.x, props.position.y);
        switch (props.alignment)
        {
        case Renderer2DProps::Alignment_TopLeft:
            position += props.scale * 0.5f;
            break;
        case Renderer2DProps::Alignment_Center:
        default:
            break;
        }

        Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, props.position);
        Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(props.rotation).ToRotationMatrix();
        Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, props.scale);
        asset_shader.SetUniform_mat4("u_model", translation_matrix * rotation_matrix * scale_matrix);

        asset_shader.SetUniform_mat4("u_projection_view", cameraData.GetProjViewMatrix());

        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            Log::Fatal("OpenGL Error: " + std::to_string(error));
        }

        glBindVertexArray(0);
        FreeQueue::RemoveAndExecute("Free UV buffer");
    }

    /*!
     * @brief Draws a simple 2D texture with minimal configuration.
     *
     * This lightweight function renders a texture on a unit square mesh using the default
     * texture shader. It is intended for cases where minimal configuration is required.
     *
     * @param texture The texture asset to draw. Defaults to Asset::Texture::None.
     * @param position The position where the texture should be drawn.
     * @param scale The scale to apply to the drawn texture.
     * @param window_size The dimensions of the window or viewport.
     * @param alignment The alignment of the texture when drawn.
     */
    void OpenGLRenderer::DrawSimpleTexture2D(
      const Asset::Texture& texture,
      const Vector2& position,
      const Vector2& scale,
      const Vector2& window_size,
      Renderer2DProps::Alignment alignment
    )
    {
        // unit square vertices (position and texcoords)
        static const float vertices[] = {
            // Position           // TexCoords
            -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, // Bottom-left
             0.5f, -0.5f, 0.0f,   1.0f, 1.0f, // Bottom-right
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, // Top-right
             0.5f,  0.5f, 0.0f,   1.0f, 0.0f, // Top-right
            -0.5f,  0.5f, 0.0f,   0.0f, 0.0f, // Top-left
            -0.5f, -0.5f, 0.0f,   0.0f, 1.0f  // Bottom-left
        };

        static GLuint vao = 0, vbo = 0;

        if (vao == 0)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

            glBindVertexArray(0);

            FreeQueue::Push(
              [=]()
            {
                glDeleteBuffers(1, &vbo);
                glDeleteVertexArrays(1, &vao);
            }
            );
        }

        if (vao == 0 || scale == Vector2::Zero) return;

        glBindVertexArray(vao);

        // One-time load the texture shader
        static Asset::Shader texture_shader;
        static bool texture_shader_loaded = false;
        if (!texture_shader_loaded)
        {
            texture_shader.Load(Path::current("assets/shaders/texture.flxshader"));
            texture_shader_loaded = true;
        }
        texture_shader.Use();

        texture_shader.SetUniform_bool("u_use_texture", true);
        texture.Bind(texture_shader, "u_texture", 0);

        // Set default uniform values
        texture_shader.SetUniform_vec3("u_color", Vector3(1.0f, 1.0f, 1.0f));
        texture_shader.SetUniform_vec3("u_color_to_add", Vector3(0.0f, 0.0f, 0.0f));
        texture_shader.SetUniform_vec3("u_color_to_multiply", Vector3(1.0f, 1.0f, 1.0f));

        // Adjust position based on alignment
        Vector2 m_position = Vector2(position.x, position.y);
        switch (alignment)
        {
        case Renderer2DProps::Alignment_TopLeft:
            m_position += scale * 0.5f;
            break;
        case Renderer2DProps::Alignment_Center:
        default:
            break;
        }

        // Setup model matrix for translation and scaling
        Matrix4x4 model = Matrix4x4::Identity;
        texture_shader.SetUniform_mat4("u_model", model.Translate(Vector3(m_position.x, m_position.y, 0.0f))
                                                  .Scale(Vector3(scale.x, scale.y, 1.0f)));

        // Setup orthographic projection matrix
        Matrix4x4 proj_view = Matrix4x4::Orthographic(0.0f, window_size.x, 0.0f, window_size.y, -1.0f, 1.0f);
        texture_shader.SetUniform_mat4("u_projection_view", proj_view);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        m_draw_calls++;

        glBindVertexArray(0);
    }
}
