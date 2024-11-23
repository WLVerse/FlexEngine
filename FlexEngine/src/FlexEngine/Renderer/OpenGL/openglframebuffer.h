#pragma once
#include <glad/glad.h>
#include <vector>
#include "FlexMath/Vector4.h"

namespace FlexEngine 
{
    class OpenGLFrameBuffer 
    {
        static GLuint m_editorFBO;         //replace default framebuffer
        static GLuint m_postProcessingFBO; //framebuffer to handle post-processing
        static GLuint m_bloomFBO;          //framebuffer to handle bloom exclusively

    public:
        static GLuint m_pingpongTex[2];
        static GLuint m_postProcessingTex;
        static GLuint m_editorTex;
        static GLuint m_finalRenderTex;

        enum __FLX_API CreatedTextureID
        {
            CID_editor,
            CID_finalRender,
            CID_brightnessPass,
            CID_blur,
        };

        static void Init(const Vector2& windowSize);
        /*!***************************************************************************
        * \brief
        * Sets the default framebuffer for rendering.
        *****************************************************************************/
        static void SetEditorFrameBuffer();
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
    };
}