// WLVERSE [https://wlverse.web.app]
// input.cpp
//
// This static class is a wrapper to manage the results of the glfw input
// callback. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "input.h"

namespace FlexEngine
{
  #pragma region Define Static Variables

  std::array<bool, GLFW_KEY_LAST>           Input::m_key_down{};
  std::array<bool, GLFW_KEY_LAST>           Input::m_key_up{};
  std::array<bool, GLFW_KEY_LAST>           Input::m_key{};
  Vector2                                   Input::m_cursor_position{};
  Vector2                                   Input::m_last_cursor_position{};
  Vector2                                   Input::m_cursor_position_delta{};
  std::array<bool, GLFW_MOUSE_BUTTON_LAST>  Input::m_mouse_button_down{};
  std::array<bool, GLFW_MOUSE_BUTTON_LAST>  Input::m_mouse_button_up{};
  std::array<bool, GLFW_MOUSE_BUTTON_LAST>  Input::m_mouse_button{};
  Vector2                                   Input::m_scroll_offset{};

  std::array<bool, GLFW_GAMEPAD_BUTTON_LAST + 1> Input::m_gamepad_prev_buttons = { false };
  bool                                           Input::m_gamepad_connected = { false };

  #pragma endregion

  void Input::UpdateGamepadInput()
  {
    bool connected = glfwJoystickIsGamepad(GLFW_JOYSTICK_1);
    if (connected) 
    {
      GLFWgamepadstate state;
      if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) 
      {
        for (int i = 0; i <= GLFW_GAMEPAD_BUTTON_LAST; i++) 
        {
          bool is_down = (state.buttons[i] == GLFW_PRESS);
          bool is_pressed = is_down && !m_gamepad_prev_buttons[i];
          bool is_up = !is_down && m_gamepad_prev_buttons[i];

          m_gamepad_prev_buttons[i] = is_down;

          //Hijack time
          if (i == GLFW_GAMEPAD_BUTTON_A) 
          {
            if (is_down)    m_key[GLFW_KEY_SPACE] = true;
            if (is_pressed) m_key_down[GLFW_KEY_SPACE] = true;
            if (is_up)
            {
              m_key_up[GLFW_KEY_SPACE] = true;
              m_key[GLFW_KEY_SPACE] = false;
            }

          }
          else if (i == GLFW_GAMEPAD_BUTTON_DPAD_UP)
          {
            if (is_down)    m_key[GLFW_KEY_W] = true;
            if (is_pressed) m_key_down[GLFW_KEY_W] = true;
            if (is_up)
            {
              m_key_up[GLFW_KEY_W] = true;
              m_key[GLFW_KEY_W] = false;
            }
          }
          else if (i == GLFW_GAMEPAD_BUTTON_DPAD_DOWN)
          {
            if (is_down)    m_key[GLFW_KEY_S] = true;
            if (is_pressed) m_key_down[GLFW_KEY_S] = true;
            if (is_up)
            {
              m_key_up[GLFW_KEY_S] = true;
              m_key[GLFW_KEY_S] = false;
            }
          }
          else if (i == GLFW_GAMEPAD_BUTTON_DPAD_LEFT)
          {
            if (is_down)
            {
              m_key[GLFW_KEY_A] = true;
            }
            if (is_pressed) m_key_down[GLFW_KEY_A] = true;
            if (is_up)
            {
              m_key_up[GLFW_KEY_A] = true;
              m_key[GLFW_KEY_A] = false;
            }
          }
          else if (i == GLFW_GAMEPAD_BUTTON_DPAD_RIGHT)
          {
            if (is_down)    m_key[GLFW_KEY_D] = true;
            if (is_pressed) m_key_down[GLFW_KEY_D] = true;
            if (is_up)
            {
              m_key_up[GLFW_KEY_D] = true;
              m_key[GLFW_KEY_D] = false;
            }
          }
          else if (i == GLFW_GAMEPAD_BUTTON_START)
          {
            if (is_down)    m_key[GLFW_KEY_ESCAPE] = true;
            if (is_pressed) m_key_down[GLFW_KEY_ESCAPE] = true;
            if (is_up)
            {
              m_key_up[GLFW_KEY_ESCAPE] = true;
              m_key[GLFW_KEY_ESCAPE] = false;
            }
          }
        }
      }
    }
    if (!connected && m_gamepad_connected)
    {
      //gamepad disconnected
      //Reset everything to prevent problems on dis/re/connection
      m_gamepad_prev_buttons.fill(false);
      m_key[GLFW_KEY_ESCAPE] = false;
      m_key[GLFW_KEY_W] = false;
      m_key[GLFW_KEY_A] = false;
      m_key[GLFW_KEY_S] = false;
      m_key[GLFW_KEY_D] = false;
      m_key[GLFW_KEY_SPACE] = false;
    }
    m_gamepad_connected = connected;
  }

  void Input::Cleanup()
  {
    m_key_down.fill(false);
    m_key_up.fill(false);
    m_mouse_button_down.fill(false);
    m_mouse_button_up.fill(false);
    m_scroll_offset = Vector2::Zero;

    m_cursor_position_delta = Vector2::Zero;
    m_cursor_position_delta = m_cursor_position - m_last_cursor_position;
    m_last_cursor_position = m_cursor_position;
  }

  #pragma warning(push) // C4100 unused parameters
  #pragma warning(disable:4100) // push warning settings as opposed to using the UNREFERENCED_PARAMETER macro

  void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    //m_key_down.fill(false);
    //m_key_up.fill(false);
    if (key == -1)
    {
      Log::Info("Unsupported key was inputted. If this key is needed, please inform someone!!");
      return;
    }

    switch (action)
    {
    case GLFW_PRESS:
      m_key_down[key] = true;
      m_key[key] = true;
      break;
    case GLFW_RELEASE:
      m_key_up[key] = true;
      m_key[key] = false;
      break;
    default:
      break;
    }
  }

  void Input::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
  {
    //m_last_cursor_position = m_cursor_position;
    m_cursor_position = { static_cast<float>(xpos), static_cast<float>(ypos) };
    //m_cursor_position_delta = m_cursor_position - m_last_cursor_position;
  }

  void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
  {
    //m_mouse_button_down.fill(false);
    //m_mouse_button_up.fill(false);

    switch (action)
    {
    case GLFW_PRESS:
      m_mouse_button_down[button] = true;
      m_mouse_button[button] = true;
      break;
    case GLFW_RELEASE:
      m_mouse_button_up[button] = true;
      m_mouse_button[button] = false;
      break;
    default:
      break;
    }
  }

  void Input::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
  {
    m_scroll_offset = { static_cast<float>(xoffset), static_cast<float>(yoffset) };
  }

  #pragma warning(pop) // C4100 unused parameters

}