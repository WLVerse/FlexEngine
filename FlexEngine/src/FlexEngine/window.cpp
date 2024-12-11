// WLVERSE [https://wlverse.web.app]
// window.cpp
// 
// This class is a wrapper around glfw window functions.
// Windows are managed by the application class.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "pch.h"

#include "window.h"

#include "application.h"

#include "imguiwrapper.h"

#include "input.h"
#include "Renderer/OpenGL/openglrenderer.h"
//#include "Renderer/OpenGL/openglframebuffer.h"
//#include "FMOD/FMODWrapper.h"

namespace
{

  // glfw: whenever the window size changed (by OS or user resize) this callback function executes
  #pragma warning(suppress : 4100) // unused parameters
  void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
  {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    //if (width > 0 && height > 0)
    //  FlexEngine::OpenGLFrameBuffer::RegenerateAllTextures(width, height);

    // update the window properties
    for (auto& win : FlexEngine::Application::GetWindows())
    {
      if (win->GetGLFWWindow() == window)
      {
        win->SetWidth(width);
        win->SetHeight(height);
        break;
      }
    }
  }

  void WindowFocusCallBack(GLFWwindow* window, int focused)
  {
    //FlexEngine::FMODWrapper::Core::WindowFocusCallback(window, focused);
    // Commented out as it causes tab down when clicking outside window
    /*if (focused)
    {
      glfwRestoreWindow(window);
    }
    else
    {
      glfwIconifyWindow(window);
    }*/
  }

}

namespace FlexEngine
{
  // Create a function to check whether 
  // Flag to indicate whether full screen
  Window::Window(WindowProps const& props)
    : s_props(props)
  {
    // window hints
    glfwDefaultWindowHints();
    for (auto& hint : s_props.window_hints)
    {
      glfwWindowHint(hint.first, hint.second);

    }

    // create window
    m_glfwwindow = glfwCreateWindow(s_props.width, s_props.height, s_props.title.c_str(), nullptr, nullptr);
    FLX_NULLPTR_ASSERT(m_glfwwindow, "Failed to create GLFW window");
    glfwMakeContextCurrent(m_glfwwindow);

    //glfwSetInputMode(m_glfwwindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // load all OpenGL function pointers (glad)
    FLX_CORE_ASSERT(gladLoadGL(), "Failed to initialize GLAD!");

    // set callbacks
    glfwSetKeyCallback(m_glfwwindow, Input::KeyCallback);
    glfwSetCursorPosCallback(m_glfwwindow, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(m_glfwwindow, Input::MouseButtonCallback);
    glfwSetScrollCallback(m_glfwwindow, Input::ScrollCallback);
    //glfwSetWindowSizeCallback(m_glfwwindow, WindowSizeCallback);
    glfwSetFramebufferSizeCallback(m_glfwwindow, FramebufferSizeCallback);
    //glfwSetWindowCloseCallback(m_glfwwindow, WindowCloseCallback);
    glfwSetWindowFocusCallback(m_glfwwindow, WindowFocusCallBack); // For now only audio requires this, but someone else should handle this centrally.
    //glfwSetCharCallback(m_glfwwindow, CharCallback);
    //glfwSetDropCallback(m_glfwwindow, DropCallback);
    #ifndef GAME
    // initialize imgui
    // this must be done after the window is created because imgui needs the OpenGL context
    // the shutdown is done in the window close function
    m_imguicontext = ImGuiWrapper::Init(this);
    #endif
    // always move the window to the center of the screen
    // this is done after the window is created to avoid the window being created off-center
    CenterWindow();
  }

  Window::~Window()
  {
    glfwDestroyWindow(m_glfwwindow);
  }

  #pragma region Getter/Setter Functions

  std::string Window::GetTitle() const { return s_props.title; }
  unsigned int Window::GetWidth() const { return s_props.width; }
  unsigned int Window::GetHeight() const { return s_props.height; }
  WindowProps Window::GetProps() const { return Props(); }
  WindowProps Window::Props() const { return s_props; }

  void Window::SetTitle(std::string const& title) { s_props.title = title; }
  void Window::SetWidth(unsigned int const& width) { s_props.width = width; }
  void Window::SetHeight(unsigned int const& height) { s_props.height = height; }

  #pragma endregion

  #pragma region Window Management Functions

  // This is where the main window loop is, ie Your game loop
  void Window::Update()
  {
    // make sure the current window is the one we are working with
    Application::Internal_SetCurrentWindow(this);
    glfwMakeContextCurrent(m_glfwwindow);
    #ifndef GAME
    ImGui::SetCurrentContext(m_imguicontext);
    #endif
    // clear screen
    OpenGLRenderer::ClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
    OpenGLRenderer::ClearFrameBuffer();

    m_frameratecontroller.BeginFrame();
    #ifndef GAME
    ImGuiWrapper::BeginFrame();
    #endif
    // update layer stack
    m_layerstack.Update();
    #ifndef GAME
    ImGuiWrapper::EndFrame();
    #endif
    m_frameratecontroller.EndFrame();

    // swap buffers
    glfwSwapBuffers(m_glfwwindow);
  }

  void Window::Close()
  {
    // remove all layers from the layer stack
    m_layerstack.Clear();
    #ifndef GAME
    // shutdown imgui
    // the imgui initialization is done in the window constructor
    ImGuiWrapper::Shutdown(m_imguicontext);
    #endif
    Application::Internal_SetCurrentWindow(nullptr);

    // set the window to close
    glfwSetWindowShouldClose(m_glfwwindow, true);
  }

  void Window::CenterWindow()
  {
    FLX_FLOW_FUNCTION();

    // get the primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor)
    {
      Log::Error("Failed to get primary monitor");
      return;
    }

    // get the video mode of the primary monitor
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode)
    {
      Log::Error("Failed to get video mode of primary monitor");
      return;
    }

    // get the size of the primary monitor
    int monitor_width, monitor_height;
    glfwGetMonitorPhysicalSize(monitor, &monitor_width, &monitor_height);

    // get the size of the window
    int window_width, window_height;
    glfwGetWindowSize(m_glfwwindow, &window_width, &window_height);

    // calculate the position of the window
    int x = (mode->width - window_width) / 2;
    int y = (mode->height - window_height) / 2;

    // set the position of the window
    SetWindowPosition(x, y);
  }

  #pragma endregion

  #pragma region Passthrough Functions

  void Window::GetWindowPosition(int* out_x, int* out_y) const
  {
    // get the position of the window
    glfwGetWindowPos(m_glfwwindow, out_x, out_y);
  }

  void Window::SetWindowPosition(int x, int y) const
  {
    // set the position of the window
    glfwSetWindowPos(m_glfwwindow, x, y);
  }

  bool Window::IsFocused() const
  {
    return glfwGetWindowAttrib(m_glfwwindow, GLFW_FOCUSED);
  }

  bool Window::IsFullScreen()
  {
    return glfwGetWindowMonitor(GetGLFWWindow()) != nullptr;
  }

  void Window::CacheMiniWindowParams() {
    int xpos = 0, ypos = 0;
    c_params.cached_mini_window_width = s_props.width;
    c_params.cached_mini_window_height = s_props.height;

    // get the position of the window
    glfwGetWindowPos(m_glfwwindow, &xpos, &ypos);
    c_params.cached_mini_window_xpos = xpos;
    c_params.cached_mini_window_ypos = ypos;
  }

  std::pair<int, int> Window::UnCacheMiniWindowsParams() {
    s_props.width = c_params.cached_mini_window_width;
    s_props.height = c_params.cached_mini_window_height;
    return std::make_pair(c_params.cached_mini_window_xpos, c_params.cached_mini_window_ypos);
  }

  void Window::SetIcon(const Asset::Texture& icon) const
  {
    FLX_FLOW_FUNCTION();

    // create the image
    GLFWimage image;
    image.width = icon.GetWidth();
    image.height = icon.GetHeight();
    image.pixels = icon.GetTextureData();

    // set the icon
    glfwSetWindowIcon(m_glfwwindow, 1, &image);
  }

  void Window::SetVSync(bool enabled) const
  {
    glfwSwapInterval(enabled ? 1 : 0);
  }

  #pragma endregion

}
