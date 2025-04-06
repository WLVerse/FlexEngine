// WLVERSE [https://wlverse.web.app]
// window.cpp
//
// This class is mostly a data structure for window properties. 
// It helps to manage the window properties and the window itself, wrapping around
// GLFW window functions. 
// 
// Windows are managed by the application class. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "window.h"

#include "application.h"
#include "assetdropmanager.h"

#ifndef IMGUI_DISABLE
#include "imguiwrapper.h"
#endif

#include "input.h"
#include "Renderer/OpenGL/openglrenderer.h"

#include "flexprefs.h"  

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32  //to expose glfwGetWin32Window
#include <GLFW/glfw3native.h>
#endif

//#include "FMOD/FMODWrapper.h"

FlexEngine::OpenGLFrameBufferManager FlexEngine::Window::FrameBufferManager;

namespace
{
  // glfw: whenever the window size changed (by OS or user resize) this callback function executes
  void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
  {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);

    // This is needed to follow with how IMGUI decides to draw images (which is using the glViewport values)
    if (width > 0 && height > 0) // If we tab down, framebuffersizecallback returns size 0, which will absolutely destroy our rendering as the framebuffer width/height is now 0
      FlexEngine::Window::FrameBufferManager.ResizeAllFramebuffers(static_cast<float>(width), static_cast<float>(height));

    // update the window properties
    for (auto& [window_name, win] : FlexEngine::Application::GetWindowRegistry())
    {
      if (win->GetGLFWWindow() == window)
      {
        win->SetWidth(width);
        win->SetHeight(height);
        break;
      }
    }
  }

  void WindowFocusCallBack(GLFWwindow*, int)
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

  void WindowCloseCallback([[maybe_unused]] GLFWwindow* window)
  {
    FlexEngine::Application::QueueCommand(FlexEngine::Application::Command::QuitApplication);
  }

}

namespace FlexEngine
{
  // static member initialization
  const WindowProps WindowProps::Null = WindowProps(
    "Null",
    0, 0,
    {},
    ""
  );

  #pragma region Managed Stateful RAII Pattern

  void Window::Open()
  {
    FLX_FLOW_FUNCTION();

    FLX_CORE_ASSERT(!is_init, "Internal_Open is being called on an already initialized window.");

    // window hints
    glfwDefaultWindowHints();
    for (auto& [hint, value] : m_props.window_hints)
    {
      glfwWindowHint(hint, value);
    }

    // Fullscreen overrides any option
    bool should_fs = false;
    #ifdef GAME
    should_fs = FlexPrefs::GetBool("game.fullscreen");
    #else
    should_fs = FlexPrefs::GetBool("editor.fullscreen");
    #endif

    if (should_fs)
    {
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);

      m_glfwwindow = glfwCreateWindow(mode->width, mode->height, m_props.title.c_str(), monitor, nullptr);
      m_props.width = mode->width;
      m_props.height = mode->height;
      m_is_full_screen = true;
    }
    else
    {
      m_props.width = 1600.f;
      m_props.height = 900.f;
      m_glfwwindow = glfwCreateWindow(m_props.width, m_props.height, m_props.title.c_str(), nullptr, nullptr);
      m_is_full_screen = false;
    }

    FLX_NULLPTR_ASSERT(m_glfwwindow, "Failed to create GLFW window");
    glfwMakeContextCurrent(m_glfwwindow);
    glfwSwapInterval(FlexPrefs::GetBool("game.vsync") ? 1 : 0);

    // load all OpenGL function pointers (glad)
    FLX_CORE_ASSERT(gladLoadGL(), "Failed to initialize GLAD!");

    // set callbacks
    glfwSetKeyCallback(m_glfwwindow, Input::KeyCallback);
    glfwSetCursorPosCallback(m_glfwwindow, Input::CursorPositionCallback);
    glfwSetMouseButtonCallback(m_glfwwindow, Input::MouseButtonCallback);
    glfwSetScrollCallback(m_glfwwindow, Input::ScrollCallback);
    //glfwSetWindowSizeCallback(m_glfwwindow, WindowSizeCallback);
    glfwSetFramebufferSizeCallback(m_glfwwindow, FramebufferSizeCallback);
    glfwSetWindowCloseCallback(m_glfwwindow, WindowCloseCallback);
    glfwSetWindowFocusCallback(m_glfwwindow, WindowFocusCallBack); // For now only audio requires this, but someone else should handle this centrally.
    //glfwSetCharCallback(m_glfwwindow, CharCallback);
    //glfwSetDropCallback(m_glfwwindow, DropCallback);

    //Register this window as a drag drop target
    HRESULT result = OleInitialize(NULL);

    if (!SUCCEEDED(result))
    {
      Log::Error("OleInitialize failed!");
    }
    m_dropmanager.Initialize(glfwGetWin32Window(m_glfwwindow));


    result = RegisterDragDrop(m_dropmanager.m_hwnd, &m_dropmanager);
    if (!SUCCEEDED(result))
    {
      Log::Error("RegisterDragDrop failed!");
    }




    // functions below require is_init to be true
    is_init = true;

    #ifndef IMGUI_DISABLE
    // initialize imgui
    // this must be done after the window is created because imgui needs the OpenGL context
    // the shutdown is done in the window close function
    m_imgui_context = ImGuiWrapper::Init(this);
    #endif

    // always move the window to the center of the screen
    // this is done after the window is created to avoid the window being created off-center
    CenterWindow();

    FrameBufferManager.Init();
  }

  void Window::Close()
  {
    FLX_FLOW_FUNCTION();

    FLX_WINDOW_ISOPEN_ASSERT;

    #ifndef IMGUI_DISABLE
    // shutdown imgui
    // the imgui initialization is done in the window constructor
    ImGuiWrapper::Shutdown(this);
    #endif

    Application::Internal_SetCurrentWindow(nullptr);
    
    OleUninitialize();

    glfwDestroyWindow(m_glfwwindow);
    m_glfwwindow = nullptr;

    is_init = false;
  }

  #pragma endregion

  #pragma region Window Management Functions

  void Window::Update()
  {
    FLX_WINDOW_ISOPEN_ASSERT;

    // make sure the current window is the one we are working with
    SetCurrentContext();

    // clear screen
    OpenGLRenderer::ClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
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

  void Window::SetIcon(const Asset::Texture& icon) const
  {
    FLX_FLOW_FUNCTION();

    FLX_WINDOW_ISOPEN_ASSERT;

    // create the image
    GLFWimage image{};
    image.width = icon.GetWidth();
    image.height = icon.GetHeight();
    image.pixels = icon.GetTextureData();

    // set the icon
    glfwSetWindowIcon(m_glfwwindow, 1, &image);
  }

  void Window::CenterWindow()
  {
    FLX_FLOW_FUNCTION();

    FLX_WINDOW_ISOPEN_ASSERT;

    // get the primary monitor
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    FLX_NULLPTR_ASSERT(monitor, "Failed to get primary monitor while trying to center the window.");

    // get the video mode of the primary monitor
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    FLX_NULLPTR_ASSERT(mode, "Failed to get video mode of the primary monitor while trying to center the window.");

    // get the size of the primary monitor
    int monitor_width, monitor_height;
    glfwGetMonitorPhysicalSize(monitor, &monitor_width, &monitor_height);

    // get the size of the window
    int window_width, window_height;
    glfwGetWindowSize(m_glfwwindow, &window_width, &window_height);

    #pragma warning(push)
    #pragma warning(disable : 6011) // dereferencing null pointer

    // calculate the centered position of the window
    int x = (mode->width - window_width) / 2;
    int y = (mode->height - window_height) / 2;

    #pragma warning(pop)

    // set the position of the window
    glfwSetWindowPos(m_glfwwindow, x, y);

    // update the cached mini window params
    CacheMiniWindowParams();
  }

  void Window::SetCurrentContext()
  {
    Application::Internal_SetCurrentWindow(this);

    // glfw and imgui context are not ready yet
    if (!is_init) return;

    FLX_GLFW_ALIGNCONTEXT();

    #ifndef IMGUI_DISABLE
    FLX_IMGUI_ALIGNCONTEXT();
    #endif
  }

  #pragma endregion

  #pragma region Cached Mini Window Params

  void Window::CacheMiniWindowParams()
  {
    FLX_WINDOW_ISOPEN_ASSERT;

    int xpos = 0, ypos = 0;
    m_params.cached_mini_window_width = m_props.width;
    m_params.cached_mini_window_height = m_props.height;

    // get the position of the window
    glfwGetWindowPos(m_glfwwindow, &xpos, &ypos);
    m_params.cached_mini_window_xpos = xpos;
    m_params.cached_mini_window_ypos = ypos;
  }

  std::pair<int, int> Window::UnCacheMiniWindowsParams()
  {
    FLX_WINDOW_ISOPEN_ASSERT;

    m_props.width = m_params.cached_mini_window_width;
    m_props.height = m_params.cached_mini_window_height;
    return std::make_pair(m_params.cached_mini_window_xpos, m_params.cached_mini_window_ypos);
  }

  #pragma endregion

  void Window::ToggleFullScreen(bool fs)
  {
    // Toggle fullscreen
    if (fs)
    {
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);

      glfwSetWindowMonitor(m_glfwwindow, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
      m_is_full_screen = true;
    }
    else
    {
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);

      // Just toggle windowed to 1600x900
      // TODO: all options are: "1920x1080", "1600x900", "1366x768", "1280x720"
      m_props.width = 1600.f;
      m_props.height = 900.f;

      glfwSetWindowMonitor(m_glfwwindow, nullptr, 0, 0, m_props.width, m_props.height, 0);
      glfwSetWindowPos(m_glfwwindow, mode->width / 2 - m_props.width/2, mode->height/2 - m_props.height/2); // Center to the screen
      m_is_full_screen = false;
    }

    // Set the preference
    FlexPrefs::SetBool("game.fullscreen", fs);
    FlexPrefs::Save();
  }
}