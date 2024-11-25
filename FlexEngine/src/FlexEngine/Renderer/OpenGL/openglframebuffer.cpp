#include "OpenGLFrameBuffer.h"
#include "FlexEngine/AssetManager/assetmanager.h" // FLX_ASSET_GET
#include "FlexEngine/DataStructures/freequeue.h"

namespace FlexEngine 
{
    #pragma region old
    //GLuint OpenGLFrameBuffer::m_editorFBO = 0;
    //GLuint OpenGLFrameBuffer::m_gameFBO = 0;
    //GLuint OpenGLFrameBuffer::m_postProcessingFBO = 0;
    //GLuint OpenGLFrameBuffer::m_bloomFBO = 0;
    //GLuint OpenGLFrameBuffer::m_pingpongTex[2] = {};
    //GLuint OpenGLFrameBuffer::m_postProcessingTex = 0;
    //GLuint OpenGLFrameBuffer::m_editorTex = 0;
    //GLuint OpenGLFrameBuffer::m_gameRenderTex = 0;
    //GLuint OpenGLFrameBuffer::m_finalRenderTex = 0;

    //void OpenGLFrameBuffer::Init(const Vector2& windowSize) 
    //{
    //    //////////////////////////////////////////////////////////////////////////////////////////////////
    //    // Create relevant FBO 
    //    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    //    glGenFramebuffers(1, &m_postProcessingFBO); //For final composite post-process
    //    SetPostProcessingFrameBuffer();

    //    // Create Post-processing FBO
    //    glGenTextures(1, &m_postProcessingTex);
    //    glBindTexture(GL_TEXTURE_2D, m_postProcessingTex);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_postProcessingTex, 0);
    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        Log::Fatal("Post-Processing Framebuffer error: " + glCheckFramebufferStatus(GL_FRAMEBUFFER));

    //    // Create Specialized Post-processing FBO : BLOOM
    //    glGenFramebuffers(1, &m_bloomFBO); //For gaussian blurring
    //    SetBloomFrameBuffer();
    //    glGenTextures(2, m_pingpongTex);
    //    glBindTexture(GL_TEXTURE_2D, m_pingpongTex[0]);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pingpongTex[0], 0);
    //    glBindTexture(GL_TEXTURE_2D, m_pingpongTex[1]);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_pingpongTex[1], 0);
    //    glDrawBuffers(2, drawBuffers);
    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        Log::Fatal("Bloom Framebuffer error: " + glCheckFramebufferStatus(GL_FRAMEBUFFER));

    //    // Create game FBO
    //    glGenFramebuffers(1, &m_gameFBO);
    //    SetGameFrameBuffer();
    //    glGenTextures(1, &m_gameRenderTex);
    //    glBindTexture(GL_TEXTURE_2D, m_gameRenderTex);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gameRenderTex, 0);
    //    glGenTextures(1, &m_finalRenderTex);
    //    glBindTexture(GL_TEXTURE_2D, m_finalRenderTex);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_finalRenderTex, 0);
    //    glDrawBuffers(2, drawBuffers);
    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        Log::Fatal("Game Framebuffer error: " + glCheckFramebufferStatus(GL_FRAMEBUFFER));

    //    // Create editor FBO
    //    glGenFramebuffers(1, &m_editorFBO); //For gaussian blurring
    //    SetEditorFrameBuffer();
    //    glGenTextures(1, &m_editorTex);
    //    glBindTexture(GL_TEXTURE_2D, m_editorTex);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_editorTex, 0);
    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        Log::Fatal("Post-Processing Framebuffer error: " + glCheckFramebufferStatus(GL_FRAMEBUFFER));

    //    // Unbind frame buffer
    //    SetDefaultFrameBuffer();

    //    FreeQueue::Push(
    //    [=]()
    //    {
    //        glDeleteFramebuffers(1, &m_postProcessingFBO);
    //        glDeleteFramebuffers(1, &m_editorFBO);
    //        glDeleteFramebuffers(1, &m_bloomFBO);
    //        glDeleteFramebuffers(1, &m_gameFBO);

    //        glDeleteTextures(1, &m_postProcessingTex);
    //        glDeleteTextures(1, &m_editorTex);
    //        glDeleteTextures(1, &m_gameRenderTex);
    //        glDeleteTextures(1, &m_finalRenderTex);
    //        glDeleteTextures(2, m_pingpongTex);
    //    }
    //    );
    //}

    //void OpenGLFrameBuffer::SetDefaultFrameBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    //void OpenGLFrameBuffer::SetEditorFrameBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, m_editorFBO );}

    //void OpenGLFrameBuffer::SetGameFrameBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, m_gameFBO); }

    //void OpenGLFrameBuffer::SetPostProcessingFrameBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, m_postProcessingFBO); }

    //void OpenGLFrameBuffer::SetBloomFrameBuffer() { glBindFramebuffer(GL_FRAMEBUFFER, m_bloomFBO); }

    ///*!***************************************************************************
    //* \brief
    //* Clears the current framebuffer.
    //*****************************************************************************/
    //void OpenGLFrameBuffer::ClearFrameBuffer() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

    //GLuint OpenGLFrameBuffer::GetCreatedTexture(GLuint textureID) 
    //{
    //    switch (textureID)
    //    {
    //    case CreatedTextureID::CID_finalRender:
    //        return m_finalRenderTex;
    //    case CreatedTextureID::CID_blur:
    //        return m_pingpongTex[0];
    //    case CreatedTextureID::CID_editor:
    //        return m_editorTex;
    //    case CreatedTextureID::CID_gameRender:
    //    default:
    //        return CID_gameRender;
    //    }
    //}

    //GLuint OpenGLFrameBuffer::GetCurrFrameBuffer() 
    //{
    //    GLint currentFBO = 0;
    //    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO);
    //    return static_cast<GLuint>(currentFBO);
    //}

    //bool OpenGLFrameBuffer::CheckSameFrameBuffer(const GLuint framebufferID) 
    //{
    //    const GLuint currentFBO = GetCurrFrameBuffer();
    //    return currentFBO == framebufferID;
    //}

    #pragma endregion

    #pragma region new
    std::unordered_map<std::string, OpenGLFrameBuffer::FBOData> OpenGLFrameBuffer::m_framebuffers;

    void OpenGLFrameBuffer::Init(const Vector2& windowSize)
    {
        CreateFramebuffer("EDITOR", 1, windowSize);

    }

    void OpenGLFrameBuffer::CreateFramebuffer(const std::string& name, int numTextures, const Vector2& windowSize)
    {
        FBOData fboData;
        glGenFramebuffers(1, &fboData.fboID);
        glBindFramebuffer(GL_FRAMEBUFFER, fboData.fboID);

        GLenum attachment = GL_COLOR_ATTACHMENT0;

        for (int i = 0; i < numTextures; ++i)
        {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                         static_cast<GLsizei>(windowSize.x), static_cast<GLsizei>(windowSize.y),
                         0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment++, GL_TEXTURE_2D, textureID, 0);

            fboData.textures.push_back(textureID);
        }

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            Log::Fatal("Framebuffer error for: " + name);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_framebuffers[name] = fboData;
    }

    void OpenGLFrameBuffer::BindFramebuffer(const std::string& name)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffers[name].fboID);
    }

    void OpenGLFrameBuffer::UnbindFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    GLuint OpenGLFrameBuffer::GetFramebufferTexture(const std::string& name, int textureIndex)
    {
        return m_framebuffers[name].textures[textureIndex];
    }

    void OpenGLFrameBuffer::DeleteFramebuffer(const std::string& name)
    {
        auto& fboData = m_framebuffers[name];
        for (GLuint texture : fboData.textures)
        {
            glDeleteTextures(1, &texture);
        }
        glDeleteFramebuffers(1, &fboData.fboID);
        m_framebuffers.erase(name);
    }

    void OpenGLFrameBuffer::ClearFrameBuffer()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLFrameBuffer::RegenerateTextures(const Vector2& newSize)
    {
        for (auto& [name, fboData] : m_framebuffers)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fboData.fboID);

            GLenum attachment = GL_COLOR_ATTACHMENT0;

            for (GLuint texture : fboData.textures)
            {
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F,
                             static_cast<GLsizei>(newSize.x), static_cast<GLsizei>(newSize.y),
                             0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glFramebufferTexture2D(GL_FRAMEBUFFER, attachment++, GL_TEXTURE_2D, texture, 0);
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    #pragma endregion
}