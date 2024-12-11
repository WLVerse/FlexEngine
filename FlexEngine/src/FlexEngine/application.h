// WLVERSE [https://wlverse.web.app]
// application.h
// 
// The architecture of the engine is designed to only have one application instance.
// This manages the windows and the main loop of the application.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "window.h"

#include <string>
#include <vector>
#include <memory> // std::shared_ptr

int main(int, char**);

namespace FlexEngine
{

  // The architecture of the engine is designed to only have one application instance.
  class __FLX_API Application
  {
    static bool m_is_closing;

    static bool m_is_running;
    static bool m_is_minimized;

    static Window* m_current_window;
    static std::vector<std::shared_ptr<Window>> m_windows;

  public:
    Application();
    virtual ~Application();

    // Closes the application and all windows
    // Important to note that this does not immediately close the application,
    // the application will close at the end of the current frame.
    static void Close();

    #pragma region Window Handling Functions

    static std::shared_ptr<Window> OpenWindow(const WindowProps& props = {});

    static void CloseWindow(const std::string& window_title);
    static void CloseWindow(std::shared_ptr<Window> window);

    #pragma endregion

    // Checks

    static bool IsRunning() { return m_is_running; }
    static bool IsMinimized() { return m_is_minimized; }

    // Returns the current window.
    // This is not the same as the focused window, the current window is
    // the window that is currently being processed.
    // Works somewhat like the opengl or imgui context.
    static Window* GetCurrentWindow();

    // Returns all the windows
    static const std::vector<std::shared_ptr<Window>>& GetWindows() { return m_windows; }

  private:

    // allow access to internal functions
    friend class Window;

    // INTERNAL FUNCTION
    // Sets the current window
    static void Internal_SetCurrentWindow(Window* window);

    // INTERNAL FUNCTION
    static void Internal_Close();

    static void Run();

    friend int ::main(int, char**);
  };

  Application* CreateApplication();
}
