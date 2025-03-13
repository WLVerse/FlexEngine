/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// openglframebuffermanager.h
//
// Declares the OpenGLFrameBufferManager class for managing OpenGL framebuffers,
// including initialization, addition, retrieval, setting the current framebuffer,
// and resizing operations.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

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
        OpenGLFrameBufferManager() = default;

        /*!************************************************************************
         * \brief Destructor that cleans up all allocated framebuffers.
         *************************************************************************/
        ~OpenGLFrameBufferManager();

        /*!************************************************************************
         * \brief Initializes the framebuffer manager with a default framebuffer.
         *************************************************************************/
        void Init();

        /*!************************************************************************
         * \brief Adds a framebuffer to the manager with a specific name.
         * \param name The unique name identifier for the framebuffer.
         * \param screenDimensions The dimensions (width and height) for the framebuffer.
         *************************************************************************/
        void AddFrameBuffer(const std::string& name, Vector2 screenDimensions);

        /*!************************************************************************
         * \brief Retrieves a framebuffer by its name.
         * \param name The unique name identifier for the framebuffer.
         * \return Pointer to the framebuffer if found; otherwise, returns nullptr.
         *************************************************************************/
        OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);

        /*!************************************************************************
         * \brief Sets the current framebuffer by its name.
         * \param name The unique name identifier for the framebuffer to set as current.
         *************************************************************************/
        void SetCurrentFrameBuffer(const std::string& name);

        /*!************************************************************************
         * \brief Retrieves the current active framebuffer.
         * \return Pointer to the current framebuffer.
         *************************************************************************/
        OpenGLFrameBuffer* GetCurrentFrameBuffer() const;

        /*!************************************************************************
         * \brief Resizes all managed framebuffers to the specified dimensions.
         * \param w The new width for the framebuffers.
         * \param h The new height for the framebuffers.
         *************************************************************************/
        void ResizeAllFramebuffers(float w, float h);
    };
} // namespace FlexEngine
