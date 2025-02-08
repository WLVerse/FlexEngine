#include "openglframebuffermanager.h"

#include "flexlogger.h"

namespace FlexEngine
{
    std::unordered_map<std::string, OpenGLFrameBuffer*> OpenGLFrameBufferManager::m_FrameBuffers;

    OpenGLFrameBufferManager::~OpenGLFrameBufferManager() 
    {
        // Clean up all dynamically allocated framebuffers
        for (auto& pair : m_FrameBuffers) 
        {
            delete pair.second; // Delete each allocated framebuffer
        }
        m_FrameBuffers.clear();
    }

    void OpenGLFrameBufferManager::Init()
    {
        // Initialize with a default framebuffer
        OpenGLFrameBuffer* defaultFramebuffer = new OpenGLFrameBuffer();
        defaultFramebuffer->Init(800, 600); // Default size
        m_FrameBuffers["default"] = defaultFramebuffer;
        m_CurrentFrameBuffer = defaultFramebuffer;
    }

    void OpenGLFrameBufferManager::AddFrameBuffer(const std::string& name, Vector2 screenDimensions)
    {
        OpenGLFrameBuffer* newFB = new OpenGLFrameBuffer(static_cast<int>(screenDimensions.x), static_cast<int>(screenDimensions.y));
        m_FrameBuffers[name] = newFB;
    }

    OpenGLFrameBuffer* OpenGLFrameBufferManager::GetFrameBuffer(const std::string& name) 
    {
        auto it = m_FrameBuffers.find(name);
        if (it != m_FrameBuffers.end()) 
        {
            return it->second;
        }
        return nullptr; // Return null if the framebuffer doesn't exist
    }

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

    OpenGLFrameBuffer* OpenGLFrameBufferManager::GetCurrentFrameBuffer() const { return m_CurrentFrameBuffer; }

    void OpenGLFrameBufferManager::ResizeAllFramebuffers(float w, float h)
    {
      for (auto& pair : m_FrameBuffers)
      {
        pair.second->Resize(w, h);
      }
    }
}