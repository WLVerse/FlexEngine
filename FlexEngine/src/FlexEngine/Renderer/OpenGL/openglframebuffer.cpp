// WLVERSE [https://wlverse.web.app] 
// openglframebuffer.cpp
// 
// Wraps the framebuffer into a class. This class extends handles to allow decoupling of framebuffer calls.
// 
// AUTHORS 
// [100%] Yew Chong (yewchong.k\@digipen.edu) 
// - Main Author 
// 
// Copyright (c) 2024 DigiPen, All rights reserved

#pragma once
#include "openglframebuffer.h"
#include "flexlogger.h"

OpenGLFrameBuffer::OpenGLFrameBuffer(int width, int height) : width(width), height(height) 
{
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
    FlexEngine::Log::Fatal("Framebuffer is incomplete!");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind framebuffer
}

OpenGLFrameBuffer::~OpenGLFrameBuffer() 
{
  // Cleanup
  glDeleteFramebuffers(1, &framebuffer);
  glDeleteTextures(1, &colorAttachment);
  glDeleteRenderbuffers(1, &depthStencilAttachment);
}

void OpenGLFrameBuffer::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void OpenGLFrameBuffer::resize(int newWidth, int newHeight) {
  width = newWidth;
  height = newHeight;

  // Resize texture
  glBindTexture(GL_TEXTURE_2D, colorAttachment);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

  // Resize renderbuffer
  glBindRenderbuffer(GL_RENDERBUFFER, depthStencilAttachment);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

GLuint OpenGLFrameBuffer::getColorAttachment() const {
  return colorAttachment;
}