#pragma once
/*!************************************************************************
* WLVERSE [https://wlverse.web.app]
* opengltextrenderer.h
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
#include "flx_api.h"

#include "openglfont.h"
#include <glad/glad.h>

#include "../cameramanager.h"
#include <string>

#define DRAW_TEXT_MAX_STRLEN 2048

namespace FlexEngine 
{
    /*!***************************************************************************
    * \brief 
    * Struct for 2D text rendering configuration and properties.
    *
    * The Renderer2DText struct encapsulates the necessary parameters for rendering
    * 2D text, including shader paths, font types, color settings, transformation
    * matrices, and alignment options for both X and Y axes.
    ***************************************************************************/
    struct __FLX_API Renderer2DText
    {
        enum __FLX_API AlignmentX
        {
            Alignment_Center = 0,      /*!< Center alignment */
            Alignment_Left = 1,        /*!< Left alignment */
            Alignment_Right = 2,       /*!< Right alignment */
        };

        enum __FLX_API AlignmentY
        {
            Alignment_Middle = 0,      /*!< Center alignment */
            Alignment_Top = 1,         /*!< Top alignment */
            Alignment_Bottom = 2,      /*!< Bottom alignment */
        };

        std::string m_shader = R"(/shaders/freetypetext)";  /*!< Path to the shader for text rendering */
        std::string m_fonttype = R"()";                     /*!< Font type to use for rendering; empty means an error will occur */
        std::string m_words;                                /*!< The actual text to render */
        Vector3 m_color = Vector3::Zero;                    /*!< Color of the text */
        Vector2 m_window_size = Vector2(800.0f, 600.0f);
        Matrix4x4 m_transform = Matrix4x4::Identity;        /*!< Transformation matrix for positioning the text */
        std::pair<AlignmentX, AlignmentY> m_alignment;      /*!< Pair indicating X and Y alignment settings */
    };

    /*!***************************************************************************
    * \brief 
    * Class for OpenGL text rendering.
    *
    * The OpenGLTextRenderer class handles the rendering of 2D text using OpenGL.
    * It initializes necessary OpenGL resources, manages text alignment, word wrapping,
    * and handles dynamic updates for rendering text quads.
    ***************************************************************************/
    class __FLX_API OpenGLTextRenderer
    {
        static GLuint m_VAO, m_VBO; // Vertex Array Object and Vertex Buffer Object

        static float m_linespacing, m_letterspacing; // Line and letter spacing settings

        /*!***************************************************************************
        * \brief
        * Initializes OpenGL resources for text rendering.
        *
        * This function sets up the vertex array object (VAO) and vertex buffer object
        * (VBO) required for rendering text quads.
        ***************************************************************************/
        static void InitRenderData(); // Initializes VBO and VAO for rendering text

        /*!***************************************************************************
        * \brief
        * Renders a single glyph at the specified position with the given color.
        *
        * This function binds the glyph texture and calculates its position and size
        * to render a quad for the glyph, updating vertex data in the VBO.
        *
        * \param glyph The glyph to render, containing its texture ID, size, and bearing.
        * \param pen_pos The position on the screen where the glyph will be rendered.
        * \param color The color to apply to the glyph during rendering.
        ***************************************************************************/
        static void RenderGlyph(const Asset::Glyph& glyph, const Vector2& pen_pos, const Vector3& color);

        /*!***************************************************************************
        * \brief
        * Calculates the width of a given text line in pixels.
        *
        * This function iterates through the characters in the text, accumulating
        * their advance values and letter spacing to compute the total line width.
        *
        * \param font The font asset containing glyph information for each character.
        * \param text The string of text to measure for width.
        * \return The total width of the text line in pixels.
        ***************************************************************************/
        static int FindLineWidth(const Asset::Font& font, const std::string& text = "");

    public:
        /*!***************************************************************************
        * \brief
        * Initializes the OpenGL text renderer.
        *
        * This public function is called to initialize the text rendering resources.
        ***************************************************************************/
        static void Init();

        /*!***************************************************************************
        * \brief
        * Renders 2D text with specified properties.
        *
        * This function is responsible for rendering 2D text, handling text alignment,
        * word wrapping, and updating vertex data based on the provided text properties.
        *
        * \param text A Renderer2DText object containing text properties, including
        * words to render, shader, font type, color, and alignment settings.
        ***************************************************************************/
        static void DrawText2D(const Renderer2DText& text, bool followcam = false);

        #pragma region Getter functions

        /*!***************************************************************************
        * \brief Retrieves the current line spacing.
        *
        * This function returns the value of the line spacing, which is used to
        * determine the vertical distance between lines of text.
        *
        * \return The current line spacing as a float value.
        *****************************************************************************/
        const float GetLineSpacing() const { return m_linespacing; }

        /*!***************************************************************************
        * \brief Retrieves the current letter spacing.
        *
        * This function returns the value of the letter spacing, which is used to
        * determine the horizontal distance between individual characters in a text.
        *
        * \return The current letter spacing as a float value.
        *****************************************************************************/
        const float GetLetterSpacing() const { return m_letterspacing; }

        #pragma endregion

        #pragma region Setter functions

        /*!***************************************************************************
        * \brief Sets the line spacing to a new value.
        *
        * This function updates the line spacing used for rendering text, affecting
        * the vertical distance between lines. A larger value increases the space
        * between lines.
        *
        * \param temp The new line spacing value to set, as a float.
        *****************************************************************************/
        void SetLineSpacing(const float temp) { m_linespacing = temp; }

        /*!***************************************************************************
        * \brief Sets the letter spacing to a new value.
        *
        * This function updates the letter spacing used for rendering text, affecting
        * the horizontal distance between characters. A larger value increases the
        * space between characters.
        *
        * \param temp The new letter spacing value to set, as a float.
        *****************************************************************************/
        void SetLetterSpacing(const float temp) { m_letterspacing = temp; }

        #pragma endregion
    };
}