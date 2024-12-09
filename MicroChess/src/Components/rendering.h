#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace MicroChess
{

  using EntityName = FlexEngine::FlexECS::Scene::StringIndex;

  class IsActive
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_active;
  };

  class Parent
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Entity parent;
  };

  class Position
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 position;
  };

  class Scale
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 scale;
  };

  class ZIndex
  { FLX_REFL_SERIALIZABLE
  public:
    int z;
  };

  class Shader
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex shader;
  };

  class Sprite
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex texture;
    Vector3 color = Vector3(1.0f, 0.0f, 1.0f);
    Vector3 color_to_add = Vector3::Zero;
    Vector3 color_to_multiply = Vector3::One;
    int alignment = Renderer2DProps::Alignment_Center;
  };

}
