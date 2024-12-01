// WLVERSE [https://wlverse.web.app]
// buffer.h
// 
// Generic interface for buffers.
// This is a test to see how support for multiple renderers can be implemented.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Renderer/OpenGL/openglvertex.h"

#include <glad/glad.h>

#include <vector>
#include <memory> // std::shared_ptr

namespace FlexEngine
{

  #pragma region VertexArray

  class __FLX_API VertexArray
  {
  public:
    virtual ~VertexArray() {}

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    // Best practice to store the pointer in a unique_ptr or shared_ptr
    // Usage: std::unique_ptr<VertexArray> vertex_array;
    //        vertex_array.reset(VertexArray::Create());
    static VertexArray* Create(unsigned int binding_point = 0);
  };

  #pragma endregion

  #pragma region VertexBuffer

  class __FLX_API VertexBuffer
  {
  public:
    virtual ~VertexBuffer() {}

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    // Best practice to store the pointer in a unique_ptr or shared_ptr
    // Usage: std::unique_ptr<VertexBuffer> vertex_buffer;
    //        vertex_buffer.reset(VertexBuffer::Create(vertices.data(), sizeof(vertices));
    static VertexBuffer* Create(Vertex* vertices, std::size_t size, unsigned int binding_point = 0);
  };

  #pragma endregion

  #pragma region IndexBuffer

  class __FLX_API IndexBuffer
  {
  public:
    virtual ~IndexBuffer() {}

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual GLsizei GetCount() const = 0;

    // Best practice to store the pointer in a unique_ptr or shared_ptr
    // Usage: std::unique_ptr<IndexBuffer> index_buffer;
    //        index_buffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
    static IndexBuffer* Create(unsigned int* indices, GLsizei count, unsigned int binding_point = 0);
  };

  #pragma endregion

}