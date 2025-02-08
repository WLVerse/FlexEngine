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
        // Stores references (or pointers) to existing framebuffers
        static std::unordered_map<std::string, OpenGLFrameBuffer*> m_FrameBuffers;
        OpenGLFrameBuffer* m_CurrentFrameBuffer = nullptr; // Pointer to the currently active framebuffer
    public:
        OpenGLFrameBufferManager() = default;
        ~OpenGLFrameBufferManager();

        void Init();

        // Add a framebuffer to the manager with a specific name
        void AddFrameBuffer(const std::string& name, Vector2 screenDimensions);

        // Get a framebuffer by its name
        OpenGLFrameBuffer* GetFrameBuffer(const std::string& name);

        // Set the current framebuffer by name
        void SetCurrentFrameBuffer(const std::string& name);

        // Get the current framebuffer
        OpenGLFrameBuffer* GetCurrentFrameBuffer() const;

        void ResizeAllFramebuffers(float w, float h);
    };
} // namespace FlexEngine
