#pragma once

namespace Editor
{
  class Position
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector2 position = Vector2::Zero;
  };

  class Scale
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector2 scale = Vector2::One;
  };

  class Rotation
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector3 rotation = Vector3::Zero;
  };

  class Transform
  {
    FLX_REFL_SERIALIZABLE
  public:
    bool is_dirty = true; //Determines if transform needs to be updated
    Matrix4x4 transform = Matrix4x4::Identity;
  };
}