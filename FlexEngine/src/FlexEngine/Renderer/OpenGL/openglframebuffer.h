// WLVERSE [https://wlverse.web.app] 
// openglframebuffer.h
// 
// Wraps the framebuffer into a class. This class extends handles to allow decoupling of framebuffer calls.
// 
// AUTHORS 
// [100%] Yew Chong (yewchong.k\@digipen.edu) 
// - Main Author 
// 
// Copyright (c) 2024 DigiPen, All rights reserved

#pragma once
#include <glad/glad.h>
#include "flx_api.h"
#include "Renderer/OpenGL/opengldebugger.h"

namespace FlexEngine 
{
  class __FLX_API OpenGLFrameBuffer {
  public:
    OpenGLFrameBuffer() = default;              // Allows for a default construction of framebuffer for forward declaration, but know that this is not a valid framebuffer.
    OpenGLFrameBuffer(int width, int height);
    ~OpenGLFrameBuffer();

    // A direct copy paste of the constructor for framebuffers that have been default constructed.
    void Init(int newWidth, int newHeight);

    // Member call to replace the bind call
    void Bind() const;

    // Universal unbind to default framebuffer, with an additional check for failures during swap buffers.
    static void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); GET_OPENGL_ERROR() }

    // Resize the current framebuffer
    void Resize(int newWidth, int newHeight);

    GLuint GetColorAttachment() const;

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }

  private:
    GLuint framebuffer;
    GLuint colorAttachment;
    GLuint depthStencilAttachment;
    int width, height;
  };
} // namespace FlexEngine