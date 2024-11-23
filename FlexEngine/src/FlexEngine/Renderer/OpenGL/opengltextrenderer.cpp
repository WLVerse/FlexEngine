/*!************************************************************************
* WLVERSE [https://wlverse.web.app]
* opengltextrenderer.cpp
*
* This file implements the OpenGLTextRenderer class, responsible for rendering
* 2D text in the game engine. It manages text rendering functionalities, including
* text alignment, word wrapping, and handling OpenGL resources for text display.
*
* Key functionalities include:
* - Configuring VAO/VBO for text rendering.
* - Managing text wrapping and alignment to ensure consistent display.
* - Updating vertex data dynamically based on text content and alignment settings.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*   - Developed core text rendering functionalities and OpenGL resource management.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "opengltextrenderer.h"
#include "FlexEngine/AssetManager/assetmanager.h" // FLX_ASSET_GET
#include "FlexEngine/DataStructures/freequeue.h"

namespace FlexEngine
{
    // Static member initialization
    GLuint OpenGLTextRenderer::m_VAO = 0;
    GLuint OpenGLTextRenderer::m_VBO = 0;
    float OpenGLTextRenderer::m_linespacing = 2.0f;
    float OpenGLTextRenderer::m_letterspacing = 5.0f;

    /*!***************************************************************************
    * \brief 
    * Initializes OpenGL resources for text rendering.
    *
    * This function sets up the VAO and VBO needed to render text quads, binding
    * and configuring vertex attributes.
    *****************************************************************************/
    void OpenGLTextRenderer::Init()
    {
        InitRenderData();

    }


    /*!***************************************************************************
    * \brief 
    * Creates VAO and VBO for dynamic text rendering and configures vertex attributes.
    *
    * This function generates the necessary buffers and attributes to handle text
    * quads in OpenGL. A lambda is used to automatically release resources when no longer needed.
    *****************************************************************************/
    void OpenGLTextRenderer::InitRenderData()
    {
        // Configure VAO/VBO for text quads
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); // 6 vertices per quad

        // Configure vertex attributes
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        FreeQueue::Push(
          [=]()
        {
            glDeleteVertexArrays(1, &m_VAO);
            glDeleteBuffers(1, &m_VBO);
        }
        );
    }

    /*!***************************************************************************
    * \brief  
    * Renders 2D text with specified alignment, color, and transformations.
    *
    * This function performs text rendering by positioning each character based on
    * alignment settings, handling line wrapping, and updating the OpenGL buffer
    * with vertex data for each glyph.
    *
    * \param text  
    * A Renderer2DText object containing properties for the text,
    * including words to render, color, shader, and alignment settings.
    *****************************************************************************/
    void OpenGLTextRenderer::DrawText2D(const Renderer2DText& text, bool followcam)
    {
        auto& asset_shader = FLX_ASSET_GET(Asset::Shader, text.m_shader);
        asset_shader.Use();

        if (text.m_fonttype.empty())
            Log::Fatal("Text Renderer: Unknown font type! Please check what you wrote!");
        if (text.m_words.empty())
            return;
        if (followcam && CameraManager::GetMainCamera() <= -1)
            followcam = false;

        asset_shader.SetUniform_vec3("u_color", text.m_color);
        asset_shader.SetUniform_mat4("u_model", text.m_transform);
        static const Matrix4x4 view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
        Matrix4x4 projection_view = Matrix4x4::Orthographic(0.0f, text.m_window_size.x, text.m_window_size.y, 0.0f, -2.0f, 2.0f) * view_matrix;
        asset_shader.SetUniform_mat4("projection", followcam ? CameraManager::GetCameraData(CameraManager::GetMainCamera())->proj_viewMatrix : projection_view);

        glActiveTexture(GL_TEXTURE0);
        glBindVertexArray(m_VAO);
        auto& asset_font = FLX_ASSET_GET(Asset::Font, text.m_fonttype);

        // Determine initial X position based on horizontal alignment
        Vector3 pen_pos = Vector3::Zero;
        float lineWidth = 0.0f;
        float maxLineHeight = 0.0f;

        switch (static_cast<Renderer2DText::AlignmentX>(text.m_alignment.first))
        {
        case Renderer2DText::Alignment_Left:
            pen_pos.x = 0.0f;
            break;
        case Renderer2DText::Alignment_Center:
            pen_pos.x = (float)FindLineWidth(asset_font, text.m_words) / 2.0f;
            break;
        case Renderer2DText::Alignment_Right:
            pen_pos.x = (float)FindLineWidth(asset_font, text.m_words);
            break;
        }

        #if 0 //textbox not yet done -> TODO setWidth text component
         Determine initial Y position based on vertical alignment (not working)
        switch (static_cast<Renderer2DText::AlignmentY>(text.m_alignment.second))
        {
        case Renderer2DText::Alignment_Top:
            pen_pos.y = 0.0f;
            break;
        case Renderer2DText::Alignment_Middle:
            pen_pos.y = maxHeight / 2.0f;
            break;
        case Renderer2DText::Alignment_Bottom:
            pen_pos.y = maxHeight;
            break;
        }
        #endif
        std::string t_subwords = text.m_words; //Remaining string to check length
        int t_pen_iter = 0; //Contains which character of words is being drawn
        // Iterate through all characters in the string
        for (char c : text.m_words)
        {
            // Get the glyph for the character
            const Asset::Glyph& glyph = asset_font.GetGlyph(c);
            maxLineHeight = std::max(maxLineHeight, glyph.size.y);

            // Check if adding this glyph exceeds max width, wrap to new line if necessary
            if (lineWidth + glyph.advance > DRAW_TEXT_MAX_STRLEN && c != ' ')
            {
                // Move to a new line
                pen_pos.y += maxLineHeight + m_linespacing; // Move down by glyph height and line spacing
                lineWidth = 0.0f;          // Reset line width for the new line
                // Update `t_subwords` to store only the remaining characters for the next line
                t_subwords = t_subwords.substr(t_pen_iter); // Keep unrendered characters
                t_pen_iter = 0; // Reset the iterator for the new line

                // Adjust starting X position for new line based on alignment
                switch (static_cast<Renderer2DText::AlignmentX>(text.m_alignment.first))
                {
                case Renderer2DText::Alignment_Left:   pen_pos.x = 0.0f; break;
                case Renderer2DText::Alignment_Center: pen_pos.x = (float)FindLineWidth(asset_font, t_subwords) / 2.0f; break;
                case Renderer2DText::Alignment_Right:  pen_pos.x = (float)FindLineWidth(asset_font, t_subwords); break;
                }
            }

            // Render the glyph at the current position
            RenderGlyph(glyph, pen_pos, text.m_color);

            // Advance position for the next character
            pen_pos.x -= glyph.advance + m_letterspacing;  // Add spacing between characters
            lineWidth += glyph.advance + m_letterspacing;  // Track width for the current line
            t_pen_iter++; // Move to the next character in the substring
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    /*!***************************************************************************
    * \brief 
    * Calculates the width of a given text line in pixels.
    *
    * This function iterates through each character in the text, accumulating
    * the advance distance and letter spacing to compute the line width.
    *
    * \param font The font asset containing glyph information for each character.
    * \param text The string of text to measure for width.
    * \return The width of the text line in pixels, capped at DRAW_TEXT_MAX_STRLEN.
    *****************************************************************************/
    int OpenGLTextRenderer::FindLineWidth(const Asset::Font& font, const std::string& text)
    {
        int t_totallength = 0;
        for (char c : text)
        {
            // Get the glyph for the character
            const Asset::Glyph& glyph = font.GetGlyph(c);
            t_totallength += glyph.advance + (int)m_letterspacing; // Add advance and spacing
            if (t_totallength > DRAW_TEXT_MAX_STRLEN)break;
        }
        return t_totallength > DRAW_TEXT_MAX_STRLEN ? DRAW_TEXT_MAX_STRLEN : t_totallength;
    }

    /*!***************************************************************************
    * \brief 
    * Renders a single glyph as a textured quad at the specified position.
    *
    * This function binds the glyph texture, calculates its position and size,
    * and renders a quad for the glyph. Vertex data is dynamically updated in the VBO.
    *
    * \param glyph The glyph to render, containing the texture ID, size, and bearing.
    * \param pen_pos The 2D position on the screen where the glyph will be rendered.
    * \param color The color to apply to the glyph when rendering.
    *****************************************************************************/
    void OpenGLTextRenderer::RenderGlyph(const Asset::Glyph& glyph, const Vector2& pen_pos, const Vector3& color)
    {
        UNREFERENCED_PARAMETER(color);
        // Bind the texture for the glyph
        glBindTexture(GL_TEXTURE_2D, glyph.textureID);

        // Calculate position and size of the glyph
        float xpos = pen_pos.x - glyph.bearing.x;
        float ypos = pen_pos.y - (glyph.bearing.y - glyph.size.y); // Align glyph to baseline
        float w = -glyph.size.x;
        float h = -glyph.size.y;

        // Define the vertices for a quad (two triangles) for the glyph
        float vertices[6][4] = {
            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos,     ypos,     0.0f, 1.0f },
            { xpos + w, ypos,     1.0f, 1.0f },

            { xpos,     ypos + h, 0.0f, 0.0f },
            { xpos + w, ypos,     1.0f, 1.0f },
            { xpos + w, ypos + h, 1.0f, 0.0f }
        };

        // Update the VBO with the new vertices
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Render the quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
}