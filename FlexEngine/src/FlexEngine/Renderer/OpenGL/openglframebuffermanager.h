/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * openglframebuffermanager.h
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

#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include <string>
#include "flx_api.h"
#include "Renderer/OpenGL/opengldebugger.h"
#include "Renderer/OpenGL/openglframebuffer.h"
#include <FlexMath/vector2.h>

namespace FlexEngine
{
    class __FLX_API OpenGLFrameBufferManager
    {
        // Stores references (or pointers) to existing framebuffers.
        static std::unordered_map<std::string, OpenGLFrameBuffer*> m_FrameBuffers;
        OpenGLFrameBuffer* m_CurrentFrameBuffer = nullptr; // Pointer to the currently active framebuffer.

    public:
        /*!
         * \brief Default constructor.
         *
         * Constructs an OpenGLFrameBufferManager instance with no active framebuffer.
         */
        OpenGLFrameBufferManager() = default;

        /*!
         * \brief Destructor.
         *
         * Releases all allocated framebuffer resources managed by the OpenGLFrameBufferManager.
         */
        ~OpenGLFrameBufferManager();

        /*!
         * \brief Initializes the framebuffer manager.
         *
         * Prepares the framebuffer manager for use by setting up necessary resources or
         * state required for managing OpenGL framebuffers.
         */
        void Init();

        /*!
         * \brief Adds a framebuffer to the manager with a specific name.
         *
         * Creates and registers a new OpenGLFrameBuffer with the given screen dimensions,
         * associating it with a unique name for future retrieval.
         *
         * \param name A unique identifier for the framebuffer.
         * \param screenDimensions The dimensions (width and height) of the framebuffer.
         */
        void AddFrameBuffer(const std::string& name, Vector2 screenDimensions);

        /*!
         * \brief Retrieves a framebuffer by its name.
         *
         * Searches the internal storage for a framebuffer associated with the provided name.
         *
         * \param name The unique identifier of the framebuffer to retrieve.
         * \return A pointer to the OpenGLFrameBuffer if found, otherwise nullptr.
         */
        OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);

        /*!
         * \brief Sets the current framebuffer by name.
         *
         * Sets the active framebuffer to the one associated with the specified name. This
         * framebuffer will be used for subsequent rendering operations.
         *
         * \param name The unique identifier of the framebuffer to activate.
         */
        void SetCurrentFrameBuffer(const std::string& name);

        /*!
         * \brief Retrieves the currently active framebuffer.
         *
         * \return A pointer to the currently active OpenGLFrameBuffer.
         */
        OpenGLFrameBuffer* GetCurrentFrameBuffer() const;
    };
} // namespace FlexEngine
