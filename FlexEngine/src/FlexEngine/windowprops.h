// WLVERSE [https://wlverse.web.app]
// windowprops.h
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
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h> // always put glad before glfw

#include <string>
#include <vector>

namespace FlexEngine
{

  // Helper macro for default window hints.
  // https://www.glfw.org/docs/3.0/window.html#window_hints
  #define FLX_DEFAULT_WINDOW_HINTS \
    { GLFW_CONTEXT_VERSION_MAJOR, 4 }, \
    { GLFW_CONTEXT_VERSION_MINOR, 6 }, \
    { GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE }, \
    { GLFW_DOUBLEBUFFER, GLFW_TRUE }, \
    { GLFW_DEPTH_BITS, 24 }, \
    { GLFW_RED_BITS, 8 }, \
    { GLFW_GREEN_BITS, 8 }, \
    { GLFW_BLUE_BITS, 8 }, \
    { GLFW_ALPHA_BITS, 8 }

  struct __FLX_API WindowProps
  {
    std::string title = "FlexEngine";
    int width = 1280;
    int height = 720;
    const std::vector<std::pair<int, int>> window_hints { FLX_DEFAULT_WINDOW_HINTS };
    const char* opengl_version_text = "#version 460 core";

    WindowProps(
      const std::string& title = "FlexEngine",
      int width = 1280, int height = 720,
      std::vector<std::pair<int, int>> window_hints = {
        FLX_DEFAULT_WINDOW_HINTS
      },
      const char* opengl_version_text = "#version 460 core"
    );
  };

}