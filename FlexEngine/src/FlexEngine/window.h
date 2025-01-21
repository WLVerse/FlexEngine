// WLVERSE [https://wlverse.web.app]
// window.h
// 
// This class is mostly a data structure for window properties.
// It helps to manage the window properties and the window itself,
// wrapping around GLFW window functions.
// 
// Windows are managed by the application class.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "frameratecontroller.h" // <chrono>
#include "Layer/layerstack.h" // <string> <memory> <vector>
#include "DataStructures/functionqueue.h" // FunctionQueue
#include "Renderer/OpenGL/opengltexture.h" // Asset::Texture

#include <glad/glad.h>
#include <GLFW/glfw3.h> // always put glad before glfw

#ifndef IMGUI_DISABLE
#include <imgui.h> // ImGuiContext
#endif

#include <string>
#include <vector>
#include <memory> // std::shared_ptr

namespace FlexEngine
{

  #pragma region WindowProps

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

  // These are the properties that can be set for a window on creation.
  // Pass in window hints as a vector of pairs of integers.
  // Window hints cannot be changed after the window is created.
  // https://www.glfw.org/docs/3.0/window.html#window_hints
  struct __FLX_API WindowProps
  {
    using WindowHint = std::pair<int, int>;

    std::string title = "FlexEngine";
    int width = 1280;
    int height = 720;
    const std::vector<WindowHint> window_hints = {
      FLX_DEFAULT_WINDOW_HINTS
    };
    const char* opengl_version_text = "#version 460 core";

    // Null for testing purposes
    static const WindowProps Null;

    WindowProps() = default;
    WindowProps(
      const std::string& _title,
      int _width, int _height,
      const std::vector<WindowHint>& _window_hints = {
        FLX_DEFAULT_WINDOW_HINTS
      },
      const char* _opengl_version_text = "#version 460 core"
    ) : title(_title)
      , width(_width), height(_height)
      , window_hints(_window_hints)
      , opengl_version_text(_opengl_version_text)
    {
    }

    // comparison
    bool operator==(const WindowProps& other) const
    {
      return title == other.title
        && width == other.width
        && height == other.height
        && window_hints == other.window_hints
        && opengl_version_text == other.opengl_version_text;
    }
    bool operator!=(const WindowProps& other) const
    {
      return !(*this == other);
    }

    // bool for quick comparisons in if statements
    operator bool() const
    {
      return *this != Null;
    }
  };

  #pragma endregion

  #pragma region Cached Mini Window Params

  struct CachedMiniWindowParams
  {
    int cached_mini_window_width = 1280;
    int cached_mini_window_height = 720;
    int cached_mini_window_xpos = 0;
    int cached_mini_window_ypos = 0;
  };

  #pragma endregion

  class __FLX_API Window
  {
    #pragma region Managed Stateful RAII Pattern

    // This is a managed stateful RAII class.
    // It must be init and shutdown explicitly and all functions use
    // the FLX_WINDOW_ISOPEN_ASSERT macro to check if the window is init.

  private:
    bool is_init = false;

  public:
    // Returns true if the window is open.
    bool IsInit() const { return is_init; }

    // macro guard for functions that require the window to be open
    #define FLX_WINDOW_ISOPEN_ASSERT assert(is_init)
    //FLX_CORE_ASSERT(is_init, "The function [" + std::string(__FUNCTION__) + "] cannot be run if the window is not initialized.")

  private:
    bool is_closing = false;

  public:
    // Returns true if the window is closing.
    bool IsClosing() const { return is_closing; }

    void Open();
    void Close();

  public:
    Window(const std::string& window_name, const WindowProps& props)
      : name(window_name), m_props(props)
    {
    }
    ~Window() { if (is_init) Close(); }

    #pragma endregion

    #pragma region Getter/Setter Functions

  private:
    // Window name identifier for the window registry.
    // This should always be the same as the key in the window registry.
    const std::string name;

    // Window properties
    WindowProps m_props = {};

  public:
    std::string GetName() const { return name; }

    std::string GetTitle() const { return m_props.title; }
    unsigned int GetWidth() const { return m_props.width; }
    unsigned int GetHeight() const { return m_props.height; }
    WindowProps Props() const { return m_props; }
    WindowProps GetProps() const { return Props(); }

    void SetTitle(const std::string& title) { m_props.title = title; }
    void SetWidth(const unsigned int& width) { m_props.width = width; }
    void SetHeight(const unsigned int& height) { m_props.height = height; }

    #pragma endregion

    #pragma region Window Management Functions

  public:
    // Sets the current window as the active window
    // Clears the screen and runs the layer stack
    void Update();

    // Sets the icon of the window
    // Does not require the asset manager
    void SetIcon(const Asset::Texture& icon) const;

    // Moves the window to the center of the screen
    // Do not call this function unnecessarily
    void CenterWindow();

    // Sets the current window context for the application, glfw, and imgui
    // This is called automatically at the start of the window update function and
    // after a new window is created. You can also call this function manually.
    void SetCurrentContext();

    #pragma endregion

    #pragma region Composition

    // Uses the Composition over Inheritance principle.

    FramerateController m_frameratecontroller;
    FramerateController& GetFramerateController() { return m_frameratecontroller; }

    LayerStack m_layerstack;
    LayerStack& GetLayerStack() { return m_layerstack; }

    #pragma endregion

    #pragma region Cached Mini Window Params

    CachedMiniWindowParams m_params;

    // This is to record parameters of window b4 full screen
    // Do not call this function unnecessarily (there really shouldn't be any need to)
    void CacheMiniWindowParams();

    // This is to set the window parameters upon full screen exit
    // Do not call this function unnecessarily (there really shouldn't be any need to)
    std::pair<int, int> UnCacheMiniWindowsParams();

    #pragma endregion

    #pragma region Context Pointers

    GLFWwindow* m_glfwwindow = nullptr;

    // Aligns the context pointer.
    // 
    // ImGui is not recommended to be used in DLLs because of the global context pointer.
    // We must call this function before using ImGui across any DLL boundaries.
    // 
    // !!IMPORTANT!! This cannot be a function. It MUST be a macro because we want
    // ImGui::SetCurrentContext to actually be called wherever this macro is used.
    // If it's a function, it will only set the context for the function's boundary,
    // which is always in this DLL.
    #define FLX_GLFW_ALIGNCONTEXT() glfwMakeContextCurrent(Application::GetCurrentWindow()->GetGLFWWindow())

    // Returns the GLFW window pointer.
    // Do not use this unless you know what you are doing.
    // The function is unsafe because it returns a raw pointer.
    GLFWwindow* GetGLFWWindow() const { FLX_WINDOW_ISOPEN_ASSERT; return m_glfwwindow; }

    #ifndef IMGUI_DISABLE

  private:
    ImGuiContext* m_imgui_context = nullptr;

  public:
    // Aligns the context pointer.
    // 
    // ImGui is not recommended to be used in DLLs because of the global context pointer.
    // We must call this function before using ImGui across any DLL boundaries.
    // 
    // !!IMPORTANT!! This cannot be a function. It MUST be a macro because we want
    // ImGui::SetCurrentContext to actually be called wherever this macro is used.
    // If it's a function, it will only set the context for the function's boundary,
    // which is always in this DLL.
    #define FLX_IMGUI_ALIGNCONTEXT() ImGui::SetCurrentContext(Application::GetCurrentWindow()->GetImGuiContext())

    ImGuiContext* GetImGuiContext() { return m_imgui_context; }

    #endif

    #pragma endregion

  };

}