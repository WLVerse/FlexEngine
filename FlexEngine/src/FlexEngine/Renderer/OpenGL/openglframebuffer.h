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

class OpenGLFrameBuffer {
public:
  OpenGLFrameBuffer(int width, int height);
  ~OpenGLFrameBuffer();

  // Member call to replace the bind call
  void Bind() const;

  // Universal unbind to default framebuffer
  static void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

  // Resize the current framebuffer
  void resize(int newWidth, int newHeight);

  GLuint getColorAttachment() const;

  int getWidth() const { return width; }
  int getHeight() const { return height; }

private:
  GLuint framebuffer;
  GLuint colorAttachment;
  GLuint depthStencilAttachment;
  int width, height;
};