/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// openglspriterenderer.h
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
#pragma once

#include "flx_api.h"
#include "FlexMath/vector4.h"
#include "opengltexture.h"
#include <glad/glad.h>

namespace FlexEngine
{
    /*!***************************************************************************
    * \struct Renderer2DProps
    * \brief
    * Structure that holds properties for 2D rendering, including shader paths,
    * texture settings, color attributes, transformations, and alignment.
    *
    * This structure serves as a configuration for sprite rendering, allowing
    * customization of how 2D sprites are drawn to the screen.
    *****************************************************************************/
    struct __FLX_API Renderer2DProps
    {
        /*!***************************************************************************
        * \enum Alignment
        * \brief
        * Enumeration defining various alignment options for 2D sprites.
        *
        * This allows developers to specify how the sprite should be aligned
        * within its designated space on the screen.
        *****************************************************************************/
        enum __FLX_API Alignment
        {
            Alignment_Center = 0,      /*!< Center alignment */
            Alignment_TopLeft = 1,     /*!< Top-left alignment */
            Alignment_TopRight = 2,    /*!< Top-right alignment */
            Alignment_BottomLeft = 3,  /*!< Bottom-left alignment */
            Alignment_BottomRight = 4, /*!< Bottom-right alignment */
        };
        /*!***************************************************************************
        * \enum VBO_Type
        * \brief
        * Enumeration defining different types of Vertex Buffer Objects (VBOs)
        * used for rendering.
        *
        * This categorization helps in managing different rendering techniques
        * within the OpenGL context.
        *****************************************************************************/
        enum __FLX_API VBO_Type
        {
            VBO_Basic,                /*!< Basic VBO for standard rendering */
            VBO_Line,                 /*!< VBO for line rendering */
            VBO_PProcessing,          /*!< VBO for post-processing effects */
        };

        std::string shader = R"(/shaders/texture)";                       /*!< Path to the shader used for rendering textures */
        std::string texture = R"(/images/flexengine/flexengine-256.png)"; /*!< Default texture path */
        Vector3 color = Vector3(1.0f, 0.0f, 1.0f);                        /*!< Base color for the sprite */
        Vector3 color_to_add = Vector3(0.0f, 0.0f, 0.0f);                 /*!< Color to add during rendering */
        Vector3 color_to_multiply = Vector3(1.0f, 1.0f, 1.0f);            /*!< Color multiplier for sprite rendering */
        Matrix4x4 transform = {};                                         /*!< Transformation matrix for the sprite */
        Vector2 window_size = Vector2(800.0f, 600.0f);                    /*!< Size of the rendering window */
        Alignment alignment = Alignment_Center;                           /*!< Alignment option for the sprite */
        GLuint vbo_id = 0;                                                /*!< ID for the Vertex Buffer Object */
    };

    /*!***************************************************************************
    * \struct VertexBufferObject
    * \brief
    * Structure that represents a Vertex Buffer Object (VBO) and its associated
    * Vertex Array Object (VAO).
    *
    * This structure is used for managing OpenGL resources related to vertex
    * data, facilitating the rendering process.
    *****************************************************************************/
    struct __FLX_API VertexBufferObject
    {
        GLuint vao = 0;
        GLuint vbo = 0;
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*!***************************************************************************
    * \class OpenGLSpriteRenderer
    * \brief
    * Class responsible for rendering 2D sprites using OpenGL.
    *
    * This class provides methods for drawing textures, enabling/disabling
    * post-processing effects, and managing rendering configurations, such as
    * blending and depth testing. It is optimized for performance and ease
    * of integration into the game engine.
    *****************************************************************************/
    class __FLX_API OpenGLSpriteRenderer
    {
        static uint32_t m_draw_calls;                 /*!< Total number of draw calls made */
        static uint32_t m_draw_calls_last_frame;      /*!< Draw calls made in the last frame */
        static bool m_depth_test;                      /*!< Flag for enabling depth testing */
        static bool m_blending;                        /*!< Flag for enabling blending */

        static std::vector<VertexBufferObject> m_vbos; /*!< Vector of Vertex Buffer Objects */

        // Testing variables (subject to change)
        static GLuint samples;                         /*!< Number of samples per pixel for MSAA anti-aliasing */
        static float gamma;                            /*!< Controls the gamma function */
        static GLuint m_rectVAO;                      /*!< VAO for rendering rectangles */
        static GLuint m_rectVBO;                      /*!< VBO for rendering rectangles */
        static GLuint m_postProcessingFBO;            /*!< Framebuffer Object for post-processing */
        static GLuint m_colorBuffer;                   /*!< Color buffer for rendering */
        static GLuint m_brightBuffer;                  /*!< Brightness buffer for bloom effects */
        static GLuint bloomTexture;                    /*!< Texture used for bloom effects */
        static GLuint postProcessingTexture;           /*!< Final texture after post-processing */
        static GLuint m_pingpongFBO[2];               /*!< Ping-pong Framebuffer Objects */
        static GLuint m_pingpongBuffer[2];            /*!< Ping-pong buffers for intermediate processing */

    public:
        /*!***************************************************************************
        * \brief
        * Retrieves the total number of draw calls made.
        *
        * \return Total draw calls as a uint32_t.
        *****************************************************************************/
        static uint32_t GetDrawCalls();

        /*!***************************************************************************
        * \brief
        * Retrieves the number of draw calls made in the last frame.
        *
        * \return Total draw calls from the last frame as a uint32_t.
        *****************************************************************************/
        static uint32_t GetDrawCallsLastFrame();

        /*!***************************************************************************
        * \brief
        * Checks if depth testing is enabled.
        *
        * \return True if depth testing is enabled, otherwise false.
        *****************************************************************************/
        static bool IsDepthTestEnabled();

        /*!***************************************************************************
        * \brief
        * Enables depth testing for rendering.
        *****************************************************************************/
        static void EnableDepthTest();

        /*!***************************************************************************
        * \brief
        * Disables depth testing for rendering.
        *****************************************************************************/
        static void DisableDepthTest();

        /*!***************************************************************************
        * \brief
        * Checks if blending is enabled.
        *
        * \return True if blending is enabled, otherwise false.
        *****************************************************************************/
        static bool IsBlendingEnabled();

        /*!***************************************************************************
        * \brief
        * Enables blending for rendering.
        *****************************************************************************/
        static void EnableBlending();

        /*!***************************************************************************
        * \brief
        * Disables blending for rendering.
        *****************************************************************************/
        static void DisableBlending();

        /*!***************************************************************************
        * \brief
        * Enables post-processing effects for rendering.
        *****************************************************************************/
        static void EnablePostProcessing();

        /*!***************************************************************************
        * \brief
        * Disables post-processing effects for rendering.
        *****************************************************************************/
        static void DisablePostProcessing();

        /*!***************************************************************************
        * \brief
        * Clears the current framebuffer.
        *****************************************************************************/
        static void ClearFrameBuffer();

        /*!***************************************************************************
        * \brief
        * Clears the color buffer with a specified color.
        *
        * \param color The color to clear the buffer to, represented as a Vector4.
        *****************************************************************************/
        static void ClearColor(const Vector4& color);

        /*!***************************************************************************
        * \brief
        * Retrieves the VAO ID for a specified VBO type.
        *
        * \param type The type of VBO for which to retrieve the VAO ID.
        * \return The corresponding VAO ID as a GLuint.
        *****************************************************************************/
        static GLuint GetVAO_ID(Renderer2DProps::VBO_Type type);

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
        static void CreateVAOandVBO(GLuint& vao, GLuint& vbo, const float* vertices, int vertexCount);
        
        /*!***************************************************************************
        * \brief
        * Initializes the VBOs.
        *
        * \param windowSize The size of the rendering window as a Vector2.
        *****************************************************************************/
        static void Init(const Vector2& windowSize);

        /*!***************************************************************************
        * \brief
        * Draws a 2D texture using the specified properties.
        *
        * \param props The properties for rendering the texture, including shader,
        *              texture, color, and transformations.
        *****************************************************************************/
        static void DrawTexture2D(const Renderer2DProps& props = {});

        /*!***************************************************************************
        * \brief
        * Draws the post-processing layer after all other rendering operations.
        *****************************************************************************/
        static void DrawPostProcessingLayer();
    };
}