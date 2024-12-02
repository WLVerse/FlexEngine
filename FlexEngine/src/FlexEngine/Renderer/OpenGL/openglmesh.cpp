// WLVERSE [https://wlverse.web.app]
// openglmesh.cpp
// 
// Class to manage mesh data.
// This is tightly coupled with the model class.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "openglmesh.h"

namespace FlexEngine
{
  namespace Asset
  {

    #pragma region Reflection

    FLX_REFL_REGISTER_START(Mesh)
      FLX_REFL_REGISTER_PROPERTY(vertices)
      FLX_REFL_REGISTER_PROPERTY(indices)
      FLX_REFL_REGISTER_PROPERTY(transform)
      FLX_REFL_REGISTER_PROPERTY(material_index)
      FLX_REFL_REGISTER_PROPERTY(name)
    FLX_REFL_REGISTER_END

    #pragma endregion


    #pragma region Constructors

    Mesh::Mesh(
      const std::vector<Vertex>& _vertices,
      const std::vector<unsigned int>& _indices,
      const Matrix4x4& _transform,
      std::size_t _material_index,
      const std::string& _name
    )
      : vertices(_vertices), indices(_indices)
      , transform(_transform), material_index(_material_index)
      , name(_name)
    {
    }

    #pragma endregion

    #pragma region Operator Overloads

    bool Mesh::operator==(const Mesh& other) const
    {
      return
        vertices == other.vertices &&
        indices == other.indices &&
        transform == other.transform &&
        material_index == other.material_index &&
        name == other.name
      ;
    }

    bool Mesh::operator!=(const Mesh& other) const
    {
      return !(*this == other);
    }

    #pragma endregion

    #pragma region Internal Functions

    // The order is very important.
    // 1. Create and bind VAO
    // 3. Create VBO
    // 4. Bind VBO
    // 5. Set VBO layout
    // 6. Create and bind IBO
    // 7. Unbind VAO to prevent further modification
    void Mesh::Internal_CreateBuffers()
    {
      VAO.reset(VertexArray::Create());
      VAO->Bind();

      VBO.reset(VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex)));
      VBO->Bind();
      Vertex::SetLayout();

      IBO.reset(IndexBuffer::Create(indices.data(), static_cast<GLsizei>(indices.size())));
      IBO->Bind();

      VAO->Unbind();
    }

    #pragma endregion

  }
}