// WLVERSE [https://wlverse.web.app]
// openglspritesheet.h
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

#pragma once

#include "flx_api.h"

#include "Renderer/OpenGL/opengltexture.h"
#include "assetkey.h"

namespace FlexEngine
{
  namespace Asset
  {

    // Spritesheet asset
    // Uses the .flxspritesheet file format
    // TODO: use the flxfmt formatter
    // TODO: save any changes to file
    // TODO: generate a new spritesheet metadata file from a texture
    struct __FLX_API Spritesheet
    {
      File& metadata;
      AssetKey texture = "";
      int columns = 1;
      int rows = 1;

      Spritesheet(File& _metadata);

      // u1, v1 (top left)
      // u2, v2 (bottom right)
      using UV = Vector4;

      // get the uv coordinates of a sprite
      UV GetUV(int sprite_index = 0) const;
    };

  }
}