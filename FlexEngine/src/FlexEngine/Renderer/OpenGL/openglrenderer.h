/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * openglrenderer.h
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

#pragma once

#include "flx_api.h"

#include "FlexMath/vector4.h"
#include "opengltexture.h"

#include <glad/glad.h>

#include "Renderer/Camera/CameraManager.h"
#include "FlexEngine/Renderer/Camera/camera.h"

namespace FlexEngine
{
    /*!
     * @brief Structure encapsulating properties for 2D texture rendering.
     *
     * This structure holds the configurable properties used when rendering a 2D texture,
     * including shader paths, asset paths, transformation parameters, and color adjustments.
     */
    struct __FLX_API Renderer2DProps
    {
        /*!
         * @brief Enumeration for alignment options.
         */
        enum __FLX_API Alignment
        {
            Alignment_Center = 0,  /*!< Center alignment */
            Alignment_TopLeft = 1, /*!< Top left alignment */
        };

        std::string shader = R"(/shaders/texture.flxshader)";
        std::string asset = R"(/images/flexengine/flexengine-256.png)"; /*!< Texture or spritesheet asset path */
        int texture_index = -1; /*!< For spritesheets, -1 means not a spritesheet */
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

    /*!
     * @brief Structure for 2D text rendering configuration and properties.
     *
     * The Renderer2DText struct encapsulates the necessary parameters for rendering
     * 2D text, including shader paths, font types, color settings, transformation
     * matrices, and alignment options for both X and Y axes.
     */
     // Can remind me what to rename this to again
    struct __FLX_API Renderer2DText
    {
        /*!
         * @brief Enumeration for horizontal text alignment.
         */
        enum __FLX_API AlignmentX
        {
            Alignment_Center = 0, /*!< Center alignment */
            Alignment_Left = 1,   /*!< Left alignment */
            Alignment_Right = 2,  /*!< Right alignment */
        };

        /*!
         * @brief Enumeration for vertical text alignment.
         */
        enum __FLX_API AlignmentY
        {
            Alignment_Middle = 0, /*!< Middle alignment */
            Alignment_Top = 1,    /*!< Top alignment */
            Alignment_Bottom = 2, /*!< Bottom alignment */
        };

        std::string m_shader = R"(/shaders/freetypetext.flxshader)"; /*!< Path to the shader for text rendering */
        std::string m_fonttype = R"()";                               /*!< Font type to use for rendering; empty means an error will occur */
        std::string m_words;                                          /*!< The actual text to render */
        Vector3 m_color = Vector3::Zero;                              /*!< Color of the text */
        Vector2 m_window_size = Vector2(800.0f, 600.0f);
        Matrix4x4 m_transform = Matrix4x4::Identity;                  /*!< Transformation matrix for positioning the text */
        std::pair<AlignmentX, AlignmentY> m_alignment;                /*!< Pair indicating X and Y alignment settings */
        float m_linespacing = 2.0f;
        float m_letterspacing = 2.0f;
        Vector2 m_textboxDimensions = { 500.0f ,500.0f };             /*!< Dimensions of the text box */
    };

    /*!
     * @brief Class for OpenGL 2D rendering operations.
     *
     * The OpenGLRenderer class provides static helper functions for managing OpenGL state,
     * clearing the framebuffer, and drawing textures and text using a unit square mesh.
     */
    class __FLX_API OpenGLRenderer
    {
        static uint32_t m_draw_calls;           /*!< Number of draw calls issued in the current frame */
        static uint32_t m_draw_calls_last_frame;  /*!< Number of draw calls issued in the last frame */
        static bool m_depth_test;               /*!< Flag indicating whether depth testing is enabled */
        static bool m_blending;                 /*!< Flag indicating whether blending is enabled */
    public:
        /*!
         * @brief Retrieves the number of draw calls issued in the current frame.
         *
         * @return The current frame's draw call count.
         */
        static uint32_t GetDrawCalls();

        /*!
         * @brief Retrieves the number of draw calls issued in the last frame.
         *
         * @return The last frame's draw call count.
         */
        static uint32_t GetDrawCallsLastFrame();

        /*!
         * @brief Checks if depth testing is enabled.
         *
         * @return True if depth testing is enabled, false otherwise.
         */
        static bool IsDepthTestEnabled();

        /*!
         * @brief Enables OpenGL depth testing.
         */
        static void EnableDepthTest();

        /*!
         * @brief Disables OpenGL depth testing.
         */
        static void DisableDepthTest();

        /*!
         * @brief Checks if blending is enabled.
         *
         * @return True if blending is enabled, false otherwise.
         */
        static bool IsBlendingEnabled();

        /*!
         * @brief Enables OpenGL blending.
         */
        static void EnableBlending();

        /*!
         * @brief Disables OpenGL blending.
         */
        static void DisableBlending();

        /*!
         * @brief Clears the current framebuffer.
         *
         * This function clears the color, depth, and stencil buffers of the current framebuffer.
         */
        static void ClearFrameBuffer();

        /*!
         * @brief Sets the clear color for the framebuffer.
         *
         * @param color The color to use when clearing the framebuffer.
         */
        static void ClearColor(const Vector4& color);

        /*!
         * @brief Issues a draw call.
         *
         * @param size The number of vertices to draw.
         */
        static void Draw(GLsizei size);

        /*!
         * @brief Draws a 2D texture using the specified rendering properties and camera.
         *
         * This helper function uses an internal unit square mesh to draw the texture.
         *
         * @param props Rendering properties including shader, asset, and transformation parameters.
         * @param camID The camera entity ID to use for rendering (default is 0).
         */
        static void DrawTexture2D(const Renderer2DProps& props = {}, const FlexECS::EntityID camID = 0);

        /*!
         * @brief Draws 2D text as a texture using the specified text rendering properties and camera.
         *
         * This overloaded function uses an internal unit square mesh to draw the text.
         *
         * @param text Text rendering properties including shader, font, and alignment settings.
         * @param camID The camera entity ID to use for rendering (default is 0).
         */
        static void DrawTexture2D(const Renderer2DText& text = {}, const FlexECS::EntityID camID = 0);

        /*!
         * @brief Draws a 2D texture using a provided camera configuration.
         *
         * This function renders the texture using the specified rendering properties and camera data.
         *
         * @param props Rendering properties for the texture.
         * @param cameraData Camera configuration data for rendering.
         */
        static void DrawTexture2D(const Renderer2DProps& props, const Camera& cameraData);

        /*!
         * @brief Draws a simple 2D texture with minimal configuration.
         *
         * This function is designed to be extremely lightweight and doesn't require a camera,
         * additional rendering properties, or an asset manager. It uses the default texture shader
         * and a unit square mesh.
         *
         * @param texture The texture asset to draw. Defaults to Asset::Texture::None.
         * @param position The position where the texture should be drawn.
         * @param scale The scale to apply to the drawn texture.
         * @param window_size The dimensions of the window or viewport.
         * @param alignment The alignment of the texture when drawn.
         */
        static void DrawSimpleTexture2D(
          const Asset::Texture& texture = Asset::Texture::None,
          const Vector2& position = Vector2(0.0f, 0.0f),
          const Vector2& scale = Vector2(1600.0f, 900.0f),
          const Vector2& window_size = Vector2(1600.0f, 900.0f),
          Renderer2DProps::Alignment alignment = Renderer2DProps::Alignment_TopLeft
        );
    };
}
