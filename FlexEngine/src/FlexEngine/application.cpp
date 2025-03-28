// WLVERSE [https://wlverse.web.app]
// application.cpp
//
// The architecture of the engine is designed to only have one application
// instance. 
// This manages the windows and the main loop of the application. 
// 
// All actions on the application, window, and layerstack are deferred and
// controlled 
// within the Application::Run function. It must be resolved in this order. 
// - Window LayerStack 
// - Window 
// - Application LayerStack 
// - Application 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "application.h"

#include "StateManager/statemanager.h"
#include "input.h"
#include "flexprefs.h"
#include "FMOD/FMODWrapper.h" // Include for initializing fmod system at application start
#include "Renderer/Camera/cameramanager.h" //Include for starting up the camera bank
namespace FlexEngine
{
  // static member initialization
  //Application* Application::s_active_application = nullptr;

  bool Application::m_is_running = true;
  bool Application::m_is_closing = false;

  Application::WindowRegistry Application::m_window_registry{};

  Window* Application::m_current_window = nullptr;

  std::queue<Application::CommandData> Application::m_command_queue{};

  LayerStack Application::m_layer_stack{};

  Application::MessagingSystem::Messages Application::MessagingSystem::m_messages{};

  #pragma region Application Loop Management

  void Application::Quit()
  {
    // Step 1: Clear all layers in each window
    for (auto& [window_name, window] : Application::GetWindowRegistry())
    {
      window->GetLayerStack().Clear(); // Clear layers in the window
    }

    // Step 2: Close all windows
    Application::CloseAllWindows();

    // Step 3: Clear the application-level layer stack
    Application::GetLayerStack().Clear();

    // Step 4: Signal the application to exit
    m_is_running = false; // Flag to break out of the main Run loop
  }

  #pragma endregion

  #pragma region Registry Pattern

  void Application::OpenWindow(const std::string& window_name, const WindowProps& props)
  {
    // guard: check if window already exists
    if (m_window_registry.count(window_name) != 0)
    {
      Log::Warning("Cannot open a window with a name that already exists in the registry. Window name: " + window_name);
      return;
    }

    // create the window
    // this validates the window name and properties
    m_window_registry[window_name] = std::make_shared<Window>(window_name, props);

    m_window_registry[window_name]->Open();

    // set the newly open window as the active window context
    m_window_registry[window_name]->SetCurrentContext();
  }

  void Application::CloseWindow(const std::string& window_name)
  {
    // guard: check if window exists
    if (m_window_registry.count(window_name) == 0)
    {
      Log::Warning("Cannot close a window that doesn't exist in the registry. Window name: " + window_name);
      return;
    }

    // Step 1: Clear the window's layers
    m_window_registry[window_name]->GetLayerStack().Clear();

    // Step 2: Close and clean up the window
    m_window_registry[window_name]->Close();
    m_window_registry.erase(window_name);
  }

  void Application::CloseAllWindows()
  {
    for (auto& [window_name, window] : m_window_registry)
    {
      // Step 1: Clear the window's layers
      window->GetLayerStack().Clear();
      // Step 2: Close and clean up the window
      window->Close();
    }

    // Step 3: Clear the window registry
    m_window_registry.clear();
  }

  #pragma endregion

  #pragma region Command Pattern

  #pragma region Helper macros

  // Every command either requires or doesn't need a parameter.
  // This provides feedback to the user if they are using the command incorrectly.

  // This asserts that the command has all the required data
  // The condition must be true.
  // Usage: _FLX_CMD_REQUIRE(cmd.layer, Application_AddLayer, "layer pointer");
  #define _FLX_CMD_REQUIRE(condition, cmd, type) \
    FLX_CORE_ASSERT(condition, "The " cmd " command requires a valid " type " as a parameter. Double check your FLX_COMMAND macros.");

  // This warns the user if extra params are passed into the command.
  // The condition will flag a warning if it's false.
  // Usage: _FLX_CMD_WARN(!cmd.window_name.empty(), Application_AddLayer, "window name");
  #define _FLX_CMD_WARN(condition, cmd, type) \
    if (condition) \
    { \
      Log::Warning("The " cmd " does not require " type " as a parameter. Double check your FLX_COMMAND macros."); \
    }

  #pragma endregion

  void Application::ProcessCommands()
  {
    while (!m_command_queue.empty())
    {
      const auto& cmd = m_command_queue.front();

      switch (cmd.command_type)
      {
      case Command::NullCommand:
        FLX_CORE_ASSERT(false,
          "Null command type processed in the application command queue. "
          "Use the provided macros FLX_COMMAND_<COMMAND_NAME>(); for the command queue."
        );
        break;

        // Application

      case Command::QuitApplication:
        m_is_closing = true;
        break;

        // Application LayerStack

      case Command::Application_AddLayer:
        _FLX_CMD_REQUIRE(cmd.layer, "Application_AddLayer", "layer pointer");
        _FLX_CMD_WARN(!cmd.window_name.empty(), "Application_AddLayer", "a window name");
        _FLX_CMD_WARN(cmd.window_props, "Application_AddLayer", "window properties");
        Application::GetLayerStack().AddLayer(cmd.layer);
        break;

      case Command::Application_AddOverlay:
        _FLX_CMD_REQUIRE(cmd.layer, "Application_AddOverlay", "layer pointer");
        _FLX_CMD_WARN(!cmd.window_name.empty(), "Application_AddOverlay", "a window name");
        _FLX_CMD_WARN(cmd.window_props, "Application_AddOverlay", "window properties");
        Application::GetLayerStack().AddOverlay(cmd.layer);
        break;

      case Command::Application_RemoveLayer:
        _FLX_CMD_REQUIRE(cmd.layer, "Application_RemoveLayer", "layer pointer");
        _FLX_CMD_WARN(!cmd.window_name.empty(), "Application_RemoveLayer", "a window name");
        _FLX_CMD_WARN(cmd.window_props, "Application_RemoveLayer", "window properties");
        Application::GetLayerStack().RemoveLayer(cmd.layer->GetName());
        break;

      case Command::Application_RemoveOverlay:
        _FLX_CMD_REQUIRE(cmd.layer, "Application_RemoveOverlay", "layer pointer");
        _FLX_CMD_WARN(!cmd.window_name.empty(), "Application_RemoveOverlay", "a window name");
        _FLX_CMD_WARN(cmd.window_props, "Application_RemoveOverlay", "window properties");
        Application::GetLayerStack().RemoveOverlay(cmd.layer->GetName());
        break;

      case Command::Application_ClearLayerStack:
        _FLX_CMD_WARN(cmd.layer, "Application_ClearLayerStack", "a layer pointer");
        _FLX_CMD_WARN(!cmd.window_name.empty(), "Application_ClearLayerStack", "window name");
        _FLX_CMD_WARN(cmd.window_props, "Application_ClearLayerStack", "window properties");
        Application::GetLayerStack().Clear();
        break;

        // Window

      case Command::OpenWindow:
        _FLX_CMD_WARN(cmd.layer, "OpenWindow", "a layer pointer");
        _FLX_CMD_REQUIRE(!cmd.window_name.empty(), "OpenWindow", "window name");
        _FLX_CMD_REQUIRE(cmd.window_props, "OpenWindow", "window properties");
        Application::OpenWindow(cmd.window_name, cmd.window_props);
        break;

      case Command::CloseWindow:
        _FLX_CMD_WARN(cmd.layer, "CloseWindow", "a layer pointer");
        _FLX_CMD_REQUIRE(!cmd.window_name.empty(), "CloseWindow", "window name");
        _FLX_CMD_WARN(cmd.window_props, "CloseWindow", "window properties");
        Application::CloseWindow(cmd.window_name);
        break;

      case Command::CloseAllWindows:
        _FLX_CMD_WARN(cmd.layer, "CloseAllWindows", "a layer pointer");
        _FLX_CMD_WARN(!cmd.window_name.empty(), "CloseAllWindows", "window name");
        _FLX_CMD_WARN(cmd.window_props, "CloseAllWindows", "window properties");
        Application::CloseAllWindows();
        break;

        // Window LayerStack

      case Command::Window_AddLayer:
        _FLX_CMD_REQUIRE(cmd.layer, "Window_AddLayer", "layer pointer");
        _FLX_CMD_REQUIRE(!cmd.window_name.empty(), "Window_AddLayer", "window name");
        _FLX_CMD_WARN(cmd.window_props, "Window_AddLayer", "window properties");
        Application::Window_AddLayer(cmd.window_name, cmd.layer);
        break;

      case Command::Window_AddOverlay:
        _FLX_CMD_REQUIRE(cmd.layer, "Window_AddOverlay", "layer pointer");
        _FLX_CMD_REQUIRE(!cmd.window_name.empty(), "Window_AddOverlay", "window name");
        _FLX_CMD_WARN(cmd.window_props, "Window_AddOverlay", "window properties");
        Application::Window_AddOverlay(cmd.window_name, cmd.layer);
        break;

      case Command::Window_RemoveLayer:
        _FLX_CMD_REQUIRE(cmd.layer, "Window_RemoveLayer", "layer pointer");
        _FLX_CMD_REQUIRE(!cmd.window_name.empty(), "Window_RemoveLayer", "window name");
        _FLX_CMD_WARN(cmd.window_props, "Window_RemoveLayer", "window properties");
        Application::Window_RemoveLayer(cmd.window_name, cmd.layer->GetName());
        break;

      case Command::Window_RemoveOverlay:
        _FLX_CMD_REQUIRE(cmd.layer, "Window_RemoveOverlay", "layer pointer");
        _FLX_CMD_REQUIRE(!cmd.window_name.empty(), "Window_RemoveOverlay", "window name");
        _FLX_CMD_WARN(cmd.window_props, "Window_RemoveOverlay", "window properties");
        Application::Window_RemoveOverlay(cmd.window_name, cmd.layer->GetName());
        break;

      case Command::Window_ClearLayerStack:
        _FLX_CMD_WARN(cmd.layer, "Window_ClearLayerStack", "a layer pointer");
        _FLX_CMD_REQUIRE(!cmd.window_name.empty(), "Window_ClearLayerStack", "window name");
        _FLX_CMD_WARN(cmd.window_props, "Window_ClearLayerStack", "window properties");
        Application::Window_ClearLayerStack(cmd.window_name);
        break;

      default:
        Log::Warning(
          "Invalid command type (" +
          std::to_string(static_cast<int>(cmd.command_type)) +
          ") in the application command queue. "
          "Use the provided macros FLX_COMMAND_<COMMAND_NAME>(); for the command queue. "
          "Check valid command types in the Command enum in application.h."
        );
        break;
      }

      m_command_queue.pop();
    }

    // quit the application if the close flag is set
    if (m_is_closing) Quit();
  }

  #pragma endregion

  #pragma region LayerStack

  void Application::Window_AddLayer(const std::string& window_name, std::shared_ptr<Layer> layer)
  {
    // guard: check if window exists
    if (m_window_registry.count(window_name) == 0)
    {
      Log::Warning("Cannot add a layer to a window that doesn't exist in the registry. Window name: " + window_name);
      return;
    }
    m_window_registry[window_name]->GetLayerStack().AddLayer(layer);
  }

  void Application::Window_AddOverlay(const std::string& window_name, std::shared_ptr<Layer> overlay)
  {
    // guard: check if window exists
    if (m_window_registry.count(window_name) == 0)
    {
      Log::Warning("Cannot add an overlay to a window that doesn't exist in the registry. Window name: " + window_name);
      return;
    }
    m_window_registry[window_name]->GetLayerStack().AddOverlay(overlay);
  }

  void Application::Window_RemoveLayer(const std::string& window_name, const std::string& layer_name)
  {
    // guard: check if window exists
    if (m_window_registry.count(window_name) == 0)
    {
      Log::Warning("Cannot remove a layer from a window that doesn't exist in the registry. Window name: " + window_name);
      return;
    }
    m_window_registry[window_name]->GetLayerStack().RemoveLayer(layer_name);
  }

  void Application::Window_RemoveOverlay(const std::string& window_name, const std::string& overlay_name)
  {
    // guard: check if window exists
    if (m_window_registry.count(window_name) == 0)
    {
      Log::Warning("Cannot remove an overlay from a window that doesn't exist in the registry. Window name: " + window_name);
      return;
    }
    m_window_registry[window_name]->GetLayerStack().RemoveOverlay(overlay_name);
  }

  void Application::Window_ClearLayerStack(const std::string& window_name)
  {
    // guard: check if window exists
    if (m_window_registry.count(window_name) == 0)
    {
      Log::Warning("Cannot clear the layer stack of a window that doesn't exist in the registry. Window name: " + window_name);
      return;
    }
    m_window_registry[window_name]->GetLayerStack().Clear();
  }

  #pragma endregion

  #pragma region Application Framework Pattern (Inversion of Control)

  Application::Application()
  {
    FLX_FLOW_BEGINSCOPE();

    // initialize glfw
    FLX_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW!");

    // Load the saved preferences from file.
    FlexPrefs::Load();

    FMODWrapper::Load();

  }

  Application::~Application()
  {
    glfwMakeContextCurrent(NULL);
    glfwTerminate();
    FMODWrapper::Unload();
    FLX_FLOW_ENDSCOPE();
  }

  void Application::Run()
  {
    // This is the main loop of the application
    while (m_is_running)
    {
      // poll IO events (keys pressed/released, mouse moved etc.)
      // this is suggested to always come first in the loop
      glfwPollEvents();

      // run the layerstack
      Application::GetLayerStack().Update();
      FMODWrapper::Update();

      //ApplicationStateManager::Update();

      // keybind to close the application
      if (Input::GetKey(GLFW_KEY_LEFT_CONTROL) && Input::GetKeyDown(GLFW_KEY_Q))
      {
        m_is_closing = true;
      }

      // All actions on the application, window, and layerstack are deferred.
      // This processes them all.
      ProcessCommands();

      // input cleanup (updates key states and mouse delta for the next frame)
      Input::Cleanup();

      // state switching only happens at the very end of the frame
      //ApplicationStateManager::UpdateManager();
    }

  }

  #pragma endregion

}