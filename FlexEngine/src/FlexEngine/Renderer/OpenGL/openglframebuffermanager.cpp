/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * openglframebuffermanager.cpp
 *
 * This file implements the OpenGLFrameBufferManager class, which is responsible
 * for managing and handling OpenGL framebuffers within the engine. It provides
 * functionality for initializing, adding, retrieving, and setting the current
 * framebuffer used in rendering operations.
 *
 * Key functionalities include:
 * - Initialization of the framebuffer manager.
 * - Management of multiple framebuffers identified by unique names.
 * - Setting and retrieving the active framebuffer for rendering.
 *
 * AUTHORS
 * [100%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Main Author
 *
 * Copyright (c) 2025 DigiPen, All rights reserved.
 **************************************************************************/
#include "openglframebuffermanager.h"
#include "flexlogger.h"

namespace FlexEngine
{
    // Static container to hold all framebuffers by name.
    std::unordered_map<std::string, OpenGLFrameBuffer*> OpenGLFrameBufferManager::m_FrameBuffers;

    /*!
     * @brief Destructor for the OpenGLFrameBufferManager class.
     *
     * Cleans up all dynamically allocated framebuffers by iterating over the
     * stored framebuffer map, deleting each framebuffer, and then clearing the map.
     */
    OpenGLFrameBufferManager::~OpenGLFrameBufferManager()
    {
        // Clean up all dynamically allocated framebuffers
        for (auto& pair : m_FrameBuffers)
        {
            delete pair.second; // Delete each allocated framebuffer
        }
        m_FrameBuffers.clear();
    }

    /*!
     * @brief Initializes the FrameBuffer Manager.
     *
     * Creates and initializes a default framebuffer with preset dimensions (800 x 600)
     * and sets it as the current framebuffer.
     */
    void OpenGLFrameBufferManager::Init()
    {
        // Initialize with a default framebuffer
        OpenGLFrameBuffer* defaultFramebuffer = new OpenGLFrameBuffer();
        defaultFramebuffer->Init(800, 600); // Default size
        m_FrameBuffers["default"] = defaultFramebuffer;
        m_CurrentFrameBuffer = defaultFramebuffer;
    }

    /*!
     * @brief Adds a new framebuffer.
     *
     * Creates a new OpenGL framebuffer with the specified screen dimensions and stores
     * it in the manager under the provided name.
     *
     * @param name A unique string identifier for the new framebuffer.
     * @param screenDimensions A Vector2 specifying the width (x) and height (y) for the framebuffer.
     */
    void OpenGLFrameBufferManager::AddFrameBuffer(const std::string& name, Vector2 screenDimensions)
    {
        OpenGLFrameBuffer* newFB = new OpenGLFrameBuffer(static_cast<int>(screenDimensions.x), static_cast<int>(screenDimensions.y));
        m_FrameBuffers[name] = newFB;
    }

    /*!
     * @brief Retrieves a framebuffer by its name.
     *
     * Searches for the framebuffer stored under the provided name and returns a pointer
     * to it. If no framebuffer is found, returns a null pointer.
     *
     * @param name The unique string identifier for the framebuffer.
     * @return Pointer to the requested OpenGLFrameBuffer, or nullptr if not found.
     */
    OpenGLFrameBuffer* OpenGLFrameBufferManager::GetFrameBuffer(const std::string& name)
    {
        auto it = m_FrameBuffers.find(name);
        if (it != m_FrameBuffers.end())
        {
            return it->second;
        }
        return nullptr; // Return null if the framebuffer doesn't exist
    }

    /*!
     * @brief Sets the current active framebuffer.
     *
     * Sets the framebuffer identified by the provided name as the current framebuffer.
     * If the framebuffer exists, it binds the framebuffer for rendering operations.
     * If the framebuffer does not exist, it logs an error message and unbinds any active framebuffer.
     *
     * @param name The unique string identifier for the framebuffer to be activated.
     */
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
            // Log or handle error: Framebuffer not found
            Log::Error("Error: Framebuffer not found.");
            OpenGLFrameBuffer::Unbind(); // Unbind to default if not found
            m_CurrentFrameBuffer = nullptr;
        }
    }

    /*!
     * @brief Retrieves the currently active framebuffer.
     *
     * @return A pointer to the current OpenGLFrameBuffer.
     */
    OpenGLFrameBuffer* OpenGLFrameBufferManager::GetCurrentFrameBuffer() const
    {
        return m_CurrentFrameBuffer;
    }
}