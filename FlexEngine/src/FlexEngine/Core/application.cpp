#include "pch.h"

#include "application.h"

#include "StateManager/statemanager.h"
#include "input.h"

namespace FlexEngine
{
  // static member initialization
  bool Application::m_is_closing = false;
  bool Application::m_is_running = true;
  bool Application::m_is_minimized = false;
  std::vector<std::shared_ptr<Window>> Application::m_windows;
  Window* Application::m_current_window = nullptr;

  Application::Application()
  {
    FLX_FLOW_BEGINSCOPE();

    // initialize glfw
    FLX_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW!");
  }

  Application::~Application()
  {
    glfwMakeContextCurrent(NULL);
    glfwTerminate();

    FLX_FLOW_ENDSCOPE();
  }

  void Application::Close()
  {
    m_is_closing = true;
  }

  void Application::Internal_Close()
  {
    FLX_FLOW_FUNCTION();

    m_current_window = nullptr;

    // close all windows
    // done this way because the windows vector is modified in the loop
    size_t windows_to_close = m_windows.size();
    for (size_t i = 0; i < windows_to_close; i++) CloseWindow(m_windows[0]);

    // make sure windows vector is empty
    m_windows.clear();

    m_is_running = false;
  }

  #pragma region Window Handling Functions

  std::shared_ptr<Window> Application::OpenWindow(const WindowProps& props)
  {
    FLX_FLOW_FUNCTION();

    auto window = std::make_shared<Window>(props);
    m_windows.push_back(window);

    // if this is the first window, set it as the current window
    if (m_windows.size() == 1) Internal_SetCurrentWindow(window.get());

    return window;
  }

  void Application::CloseWindow(const std::string& window_title)
  {
    // find the window with the given title and close it
    CloseWindow(
      *(std::find_if(
        m_windows.begin(), m_windows.end(),
        [&window_title](std::shared_ptr<Window> window) { return window->GetTitle() == window_title; }
      ))
    );
  }

  void Application::CloseWindow(std::shared_ptr<Window> window)
  {
    FLX_FLOW_FUNCTION();

    window->Close();
    m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), window), m_windows.end());
  }

  #pragma endregion

  #pragma region Active Window Handling

  Window* Application::GetCurrentWindow()
  {
    return m_current_window;
  }

  void Application::Internal_SetCurrentWindow(Window* window)
  {
    m_current_window = window;
  }

  #pragma endregion


  void Application::Run()
  {
    while (m_is_running)
    {
      // poll IO events (keys pressed/released, mouse moved etc.)
      glfwPollEvents();

      // run the application state
      ApplicationStateManager::Update();

      // quit application
      if (
        m_is_closing ||
        Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_Q)
      )
      {
        Application::Internal_Close();
      }

      // input cleanup (updates key states and mouse delta for the next frame)
      Input::Cleanup();
    }
  }

}