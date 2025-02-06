// WLVERSE [https://wlverse.web.app]
// openglmodel.cpp
//
// Unused. This will become a part of the 3D renderer. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "openglmodel.h"

namespace FlexEngine
{
  namespace Asset
  {

    // static member initialization
    Model Model::Null = Model();

    #pragma region Constructors

    Model::Model(const std::vector<Mesh>& _meshes, const std::vector<Material>& _materials)
      : meshes(_meshes), materials(_materials)
    {
      Internal_CreateBuffers();
    }

    #pragma endregion

    #pragma region Operator Overloads

    bool Model::operator==(const Model& other) const
    {
      return meshes == other.meshes && materials == other.materials;
    }

    bool Model::operator!=(const Model& other) const
    {
      return !(*this == other);
    }

    Model::operator bool() const
    {
      return !(*this == Null);
    }

    #pragma endregion

    #pragma region Internal Functions

    void Model::Internal_CreateBuffers()
    {
      for (auto& mesh : meshes)
      {
        mesh.Internal_CreateBuffers();
      }
    }

    #pragma endregion

  }
}