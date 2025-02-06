// WLVERSE [https://wlverse.web.app]
// openglmodel.h
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
#include "Renderer/OpenGL/openglmaterial.h"
#include "Renderer/OpenGL/openglmesh.h"

#include <vector>

namespace FlexEngine
{
  namespace Asset
  {
    // A model is defined by a collection of meshes and materials.
    class __FLX_API Model
    {
    public:
      static Model Null;
      //static Model Default; // unit cube

      std::vector<Mesh> meshes;
      std::vector<Material> materials;

      #pragma region Constructors

      Model() = default;

      // Construct a model from a list of meshes and materials.
      // Automatically creates the necessary buffers for the meshes.
      Model(const std::vector<Mesh>& meshes, const std::vector<Material>& materials);

      #pragma endregion

      #pragma region Operator Overloads
      
      bool operator==(const Model& other) const;
      bool operator!=(const Model& other) const;

      operator bool() const;

      #pragma endregion

      #pragma region Internal Functions

    private:

      // INTERNAL FUNCTION
      // Helper function that handles all the buffers for the meshes within the model.
      // Simply calls Internal_CreateBuffers() on each mesh.
      void Internal_CreateBuffers();

      #pragma endregion

    };

  }
}