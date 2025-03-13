/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// openglrenderer.h / .cpp
//
// This header declares the structures and functions used for 2D rendering
// with OpenGL. It defines properties for texture and text rendering, sprite
// batching, and includes the OpenGLRenderer class, which provides a suite of
// static functions for drawing 2D elements (textures, text, and batches) using
// an internal unit square mesh.
//
// Structures defined:
// - Renderer2DProps: Contains transformation, shader, asset, and color data
//   for rendering 2D textures.
// - Renderer2DText: Encapsulates text rendering parameters including shader,
//   font type, alignment, and transformation settings.
// - Renderer2DSpriteBatch: Holds batch data for rendering multiple sprite
//   instances with transformations and per-instance opacity.
//
// The OpenGLRenderer class provides static helper functions for common
// 2D rendering tasks and configuration options for depth testing and blending.
//
// AUTHORS
// [40%] Wen Loong (wenloong.l\@digipen.edu)
// [30%] Yew Chong (yewchong.k\@digipen.edu)
// [30%] Soh Wei Jie (weijie.soh\@digipen.edu)
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "flx_api.h"

#include "FlexMath/vector4.h"
#include "opengltexture.h"

#include <glad/glad.h>

#include "Renderer/Camera/CameraManager.h"
#include "FlexEngine/Renderer/Camera/camera.h"

namespace FlexEngine
{
    /**
     * @brief Properties for 2D texture rendering.
     *
     * Contains transformation data, shader and asset paths, color values,
     * and alignment settings for rendering a 2D texture. (Rename this struct
     * as needed.)
     */
    struct __FLX_API Renderer2DProps
    {
        enum __FLX_API Alignment
        {
            Alignment_Center = 0,
            Alignment_TopLeft = 1,
        };

        Matrix4x4 world_transform = Matrix4x4::Identity;  ///< Transformation applied to the texture.
        std::string shader = R"(/shaders/texture.flxshader)";  ///< Path to the shader used for texture rendering.
        std::string asset = R"(/images/flexengine/flexengine-256.png)"; ///< Asset path (texture or spritesheet).
        int texture_index = -1; ///< For spritesheets, -1 means not a spritesheet.
        Vector3 color = Vector3(1.0f, 0.0f, 1.0f); ///< Base color tint.
        Vector3 color_to_add = Vector3(0.0f, 0.0f, 0.0f); ///< Color additive value.
        Vector3 color_to_multiply = Vector3(1.0f, 1.0f, 1.0f); ///< Color multiplicative value.
        Vector2 window_size = Vector2(800.0f, 600.0f); ///< Current window dimensions.
        float alpha = 1.0f; ///< Opacity level.
        Alignment alignment = Alignment_Center; ///< Texture alignment setting.
    };

    /**
     * @brief Configuration and properties for 2D text rendering.
     *
     * The Renderer2DText struct encapsulates the necessary parameters for rendering
     * 2D text, including shader paths, font types, color settings, transformation
     * matrices, and alignment options for both X and Y axes.
     *
     * (Rename this struct as needed.)
     */
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
            Alignment_Middle = 0,      /*!< Middle (center) alignment */
            Alignment_Top = 1,         /*!< Top alignment */
            Alignment_Bottom = 2,      /*!< Bottom alignment */
        };

        std::string m_shader = R"(/shaders/freetypetext_GPU.flxshader)";  ///< Shader path for text rendering.
        std::string m_fonttype = R"()";  ///< Font type for rendering (empty indicates an error).
        std::string m_words;             ///< The text content to render.
        Vector3 m_color = Vector3::Zero; ///< Text color.
        Vector2 m_window_size = Vector2(800.0f, 600.0f); ///< Window dimensions.
        Matrix4x4 m_transform = Matrix4x4::Identity; ///< Transformation matrix for positioning.
        std::pair<AlignmentX, AlignmentY> m_alignment; ///< X and Y alignment settings.
        float m_linespacing = 2.0f;      ///< Spacing between lines.
        float m_letterspacing = 2.0f;    ///< Spacing between letters.
        Vector2 m_textboxDimensions = { 500.0f ,500.0f };  ///< Dimensions of the text box.
    };

    /**
     * @brief Holds data for batching multiple sprite instances.
     *
     * Contains shader information and per-instance data for transformation,
     * z-index, UV mapping (for animation), and opacity.
     */
    struct __FLX_API Renderer2DSpriteBatch
    {
        std::string m_shader = R"(/shaders/batchtexture.flxshader)";  ///< Shader path for sprite batch rendering.

        std::vector<int> m_zindex; ///< Z-index values for layering sprites.
        std::vector<Matrix4x4> m_transformationData; ///< Transformation matrices for each sprite.
        // For animation:
        std::vector<Vector4> m_UVmap; ///< UV mapping coordinates.
        // For opacity control:
        std::vector<float> m_opacity; ///< Opacity values for each sprite instance.
    };

    /**
     * @brief Provides a suite of static methods for 2D rendering using OpenGL.
     *
     * The OpenGLRenderer class offers functionality for drawing 2D textures, text,
     * and sprite batches. It also provides methods to query and control rendering
     * states such as depth testing and blending.
     */
    class __FLX_API OpenGLRenderer
    {
        static uint32_t m_draw_calls;               ///< Total draw calls made.
        static uint32_t m_draw_calls_last_frame;      ///< Draw calls made in the previous frame.
        static uint32_t m_maxInstances;               ///< Maximum allowed instances for batching.
        static bool m_depth_test;                     ///< Flag indicating if depth testing is enabled.
        static bool m_blending;                       ///< Flag indicating if blending is enabled.
    public:

        /// @brief Retrieves the total number of draw calls.
        static uint32_t GetDrawCalls();

        /// @brief Retrieves the number of draw calls from the last frame.
        static uint32_t GetDrawCallsLastFrame();

        /// @brief Checks if depth testing is currently enabled.
        static bool IsDepthTestEnabled();

        /// @brief Enables depth testing.
        static void EnableDepthTest();

        /// @brief Disables depth testing.
        static void DisableDepthTest();

        /// @brief Checks if blending is currently enabled.
        static bool IsBlendingEnabled();

        /// @brief Enables blending.
        static void EnableBlending();

        /// @brief Disables blending.
        static void DisableBlending();

        /// @brief Clears the frame buffer.
        static void ClearFrameBuffer();

        /// @brief Clears the screen with the specified color.
        /// @param color The clear color as a Vector4.
        static void ClearColor(const Vector4& color);

        /// @brief Executes a draw call with the specified instance size.
        /// @param size Number of instances to draw.
        static void Draw(GLsizei size);

        /// @brief Draws a 2D texture using the given camera and rendering properties.
        /// @param cam The camera through which the texture is rendered.
        /// @param props Rendering properties for the texture.
        static void DrawTexture2D(Camera const& cam, const Renderer2DProps& props = {});

        /// @brief Draws 2D text as a texture using the given camera and text settings.
        /// @param cam The camera through which the text is rendered.
        /// @param text Text rendering properties.
        static void DrawTexture2D(Camera const& cam, const Renderer2DText& text = {});

        /// @brief Draws a 2D texture using the provided rendering properties and camera data.
        /// @param props Rendering properties for the texture.
        /// @param cameraData Camera data to determine the view.
        static void DrawTexture2D(const Renderer2DProps& props, const Camera& cameraData);

        /// @brief Draws 2D text using the provided text properties and camera data.
        /// @param text Text rendering properties.
        /// @param cameraData Camera data to determine the view.
        static void DrawTexture2D(const Renderer2DText& text, const Camera& cameraData);

        /// @brief Draws batched sprite instances using the provided properties and batch data.
        /// @param props Rendering properties for the texture.
        /// @param data Batch data containing transformations and opacity values.
        /// @param cameraData Camera data to determine the view.
        static void DrawBatchTexture2D(const Renderer2DProps& props, const Renderer2DSpriteBatch& data, const Camera& cameraData);

        /// @brief Draws a simple texture without needing camera or asset manager data.
        ///
        /// This lightweight function uses the texture shader and an internal unit square mesh.
        /// @param texture The texture asset to draw.
        /// @param position The position at which to draw the texture.
        /// @param scale The scale to apply to the texture.
        /// @param window_size The dimensions of the window.
        /// @param alignment The alignment option for positioning the texture.
        static void DrawSimpleTexture2D(
          const Asset::Texture& texture = Asset::Texture::None,
          const Vector2& position = Vector2(0.0f, 0.0f),
          const Vector2& scale = Vector2(1600.0f, 900.0f),
          const Vector2& window_size = Vector2(1600.0f, 900.0f),
          Renderer2DProps::Alignment alignment = Renderer2DProps::Alignment_TopLeft
        );
    };

}
