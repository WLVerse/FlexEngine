// WLVERSE [https://wlverse.web.app]
// openglvertex.cpp
//
// Unused. This will become a part of the 3D renderer. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "openglvertex.h"

#include <glad/glad.h>

namespace FlexEngine
{

  #pragma region Reflection

  FLX_REFL_REGISTER_START(Vertex)
    FLX_REFL_REGISTER_PROPERTY(position)
    FLX_REFL_REGISTER_PROPERTY(color)
    FLX_REFL_REGISTER_PROPERTY(tex_coords)
    FLX_REFL_REGISTER_PROPERTY(normal)
    FLX_REFL_REGISTER_PROPERTY(tangent)
    FLX_REFL_REGISTER_PROPERTY(bitangent)
  FLX_REFL_REGISTER_END;

  #pragma endregion

  Vertex::Vertex(
    Vector3 _position,
    Vector3 _color,
    Vector2 _tex_coords,
    Vector3 _normal,
    Vector3 _tangent,
    Vector3 _bitangent
  ) : position(_position)
    , color(_color)
    , tex_coords(_tex_coords)
    , normal(_normal)
    , tangent(_tangent)
    , bitangent(_bitangent)
  {
  }

  void Vertex::SetLayout()
  {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tex_coords));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, bitangent));
  }

  #pragma region Operator Overloads

  bool Vertex::operator==(const Vertex& other) const
  {
    return (
      position == other.position &&
      color == other.color &&
      tex_coords == other.tex_coords &&
      normal == other.normal &&
      tangent == other.tangent &&
      bitangent == other.bitangent
    );
  }

  bool Vertex::operator!=(const Vertex& other) const
  {
    return !(*this == other);
  }

  #pragma endregion

}