/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * openglspriterenderer.h
 *
 * This file implements the OpenGLSpriteRenderer class, responsible for
 * handling 2D sprite rendering within the game engine. The class provides
 * functions for rendering sprites and managing OpenGL resources such as
 * shaders and vertex buffers.
 *
 * Key functionalities include:
 * - Rendering 2D sprites with texture binding, transformations, and alignment.
 * - Supporting efficient batching for optimized performance.
 * - Managing rendering configurations like blending and depth testing.
 *
 * This renderer is designed with performance and flexibility in mind, enabling
 * easy extension and customization for rendering tasks in game engines.
 *
 * AUTHORS
 * [100%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Main Author
 *   - Developed core rendering functionalities and resource management
 *     within the FlexEngine architecture.
 *
 * Copyright (c) 2024 DigiPen, All rights reserved.
 **************************************************************************/
#pragma once

#include "flx_api.h"
#include "FlexMath/vector4.h"
#include "opengltexture.h"
#include "openglpostprocessing.h"
#include "../cameramanager.h"

namespace FlexEngine
{
    class CameraManager; // Forward declaration
    #pragma region Data-types
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
            VBO_BasicInverted,        /*!< Basic VBO for opengl texture rendering */
            VBO_Line,                 /*!< VBO for line rendering */
            VBO_PProcessing,          /*!< VBO for post-processing effects */
        };

        std::string shader = R"(/shaders/texture)";                       /*!< Path to the shader used for rendering textures */
        std::string texture = R"(/images/flexengine/flexengine-256.png)"; /*!< Default texture path */
        Matrix4x4 transform = Matrix4x4::Identity;                        // Transformation matrix per instance
        Vector3 color_to_add = Vector3(0.0f, 0.0f, 0.0f);                 // Color addition effect
        Vector3 color_to_multiply = Vector3(1.0f, 1.0f, 1.0f);            // Color multiplication effect
        Vector2 window_size = Vector2(800.0f, 600.0f);                    /*!< Size of the rendering window */
        Alignment alignment = Alignment_Center;                           /*!< Alignment option for the sprite */
        GLuint vbo_id = VBO_Basic;                                        /*!< ID for the Vertex Buffer Object */
    };

    /*!***************************************************************************
    * \struct MeshBuffer
    * \brief
    * Structure that represents a Vertex Buffer Object (VBO) and its associated
    * Vertex Array Object (VAO).
    *
    * This structure is used for managing OpenGL resources related to vertex
    * data, facilitating the rendering process.
    *****************************************************************************/
    struct __FLX_API MeshBuffer
    {
        GLuint vao = 0; // Vertex Array Object
        GLuint vbo = 0; // Vertex Buffer Object
    };

    /*!***************************************************************************
    * \struct Sprite_Batch_Inst
    * \brief
    * Holds data for batching multiple sprite instances, including transformation
    * and color data for each instance.
    *****************************************************************************/
    struct __FLX_API Sprite_Batch_Inst
    {
        GLuint m_vboid = 0;
        std::vector<int> m_zindex;
        std::vector<Matrix4x4> m_transformationData;
        std::vector<Vector3> m_colorAddData, m_colorMultiplyData;

        //For animation
        std::vector<Vector4> m_UVmap;
    };
    #pragma endregion

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*!***************************************************************************
    * \class OpenGLSpriteRenderer
    * \brief
    * Class responsible for rendering 2D sprites using OpenGL.
    *
    * This class provides methods for drawing textures, enabling efficient batching,
    * and managing OpenGL configurations like blending and depth testing.
    *****************************************************************************/
    class __FLX_API OpenGLSpriteRenderer
    {
        #pragma region Static Variables
        static uint32_t m_draw_calls;                 /*!< Total number of draw calls made */
        static uint32_t m_draw_calls_last_frame;      /*!< Draw calls made in the last frame */
        static uint32_t m_maxInstances; // Maximum number of instances allowed in one batch
        static bool m_depth_test;                      /*!< Flag for enabling depth testing */
        static bool m_blending;                        /*!< Flag for enabling blending */

        static std::vector<MeshBuffer> m_vbos; /*!< Vector of Vertex Buffer Objects */
        static std::vector<GLuint> m_batchSSBOs;
        
        static const CameraManager* m_CamM_Instance;
        #pragma endregion

    public:
        #pragma region Wrapper Func
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
        * Clears the color buffer with a specified color.
        *
        * \param color The color to clear the buffer to, represented as a Vector4.
        *****************************************************************************/
        static void ClearColor(const Vector4& color);

        #pragma endregion

        /*!***************************************************************************
        * \brief
        * Retrieves the VAO ID for a specified VBO type.
        *
        * \param type The type of VBO for which to retrieve the VAO ID.
        * \return The corresponding VAO ID as a GLuint.
        *****************************************************************************/
        static GLuint GetVAO_ID(Renderer2DProps::VBO_Type type);

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
        static void InitQuadVAO_VBO(GLuint& vao, GLuint& vbo, const float* vertices, int vertexCount);
        
        /*!***************************************************************************
        * \brief
        * Initializes Shader Storage Buffer Objects (SSBOs) for batch rendering.
        *****************************************************************************/
        static void InitBatchSSBO();
        
        /*!***************************************************************************
        * \brief
        * \brief
        * Initializes the VBOs.
        *
        * \param windowSize The size of the rendering window as a Vector2.
        *****************************************************************************/
        static void Init(const Vector2& windowSize, const CameraManager* m_CamM_Instance = nullptr);

        #pragma endregion

        #pragma region Draw
        /*!***************************************************************************
        * \brief
        * Draws a 2D texture using the specified properties.
        *
        * \param props The properties for rendering the texture, including shader,
        *              texture, color, and transformations.
        *****************************************************************************/
        static void DrawTexture2D(const Renderer2DProps& props = {});
        static void DrawTexture2D(GLuint TextureID, const Renderer2DProps& props = {});
        
        /*!***************************************************************************
        * \brief
        * Draws a batch of 2D textures using the specified properties and batch data.
        *
        * \param props The rendering properties, including shaders, textures, and transformations.
        * \param data The batch instance data including transformation and color information.
        *****************************************************************************/
        static void DrawBatchTexture2D(const Renderer2DProps& props = {}, const Sprite_Batch_Inst& data = {});
        
        /*!***************************************************************************
        * \brief
        * Draws an animated 2D texture with the given properties and texture coordinates.
        *
        * \param props The rendering properties, including shaders, textures, and transformations.
        * \param uv The texture coordinates for the animation frame.
        *****************************************************************************/
        static void DrawAnim2D(const Renderer2DProps& props = {}, const Vector4 uv = Vector4::Zero);
        
        /*!***************************************************************************
        * \brief
        * Draws the post-processing layer after all other rendering operations.
        *****************************************************************************/
        #ifndef GAME
        static void DrawPostProcessingLayer();
        #endif
        #pragma endregion
    };
}