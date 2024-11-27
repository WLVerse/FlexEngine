#pragma once
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
    
        static void RegenerateAllTextures(GLsizei width, GLsizei height);
    };
}