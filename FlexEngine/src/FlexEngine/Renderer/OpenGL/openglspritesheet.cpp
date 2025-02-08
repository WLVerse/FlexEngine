// WLVERSE [https://wlverse.web.app]
// openglspritesheet.cpp
//
// The spritesheet helps to manage the texture coordinates of a spritesheet. 
// Simply provide the number of columns and rows in the spritesheet and the
// spritesheet will calculate the UV coordinates for you. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "openglspritesheet.h"

namespace FlexEngine
{

  namespace Asset
  {

    Spritesheet::Spritesheet(File& _metadata)
      : metadata(_metadata)
    {
      // parse the file
      // the file is just a text file with the texture path, columns, and rows
      std::stringstream ss(metadata.Read());
      ss >> texture >> columns >> rows;
    }

    Spritesheet::UV Spritesheet::GetUV(int sprite_index) const
    {
      // get the row and column of the sprite
      int row = sprite_index / columns;
      int column = sprite_index % columns;
      // calculate the uv coordinates
      float u1 = (float)column / columns;
      float v1 = (float)row / rows;
      float u2 = (float)(column + 1) / columns;
      float v2 = (float)(row + 1) / rows;
      //return UV(u1, v1, u2, v2);
      return UV(u1, v2, u2, v1); // flip y
    }

  }

}