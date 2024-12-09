#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace MicroChess
{

  enum CursorType : int
  {
    CURSORTYPE_NONE = -1,
    CURSORTYPE_DEFAULT = 0,
    CURSORTYPE_POINTER,
    CURSORTYPE_DISABLED,
    CURSORTYPE_DRAG_HOVER,
    CURSORTYPE_DRAG_CLICK,
    CURSORTYPE_CROSS,
    CURSORTYPE_BUSY,
    CURSORTYPE_HELP
  };

  class CustomCursor
  { FLX_REFL_SERIALIZABLE
  public:
    int type = CURSORTYPE_DEFAULT;
  };

  class IsWhite
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_white = true;
  };

  class Board
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 size = Vector2(4, 4);
  };

  class BoardTile
  { FLX_REFL_SERIALIZABLE
  };

  class PiecePosition
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 position = Vector2(0, 0);
  };

  enum PieceTypes : int
  {
    PIECETYPE_PAWN = 0,
    PIECETYPE_ROOK = 1,
    PIECETYPE_KNIGHT = 2,
    PIECETYPE_BISHOP = 3,
    PIECETYPE_QUEEN = 4,
    PIECETYPE_KING = 5
  };

  class PieceType
  { FLX_REFL_SERIALIZABLE
  public:
    int type = PIECETYPE_QUEEN;
  };

  class PieceStatus
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_hovered;
    bool is_dragged;
  };

}
