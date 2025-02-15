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
      FLX_ASSERT(
        (ss >> texture >> columns >> rows),
        "Spritesheet metadata file is corrupted. Texture path, columns, and rows are required. " +
          metadata.path.string()
      );
      FLX_ASSERT(
        columns > 0 && rows > 0,
        "Spritesheet metadata file is corrupted. Columns and rows must be greater than 0. " + metadata.path.string()
      );

      // make sure that there are col * rows amount of lines after the first line
      // this is the frame times
      int total_frames = columns * rows;
      frame_times.clear();
      frame_times.reserve(total_frames + 1);
      total_frame_time = 0.0f;

      for (int i = 0; i < total_frames; ++i)
      {
        float frame_time = 0.0f;
        if (!(ss >> frame_time))
        {
          Log::Fatal(
            "Spritesheet metadata file is corrupted. Missing frame time at frame " + std::to_string(i) +
            ". Expected " + std::to_string(total_frames) + " frames. " + metadata.path.string()
          );
        }
        frame_times.push_back(frame_time);
        total_frame_time += frame_time;
      }

      // make sure that there are the correct amount of frame times
      FLX_ASSERT(
        frame_times.size() == total_frames,
        "Spritesheet metadata file is corrupted. Incorrect number of frame times. Got " +
          std::to_string(frame_times.size()) + ", expected " + std::to_string(total_frames) + ". " +
          metadata.path.string()
      );

      // make sure that the total frame time is not 0
      FLX_ASSERT(
        total_frame_time > 0.0f,
        "Spritesheet metadata file is corrupted. Total frame time is 0. " + metadata.path.string()
      );

      // ignore the rest of the file
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
      return UV(u1, v1, u2, v2);
      //return UV(u1, v2, u2, v1); // flip y
    }

  }

}