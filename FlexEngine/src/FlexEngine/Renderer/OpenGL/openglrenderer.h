/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * openglrenderer.h
 *
 * This file implements the OpenGLRenderer class and related structures used for
 * rendering 2D textures and text in the engine. The OpenGLRenderer provides
 * various static methods for managing OpenGL state such as depth testing and
 * blending, clearing frame buffers, and drawing textures and text using
 * predefined properties.
 *
 * Key functionalities include:
 * - Configuring and retrieving draw call statistics.
 * - Enabling/disabling depth testing and blending.
 * - Clearing frame buffers with specified colors.
 * - Drawing 2D textures with customizable properties.
 * - Drawing 2D text using custom text rendering settings.
 * - A lightweight drawing function for simple textures.
 *
 * AUTHORS
 * [70%] Chan Wen Loong (wenloong.c\@digipen.edu)
 *   - Main Author
 * [30%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Sub Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/

#pragma once

#include "flx_api.h"

#include "FlexMath/vector4.h"
#include "opengltexture.h"

#include <glad/glad.h>

#include "Renderer/Camera/CameraManager.h"
#include "FlexEngine/Renderer/Camera/camera.h"

namespace FlexEngine
{
    /*!************************************************************************
     * \brief Struct for 2D texture rendering properties.
     *
     * The Renderer2DProps structure encapsulates properties used for rendering
     * 2D textures and sprites, including shader paths, texture assets, transformation
     * parameters, color modifications, and alignment options.
     *
     * Key properties include:
     * - shader: Path to the shader used for rendering.
     * - asset: Path to the texture or spritesheet.
     * - texture_index: Index for selecting a texture from a spritesheet (-1 indicates not a spritesheet).
     * - color: Base color applied to the texture.
     * - color_to_add: Additional color to add to the texture.
     * - color_to_multiply: Color used to modulate the texture.
     * - position, scale, rotation: Transformation properties.
     * - window_size: Dimensions of the rendering window.
     * - alpha: Transparency value.
     * - alignment: Alignment mode for rendering.
     *************************************************************************/
    struct __FLX_API Renderer2DProps
    {
        enum __FLX_API Alignment
        {
            Alignment_Center = 0,
            Alignment_TopLeft = 1,
        };

        std::string shader = R"(/shaders/texture.flxshader)";
        std::string asset = R"(/images/flexengine/flexengine-256.png)"; // Texture or spritesheet
        int texture_index = -1; // For spritesheets, -1 means not a spritesheet
        Vector3 color = Vector3(1.0f, 0.0f, 1.0f);
        Vector3 color_to_add = Vector3(0.0f, 0.0f, 0.0f);
        Vector3 color_to_multiply = Vector3(1.0f, 1.0f, 1.0f);
        Vector2 position = Vector2(0.0f, 0.0f);
        Vector2 scale = Vector2(1.0f, 1.0f);
        Vector3 rotation = Vector3(0.f, 0.f, 0.f);
        Vector2 window_size = Vector2(800.0f, 600.0f);
        float alpha = 1.0f;
        Alignment alignment = Alignment_Center;
    };

    /*!************************************************************************
     * \brief Struct for 2D text rendering configuration and properties.
     *
     * The Renderer2DText structure encapsulates the necessary parameters for rendering
     * 2D text, including shader paths, font types, color settings, transformation
     * matrices, and alignment options for both X and Y axes.
     *
     * Key properties include:
     * - m_shader: Path to the shader used for text rendering.
     * - m_fonttype: The font type to use for rendering text.
     * - m_words: The actual text to render.
     * - m_color: The color of the text.
     * - m_window_size: Dimensions of the rendering window.
     * - m_transform: Transformation matrix for positioning and scaling the text.
     * - m_alignment: Pair indicating horizontal (X) and vertical (Y) alignment.
     * - m_linespacing: Spacing between lines of text.
     * - m_letterspacing: Spacing between individual letters.
     * - m_textboxDimensions: Dimensions of the text box.
     *************************************************************************/
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

        std::string m_shader = R"(/shaders/freetypetext.flxshader)";  /*!< Path to the shader for text rendering */
        std::string m_fonttype = R"()";                     /*!< Font type to use for rendering; empty means an error will occur */
        std::string m_words;                                /*!< The actual text to render */
        Vector3 m_color = Vector3::Zero;                    /*!< Color of the text */
        Vector2 m_window_size = Vector2(800.0f, 600.0f);     /*!< Dimensions of the rendering window */
        Matrix4x4 m_transform = Matrix4x4::Identity;        /*!< Transformation matrix for positioning the text */
        std::pair<AlignmentX, AlignmentY> m_alignment;      /*!< Pair indicating X and Y alignment settings */
        float m_linespacing = 2.0f;                         /*!< Spacing between lines of text */
        float m_letterspacing = 2.0f;                       /*!< Spacing between individual letters */
        Vector2 m_textboxDimensions = { 500.0f, 500.0f };     /*!< Dimensions of the text box */
    };

    /*!************************************************************************
     * \brief The OpenGLRenderer class.
     *
     * The OpenGLRenderer class provides static functions to facilitate rendering of
     * 2D textures and text using OpenGL. It manages draw call statistics, OpenGL
     * state (such as depth testing and blending), and provides helper functions to
     * draw textures and text with customizable properties.
     *************************************************************************/
    class __FLX_API OpenGLRenderer
    {
        static uint32_t m_draw_calls;                /*!< Number of draw calls in the current frame */
        static uint32_t m_draw_calls_last_frame;       /*!< Number of draw calls in the last frame */
        static bool m_depth_test;                      /*!< Flag indicating if depth testing is enabled */
        static bool m_blending;                        /*!< Flag indicating if blending is enabled */
    public:
        /*!
         * \brief Retrieves the number of draw calls executed in the current frame.
         *
         * \return The number of draw calls executed.
         */
        static uint32_t GetDrawCalls();

        /*!
         * \brief Retrieves the number of draw calls executed in the last frame.
         *
         * \return The number of draw calls executed in the previous frame.
         */
        static uint32_t GetDrawCallsLastFrame();

        /*!
         * \brief Checks if depth testing is currently enabled.
         *
         * \return \c true if depth testing is enabled, otherwise \c false.
         */
        static bool IsDepthTestEnabled();

        /*!
         * \brief Enables OpenGL depth testing.
         *
         * Configures OpenGL to perform depth comparisons and updates the depth buffer.
         */
        static void EnableDepthTest();

        /*!
         * \brief Disables OpenGL depth testing.
         *
         * Turns off depth comparisons, which may improve performance in certain cases.
         */
        static void DisableDepthTest();

        /*!
         * \brief Checks if blending is currently enabled.
         *
         * \return \c true if blending is enabled, otherwise \c false.
         */
        static bool IsBlendingEnabled();

        /*!
         * \brief Enables OpenGL blending.
         *
         * Activates blending to allow for transparency and smooth transitions between
         * overlapping textures.
         */
        static void EnableBlending();

        /*!
         * \brief Disables OpenGL blending.
         *
         * Deactivates blending, which may be desirable when rendering opaque objects.
         */
        static void DisableBlending();

        /*!
         * \brief Clears the currently active framebuffer.
         *
         * This function clears the color and depth buffers of the active framebuffer.
         */
        static void ClearFrameBuffer();

        /*!
         * \brief Clears the framebuffer with a specific color.
         *
         * Sets the clear color to the specified value and clears the framebuffer.
         *
         * \param color The color to clear the framebuffer with.
         */
        static void ClearColor(const Vector4& color);

        /*!
         * \brief Issues a draw call.
         *
         * Draws primitives using the currently bound vertex data.
         *
         * \param size The number of vertices to draw.
         */
        static void Draw(GLsizei size);

        /*!
         * \brief Draws a 2D texture using the provided rendering properties.
         *
         * Uses an internal unit square mesh and the specified shader to render a texture.
         * The camera is identified by an entity ID, defaulting to 0 if not specified.
         *
         * \param props The rendering properties for the texture.
         * \param camID The entity ID of the camera to use for rendering (default is 0).
         */
        static void DrawTexture2D(const Renderer2DProps& props = {}, const FlexECS::EntityID camID = 0);

        /*!
         * \brief Draws 2D text using the provided text rendering properties.
         *
         * Renders text as a texture using an internal unit square mesh and the specified
         * shader. The camera is identified by an entity ID, defaulting to 0 if not specified.
         *
         * \param text The text rendering properties.
         * \param camID The entity ID of the camera to use for rendering (default is 0).
         */
        static void DrawTexture2D(const Renderer2DText& text = {}, const FlexECS::EntityID camID = 0);

        /*!
         * \brief Draws a 2D texture using a provided Camera object.
         *
         * Renders a texture with the specified properties and uses the provided Camera
         * object's data for view and projection transformations.
         *
         * \param props The rendering properties for the texture.
         * \param cameraData The Camera object containing view/projection data.
         */
        static void DrawTexture2D(const Renderer2DProps& props, const Camera& cameraData);

        /*!
         * \brief Draws 2D text using a provided Camera object.
         *
         * Renders text with the specified properties and uses the provided Camera object's
         * data for view and projection transformations.
         *
         * \param text The text rendering properties.
         * \param cameraData The Camera object containing view/projection data.
         */
        static void DrawTexture2D(const Renderer2DText& text, const Camera& cameraData);

        /*!
         * \brief Draws a simple 2D texture without using a camera entity.
         *
         * This lightweight function draws a texture using the default texture shader
         * and an internal unit square mesh. It does not require camera data, rendering
         * properties, or an asset manager.
         *
         * \param texture The texture asset to draw (default is Asset::Texture::None).
         * \param position The position at which to render the texture.
         * \param scale The scale to apply to the texture.
         * \param window_size The dimensions of the rendering window.
         * \param alignment The alignment mode to use for rendering.
         */
        static void DrawSimpleTexture2D(
          const Asset::Texture& texture = Asset::Texture::None,
          const Vector2& position = Vector2(0.0f, 0.0f),
          const Vector2& scale = Vector2(1600.0f, 900.0f),
          const Vector2& window_size = Vector2(1600.0f, 900.0f),
          Renderer2DProps::Alignment alignment = Renderer2DProps::Alignment_TopLeft
        );
    };

} // namespace FlexEngine
