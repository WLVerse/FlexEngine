// WLVERSE [https://wlverse.web.app]
// windowprops.cpp
// 
// Struct to store window properties.
// Pass in window hints as a vector of pairs of integers.
// Window hints cannot be changed after the window is created.
// https://www.glfw.org/docs/3.0/window.html#window_hints
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "pch.h"

#include "window.h"

namespace FlexEngine
{

  WindowProps::WindowProps(
    const std::string& title,
    int width, int height,
    std::vector<std::pair<int, int>> window_hints,
    const char* opengl_version_text
  )
    : title(title)
    , width(width), height(height)
    , window_hints(window_hints)
    , opengl_version_text(opengl_version_text)
  {
  }

}