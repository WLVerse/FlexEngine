#pragma once

#include "flx_api.h"

#include "FlexEngine/Core/frameratecontroller.h" // <chrono>
#include "FlexEngine/Core/layerstack.h" // <string> <memory> <vector>
#include "Renderer/OpenGL/opengltexture.h" // Asset::Texture

#include <glad/glad.h>
#include <GLFW/glfw3.h> // always put glad before glfw

#include <imgui.h> // ImGuiContext

#include <string>
#include <vector>
#include <memory> // std::shared_ptr

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

  // Window properties.
  // Pass in window hints as a vector of pairs of integers.
  // Window hints cannot be changed after the window is created.
  // https://www.glfw.org/docs/3.0/window.html#window_hints
  struct __FLX_API WindowProps
  {
    std::string title = "FlexEngine";
    int width = 1280;
    int height = 720;
    const std::vector<std::pair<int, int>> window_hints{};
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

  class __FLX_API Window
  {
    WindowProps s_props;

    FramerateController m_frameratecontroller;
    LayerStack m_layerstack;

    GLFWwindow* m_glfwwindow{ nullptr };
    ImGuiContext* m_imguicontext{ nullptr };

  public:
    Window(const WindowProps& props);
    ~Window();

    // delete default, copy and move constructors, and assign operators
    Window() = delete;
    Window(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&) = delete;

    #pragma region Getter/Setter Functions

    std::string GetTitle() const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;
    WindowProps GetProps() const; // alias for convention
    WindowProps Props() const;

    void SetTitle(std::string const& title);
    void SetWidth(unsigned int const& width);
    void SetHeight(unsigned int const& height);

    #pragma endregion

    #pragma region Window Management Functions

    // Sets the current window as the active window
    // Clears the screen and runs the layer stack
    void Update();

    // Closes the window
    void Close();

    // Moves the window to the center of the screen
    // Do not call this function unnecessarily
    void CenterWindow();

    #pragma endregion

    #pragma region Passthrough Functions

    // passthrough functions for glfw

    void GetWindowPosition(int* out_x, int* out_y) const;
    void SetWindowPosition(int x, int y) const;

    bool IsFocused() const;

    void SetIcon(const Asset::Texture& icon) const;

    void SetVSync(bool enabled = 0) const;

    // passthrough functions for the framerate controller

    float GetDeltaTime() const { return m_frameratecontroller.GetDeltaTime(); }
    unsigned int GetFPS() const { return m_frameratecontroller.GetFPS(); }

    void SetTargetFPS(unsigned int fps = 0) { m_frameratecontroller.SetTargetFPS(fps); }

    // passthrough functions for the layer stack

    void PushLayer(std::shared_ptr<Layer> layer) { m_layerstack.PushLayer(layer); }
    void PushOverlay(std::shared_ptr<Layer> layer) { m_layerstack.PushOverlay(layer); }
    void PopLayer() { m_layerstack.PopLayer(); }
    void PopOverlay() { m_layerstack.PopOverlay(); }

    #pragma endregion

    // Returns the GLFW window pointer.
    // Do not use this unless you know what you are doing.
    // The function is unsafe because it returns a raw pointer.
    GLFWwindow* GetGLFWWindow() const { return m_glfwwindow; }

    // Returns the ImGui context pointer.
    // Do not use this unless you know what you are doing.
    // The function is unsafe because it returns a raw pointer.
    ImGuiContext* GetImGuiContext() const { return m_imguicontext; }

  };

}