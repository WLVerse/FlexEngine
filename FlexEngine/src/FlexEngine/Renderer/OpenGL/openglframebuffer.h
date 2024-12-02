#pragma once
/*!************************************************************************
 // WLVERSE [https://wlverse.web.app]
 // openglframebuffer.h
 //
 // This header defines the `OpenGLFrameBuffer` class, a utility within the
 // FlexEngine framework for managing framebuffer objects (FBOs) used in
 // rendering pipelines. The `OpenGLFrameBuffer` class provides static
 // methods and members to handle multiple rendering targets and post-processing
 // stages efficiently.
 //
 // Key Responsibilities:
 // - Managing framebuffers for different rendering contexts:
 //   - Editor camera rendering (m_editorFBO)
 //   - In-game camera rendering (m_gameFBO)
 //   - Post-processing effects (m_postProcessingFBO)
 //   - Bloom rendering (m_bloomFBO)
 // - Managing associated textures for framebuffers, including ping-pong textures
 //   for effects like blur and the final rendered output.
 // - Providing utility functions for framebuffer operations such as setting,
 //   clearing, and verifying the currently bound framebuffer.
 // - Allowing dynamic regeneration of textures to accommodate window resizing.
 //
 // Main Features:
 // - Separate framebuffers for editor and in-game rendering.
 // - Support for advanced rendering effects such as post-processing and bloom.
 // - Efficient texture management and resizing.
 // - Static interface for global framebuffer operations.
 //
 // AUTHORS
 // [100%] Soh Wei Jie (weijie.soh@digipen.edu)
 //   - Main Author
 //   - Implemented framebuffer setup, texture management, and utility functions.
 //
 // Copyright (c) 2024 DigiPen, All rights reserved.
 **************************************************************************/
#include <vector>
#include "FlexMath/Vector4.h"
#include <glad/glad.h>
namespace FlexEngine 
{
    class OpenGLFrameBuffer 
    {
    public:
        static GLuint m_editorFBO;         //framebuffer to handle editor cam
        static GLuint m_gameFBO;           //framebuffer to handle in-game cam
        static GLuint m_postProcessingFBO; //framebuffer to handle post-processing
        static GLuint m_bloomFBO;          //framebuffer to handle bloom exclusively
        
        static GLuint m_pingpongTex[2];
        static GLuint m_postProcessingTex;
        static GLuint m_editorTex;
        static GLuint m_gameRenderTex;
        static GLuint m_finalRenderTex;

        enum __FLX_API CreatedTextureID
        {
            CID_editor,
            CID_gameRender,
            CID_finalRender,
            CID_brightnessPass,
            CID_blur,
        };

        /*!***************************************************************************
         * \brief
         * Initializes framebuffers and their associated textures based on the
         * given window size.
         *
         * \param windowSize The size of the window for which textures should be
         *                   generated.
         *****************************************************************************/
        static void Init(const Vector2& windowSize);
        /*!***************************************************************************
        * \brief
        * Sets the editor framebuffer for rendering.
        *****************************************************************************/
        static void SetEditorFrameBuffer();
        /*!***************************************************************************
        * \brief
        * Sets the game framebuffer for rendering.
        *****************************************************************************/
        static void SetGameFrameBuffer();
        /*!***************************************************************************
        * \brief
        * Sets the default framebuffer for rendering.
        *****************************************************************************/
        static void SetDefaultFrameBuffer();
        /*!***************************************************************************
        * \brief
        * Enables post-processing effects for rendering.
        *****************************************************************************/
        static void SetPostProcessingFrameBuffer();
        /*!***************************************************************************
        * \brief
        * Sets the framebuffer specifically for bloom post-processing effects.
        *****************************************************************************/
        static void SetBloomFrameBuffer();
        /*!***************************************************************************
        * \brief
        * Clears the current framebuffer.
        *****************************************************************************/
        static void ClearFrameBuffer();
        /*!***************************************************************************
        * \brief
        * Retrieves the texture ID associated with a specific framebuffer texture type.
        *
        * \param textureID The ID of the texture type (from `CreatedTextureID` enum).
        * \return The OpenGL texture ID corresponding to the requested type.
        *****************************************************************************/
        static GLuint GetCreatedTexture(GLuint textureID);
        /*!***************************************************************************
        * \brief Retrieves the ID of the currently bound framebuffer.
        * \return The ID of the currently bound framebuffer.
        *****************************************************************************/
        static GLuint GetCurrFrameBuffer();
        /*!**************************************************************************
        * \brief Checks if the currently bound framebuffer matches the given framebuffer ID.
        * \param framebufferID The ID of the framebuffer to compare against.
        * \return True if the currently bound framebuffer matches the given ID, false otherwise.
        ***************************************************************************/
        static bool CheckSameFrameBuffer(const GLuint framebufferID);
        /*!***************************************************************************
        * \brief
        * Regenerates all framebuffer textures to fit a new window size.
        *
        * \param width The new width of the window.
        * \param height The new height of the window.
        *****************************************************************************/
        static void RegenerateAllTextures(GLsizei width, GLsizei height);
    };
}