#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace MicroChess
{

  class BoundingBox2D
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 size = Vector2::One;
  };

  class OnHover
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_hovering;
    bool on_enter;
    bool on_exit;
  };

  class OnClick
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_clicked;
  };

}
