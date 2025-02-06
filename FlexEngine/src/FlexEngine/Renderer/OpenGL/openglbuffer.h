// WLVERSE [https://wlverse.web.app]
// openglbuffer.h
//
// Unused. This will become a part of the 3D renderer. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Renderer/buffer.h"

namespace FlexEngine
{

  #pragma region OpenGLVertexArray

  class __FLX_API OpenGLVertexArray : public VertexArray
  {
    unsigned int m_binding_point = 0;

  public:
    OpenGLVertexArray(unsigned int binding_point = 0);
    virtual ~OpenGLVertexArray();

    virtual void Bind() const;
    virtual void Unbind() const;
  };

  #pragma endregion

  #pragma region OpenGLVertexBuffer

  class __FLX_API OpenGLVertexBuffer : public VertexBuffer
  {
    unsigned int m_binding_point = 0;

  public:
    OpenGLVertexBuffer(Vertex* vertices, std::size_t size, unsigned int binding_point = 0);
    virtual ~OpenGLVertexBuffer();

    virtual void Bind() const;
    virtual void Unbind() const;
  };

  #pragma endregion

  #pragma region OpenGLIndexBuffer

  class __FLX_API OpenGLIndexBuffer : public IndexBuffer
  {
    unsigned int m_binding_point = 0;
    GLsizei m_count = 0;

  public:
    OpenGLIndexBuffer(unsigned int* indices, GLsizei count, unsigned int binding_point = 0);
    virtual ~OpenGLIndexBuffer();

    virtual void Bind() const;
    virtual void Unbind() const;

    virtual GLsizei GetCount() const;
  };

  #pragma endregion

}