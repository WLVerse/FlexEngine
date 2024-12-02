// WLVERSE [https://wlverse.web.app]
// openglvertex.h
// 
// Class to store vertex data.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Reflection/base.h"

#include "FlexMath/vector3.h"

namespace FlexEngine
{
  
  class __FLX_API Vertex
  { FLX_REFL_SERIALIZABLE
  public:
    Vector3 position;
    Vector3 color;
    Vector2 tex_coords;
    Vector3 normal;
    Vector3 tangent;
    Vector3 bitangent;
    //unsigned int object_id;

    Vertex(
      Vector3 _position = Vector3::Zero,
      Vector3 _color = Vector3::Zero,
      Vector2 _tex_coords = Vector2::Zero,
      Vector3 _normal = Vector3::Zero,
      Vector3 _tangent = Vector3::Zero,
      Vector3 _bitangent = Vector3::Zero
    );

    static void SetLayout();

    #pragma region Operator Overloads

    bool operator==(const Vertex& other) const;
    bool operator!=(const Vertex& other) const;

    #pragma endregion

  };

}