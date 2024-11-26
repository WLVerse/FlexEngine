// WLVERSE [https://wlverse.web.app]
// openglmesh.h
// 
// Class to manage mesh data.
// This is tightly coupled with the model class.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Renderer/buffer.h"
#include "Renderer/OpenGL/openglvertex.h"
#include "Renderer/OpenGL/openglshader.h"
#include "FlexMath/matrix4x4.h"

#include <vector>

namespace FlexEngine
{
  namespace Asset
  {

    class __FLX_API Mesh
    { FLX_REFL_SERIALIZABLE
    public:

      // A mesh is a collection of vertices and indices that make up a 3D model.

      std::vector<Vertex> vertices;
      std::vector<unsigned int> indices;

      // The transform matrix of the mesh which is used to offset the mesh from the origin.
      Matrix4x4 transform = Matrix4x4::Identity;

      // The index of the material that the mesh uses.
      // The material is stored in the model's material list.
      std::size_t material_index = 0;

      // The name of the mesh as created by the modeller.
      std::string name = "Unnamed Mesh";

      // The buffers that the mesh uses.
      // Using shared_ptr to avoid copying the buffers when copying the mesh.
      // The buffers are not reflected or serialized, make sure to run Internal_CreateBuffers() after deserialization.

      std::shared_ptr<VertexArray> VAO = nullptr;
      std::shared_ptr<VertexBuffer> VBO = nullptr;
      std::shared_ptr<IndexBuffer> IBO = nullptr;

      #pragma region Constructors

      Mesh() = default;
      Mesh(
        const std::vector<Vertex>& _vertices,
        const std::vector<unsigned int>& _indices,
        const Matrix4x4& _transform = Matrix4x4::Identity,
        std::size_t _material_index = 0,
        const std::string& _name = "Unnamed Mesh"
      );

      #pragma endregion

      #pragma region Operator Overloads

      bool operator==(const Mesh& other) const;
      bool operator!=(const Mesh& other) const;

      #pragma endregion

      #pragma region Internal Functions

      // INTERNAL FUNCTION
      // Creates all the buffers for the mesh.
      // Memory is automatically managed.
      void Internal_CreateBuffers();

      #pragma endregion

    };

  }
}
