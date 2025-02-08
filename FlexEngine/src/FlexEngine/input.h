// WLVERSE [https://wlverse.web.app]
// input.h
//
// This static class is a wrapper to manage the results of the glfw input
// callback. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "FlexMath/vector2.h"

#include <GLFW/glfw3.h>
//#include "glm.hpp"

#include <array>
#include <algorithm>

// GLFW key codes
// https://www.glfw.org/docs/3.3/group__keys.html

namespace FlexEngine
{

  class __FLX_API Input
  {
  // public functions
  public:
    static void Cleanup(); // cleans the input state, should be called once per frame at the end

    static bool GetKeyDown(int key)             { return m_key_down[key]; }
    static bool GetKeyUp(int key)               { return m_key_up[key]; }
    static bool GetKey(int key)                 { return m_key[key]; }
    static bool AnyKeyDown()                    { return std::any_of(m_key_down.begin(), m_key_down.end(), [](bool b) { return b; }); }
    static bool AnyKeyUp()                      { return std::any_of(m_key_up.begin(), m_key_up.end(), [](bool b) { return b; }); }
    static bool AnyKey()                        { return std::any_of(m_key.begin(), m_key.end(), [](bool b) { return b; }); }

    static Vector2 GetCursorPosition()          { return m_cursor_position; }
    static Vector2 GetCursorPositionDelta()     { return m_cursor_position_delta; }

    // Exceedingly illegal, but this is the only way to work with editor input. If you attempt to use this for any purpose other than this, it's a skill issue.
    static Vector2 SetCursorPosition(Vector2 pos) { return m_cursor_position = pos; } 

    // renaming for convenience
    static Vector2 GetMousePosition()           { return GetCursorPosition(); }
    static Vector2 GetMousePositionDelta()      { return GetCursorPositionDelta(); }

    static bool GetMouseButtonDown(int button)  { return m_mouse_button_down[button]; }
    static bool GetMouseButtonUp(int button)    { return m_mouse_button_up[button]; }
    static bool GetMouseButton(int button)      { return m_mouse_button[button]; }
    static bool AnyMouseButtonDown()            { return std::any_of(m_mouse_button_down.begin(), m_mouse_button_down.end(), [](bool b) { return b; }); }
    static bool AnyMouseButtonUp()              { return std::any_of(m_mouse_button_up.begin(), m_mouse_button_up.end(), [](bool b) { return b; }); }
    static bool AnyMouseButton()                { return std::any_of(m_mouse_button.begin(), m_mouse_button.end(), [](bool b) { return b; }); }

  // member variables
  private:
    static std::array<bool, GLFW_KEY_LAST>          m_key_down;
    static std::array<bool, GLFW_KEY_LAST>          m_key_up;
    static std::array<bool, GLFW_KEY_LAST>          m_key;

    static Vector2                                  m_cursor_position;
    static Vector2                                  m_last_cursor_position;
    static Vector2                                  m_cursor_position_delta;

    static std::array<bool, GLFW_MOUSE_BUTTON_LAST> m_mouse_button_down;
    static std::array<bool, GLFW_MOUSE_BUTTON_LAST> m_mouse_button_up;
    static std::array<bool, GLFW_MOUSE_BUTTON_LAST> m_mouse_button;

    static Vector2                                  m_scroll_offset;

  // callback functions for glfw
  public:
    static void KeyCallback             (GLFWwindow* window, int key, int scancode, int action, int mods);
    static void CursorPositionCallback  (GLFWwindow* window, double xpos, double ypos);
    static void MouseButtonCallback     (GLFWwindow* window, int button, int action, int mods);
    static void ScrollCallback          (GLFWwindow* window, double xoffset, double yoffset);

  // prevent instantiation
  public:
    Input() = delete;
    Input(const Input&) = delete;
    Input(Input&&) = delete;
    Input& operator=(const Input&) = delete;
    Input& operator=(Input&&) = delete;
  };

}