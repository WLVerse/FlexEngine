// WLVERSE [https://wlverse.web.app]
// openglframebuffer.cpp
//
// Wraps the framebuffer into a class. This class extends handles to allow
// decoupling of framebuffer calls. 
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once
#include "openglframebuffer.h"
#include "flexlogger.h"

namespace FlexEngine 
{
  OpenGLFrameBuffer::OpenGLFrameBuffer(int width, int height)
  {
    Init(width, height);
  }

  OpenGLFrameBuffer::~OpenGLFrameBuffer()
  {
    // Cleanup
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &colorAttachment);
    glDeleteRenderbuffers(1, &depthStencilAttachment);

    Log::Info("Framebuffer deleted with ID: " + std::to_string(framebuffer));
  }

  void OpenGLFrameBuffer::Init(int newWidth, int newHeight)
  {
    width = newWidth;
    height = newHeight;

    // Create and bind framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create texture for color attachment
    glGenTextures(1, &colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

    // Create renderbuffer for depth and stencil attachment
    glGenRenderbuffers(1, &depthStencilAttachment);
    glBindRenderbuffer(GL_RENDERBUFFER, depthStencilAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilAttachment);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      Log::Fatal("Framebuffer is incomplete!");
    }

    Log::Info("Framebuffer created with ID: " + std::to_string(framebuffer));

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer to default after creation
  }

  void OpenGLFrameBuffer::Bind() const 
  {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    GET_OPENGL_ERROR()
  }

  void OpenGLFrameBuffer::Resize(int newWidth, int newHeight) 
  {
    width = newWidth;
    height = newHeight;

    // Resize texture
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    // Resize renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, depthStencilAttachment);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  }

  GLuint OpenGLFrameBuffer::GetColorAttachment() const 
  {
    return colorAttachment;
  }

  void OpenGLFrameBuffer::Clear() const
  {
    Bind();
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Unbind();
  }
} // namespace FlexEngine