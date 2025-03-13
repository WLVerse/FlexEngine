/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// openglframebuffermanager.cpp
//
// Implements the OpenGLFrameBufferManager class for managing OpenGL
// framebuffers, including initialization, addition, retrieval, and resizing.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "openglframebuffermanager.h"

#include "flexlogger.h"

namespace FlexEngine
{
    // Define static member variable.
    std::unordered_map<std::string, OpenGLFrameBuffer*> OpenGLFrameBufferManager::m_FrameBuffers;

    /*!************************************************************************
     * \brief Destructor that cleans up all dynamically allocated framebuffers.
     *************************************************************************/
    OpenGLFrameBufferManager::~OpenGLFrameBufferManager()
    {
        // Clean up all dynamically allocated framebuffers.
        for (auto& pair : m_FrameBuffers)
        {
            delete pair.second; // Delete each allocated framebuffer.
        }
        m_FrameBuffers.clear();
    }

    /*!************************************************************************
     * \brief Initializes the frame buffer manager with a default framebuffer.
     *
     * This function creates a default framebuffer with dimensions 800x600,
     * adds it to the manager, and sets it as the current framebuffer.
     *************************************************************************/
    void OpenGLFrameBufferManager::Init()
    {
        // Initialize with a default framebuffer.
        OpenGLFrameBuffer* defaultFramebuffer = new OpenGLFrameBuffer();
        defaultFramebuffer->Init(800, 600); // Default size.
        m_FrameBuffers["default"] = defaultFramebuffer;
        m_CurrentFrameBuffer = defaultFramebuffer;
    }

    /*!************************************************************************
     * \brief Adds a new framebuffer to the manager.
     * \param name The unique name identifier for the framebuffer.
     * \param screenDimensions The dimensions (width and height) for the framebuffer.
     *************************************************************************/
    void OpenGLFrameBufferManager::AddFrameBuffer(const std::string& name, Vector2 screenDimensions)
    {
        OpenGLFrameBuffer* newFB = new OpenGLFrameBuffer(static_cast<int>(screenDimensions.x), static_cast<int>(screenDimensions.y));
        m_FrameBuffers[name] = newFB;
    }

    /*!************************************************************************
     * \brief Retrieves a framebuffer by its name.
     * \param name The unique name identifier for the framebuffer.
     * \return Pointer to the framebuffer if found; otherwise, returns nullptr.
     *************************************************************************/
    OpenGLFrameBuffer* OpenGLFrameBufferManager::GetFrameBuffer(const std::string& name)
    {
        auto it = m_FrameBuffers.find(name);
        if (it != m_FrameBuffers.end())
        {
            return it->second;
        }
        return nullptr; // Return null if the framebuffer doesn't exist.
    }

    /*!************************************************************************
     * \brief Sets the current framebuffer by its name.
     * \param name The unique name identifier for the framebuffer to set as current.
     *************************************************************************/
    void OpenGLFrameBufferManager::SetCurrentFrameBuffer(const std::string& name)
    {
        OpenGLFrameBuffer* framebuffer = GetFrameBuffer(name);
        if (framebuffer)
        {
            m_CurrentFrameBuffer = framebuffer;
            m_CurrentFrameBuffer->Bind();
        }
        else
        {
            // Log or handle error: Framebuffer not found.
            Log::Error("Error: Framebuffer not found.");
            OpenGLFrameBuffer::Unbind(); // Unbind to default if not found.
            m_CurrentFrameBuffer = nullptr;
        }
    }

    /*!************************************************************************
     * \brief Retrieves the current active framebuffer.
     * \return Pointer to the current framebuffer.
     *************************************************************************/
    OpenGLFrameBuffer* OpenGLFrameBufferManager::GetCurrentFrameBuffer() const
    {
        return m_CurrentFrameBuffer;
    }

    /*!************************************************************************
     * \brief Resizes all managed framebuffers to the specified dimensions.
     * \param w The new width for the framebuffers.
     * \param h The new height for the framebuffers.
     *************************************************************************/
    void OpenGLFrameBufferManager::ResizeAllFramebuffers(float w, float h)
    {
        for (auto& pair : m_FrameBuffers)
        {
            pair.second->Resize(static_cast<int>(w), static_cast<int>(h));
        }
    }
}
