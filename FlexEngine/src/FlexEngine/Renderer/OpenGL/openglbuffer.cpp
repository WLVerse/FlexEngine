// WLVERSE [https://wlverse.web.app]
// openglbuffer.cpp
//
// Unused. This will become a part of the 3D renderer. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "Renderer/OpenGL/openglbuffer.h"

#include <glad/glad.h>

namespace FlexEngine
{

  #pragma region OpenGLVertexArray

  OpenGLVertexArray::OpenGLVertexArray(unsigned int binding_point)
    : m_binding_point(binding_point)
  {
    glGenVertexArrays(1, &m_binding_point);
  }

  OpenGLVertexArray::~OpenGLVertexArray()
  {
    glDeleteVertexArrays(1, &m_binding_point);
  }

  void OpenGLVertexArray::Bind() const
  {
    glBindVertexArray(m_binding_point);

    // Hardcoded vertex layout
    //Vertex::SetLayout();
  }

  void OpenGLVertexArray::Unbind() const
  {
    glBindVertexArray(0);
  }

  #pragma endregion

  #pragma region OpenGLVertexBuffer

  OpenGLVertexBuffer::OpenGLVertexBuffer(Vertex* vertices, std::size_t size, unsigned int binding_point)
    : m_binding_point(binding_point)
  {
    glGenBuffers(1, &m_binding_point);
    glBindBuffer(GL_ARRAY_BUFFER, m_binding_point);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  }

  OpenGLVertexBuffer::~OpenGLVertexBuffer()
  {
    glDeleteBuffers(1, &m_binding_point);
  }

  void OpenGLVertexBuffer::Bind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, m_binding_point);
  }

  void OpenGLVertexBuffer::Unbind() const
  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  #pragma endregion

  #pragma region OpenGLIndexBuffer

  OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indices, GLsizei count, unsigned int binding_point)
    : m_count(count), m_binding_point(binding_point)
  {
    glGenBuffers(1, &m_binding_point);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_binding_point);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
  }

  OpenGLIndexBuffer::~OpenGLIndexBuffer()
  {
    glDeleteBuffers(1, &m_binding_point);
  }

  void OpenGLIndexBuffer::Bind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_binding_point);
  }

  void OpenGLIndexBuffer::Unbind() const
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  GLsizei OpenGLIndexBuffer::GetCount() const
  {
    return m_count;
  }

  #pragma endregion

}