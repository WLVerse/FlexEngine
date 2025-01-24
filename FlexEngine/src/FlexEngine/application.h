// WLVERSE [https://wlverse.web.app]
// application.h
// 
// The architecture of the engine is designed to only have one application instance.
// This manages the windows and the main loop of the application.
// 
// It uses common design patterns like the Registry Pattern, Command Pattern,
// and Active Object Pattern.
// 
// All actions on the application, window, and layerstack are deferred and controlled
// within the Application::Run function. It must be resolved in this order.
// - Window LayerStack
// - Window
// - Application LayerStack
// - Application
// 
// The application contains a headless window, this forces this application to
// always have a window. This is useful for applications that require
// multi-window support.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "DataStructures/functionqueue.h"
#include "Layer/layerstack.h"
#include "window.h"
#include <string>
#include <vector>
#include <memory> // std::shared_ptr
#include <any>
#include <queue>

int main(int, char**);

namespace FlexEngine
{

  // The architecture of the engine is designed to only have one application instance.
  class __FLX_API Application
  {
    #pragma region Application Loop Management

    // Run() starts the loop
    // Stop the loop by setting m_is_running to false
    // m_is_closing is the flag used to close the application
    // Code at the end of the loop will break out of the loop

  private:
    static bool m_is_running;
    static bool m_is_closing;

  public:
    // Closes the application and all windows
    // Important to note that this does not immediately close the application,
    // the application will close at the end of the current frame.
    static void Quit();

    #pragma endregion

    #pragma region Registry Pattern
    // This pattern is used to manage the windows in the application.

  public:
    using WindowRegistry = std::unordered_map<std::string, std::shared_ptr<Window>>;

  private:
    static WindowRegistry m_window_registry;

  public:
    static WindowRegistry& GetWindowRegistry() { return m_window_registry; }

    // Sets the newly open window as the active window context
    // Window initialization is deferred to the end of the frame
    static void OpenWindow(const std::string& window_name, const WindowProps& props = {});

    static void CloseWindow(const std::string& window_name);

    static void CloseAllWindows();

  private:
    // allow access to internal functions
    friend class Window;

    #pragma endregion

    #pragma region Active Object Pattern (Window)

  private:
    static Window* m_current_window;

  public:
    // Returns the current window.
    // This is not the same as the focused window, the current window is
    // the window that is currently being processed.
    // Works somewhat like the opengl or imgui context.
    static Window* GetCurrentWindow() { return m_current_window; }

  private:
    // INTERNAL FUNCTION
    // Sets the current window pointer
    // This does not set the current window context for glfw or imgui
    static void Internal_SetCurrentWindow(Window* window) { m_current_window = window; }

    #pragma endregion

    #pragma region Command Pattern

    #pragma region Helper Macros

    // Helper macros to reduce boilerplate code
    // The macros are named FLX_COMMAND_<COMMAND_NAME>()

    // Quit the application.
    // Usage: FLX_COMMAND_QUIT_APPLICATION();
    #define FLX_COMMAND_QUIT_APPLICATION() \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::QuitApplication \
        ) \
      )

    #pragma region Application

    // Add a layer to the application layerstack.
    // Pass in a shared pointer to the layer.
    // Usage: FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<Layer>());
    #define FLX_COMMAND_ADD_APPLICATION_LAYER(layer) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Application_AddLayer, \
          layer \
        ) \
      )

    // Add an overlay to the application layerstack.
    // Pass in a shared pointer to the overlay.
    // Usage: FLX_COMMAND_ADD_APPLICATION_OVERLAY(std::make_shared<Layer>());
    #define FLX_COMMAND_ADD_APPLICATION_OVERLAY(overlay) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Application_AddOverlay, \
          overlay \
        ) \
      )

    // Remove a layer from the application layerstack.
    // Pass in the pointer to the layer.
    // Usage: FLX_COMMAND_REMOVE_APPLICATION_LAYER(Layer);
    #define FLX_COMMAND_REMOVE_APPLICATION_LAYER(layer) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Application_RemoveLayer, \
          layer \
        ) \
      )

    // Remove an overlay from the application layerstack.
    // Pass in the pointer to the overlay.
    // Usage: FLX_COMMAND_REMOVE_APPLICATION_OVERLAY(Overlay);
    #define FLX_COMMAND_REMOVE_APPLICATION_OVERLAY(overlay) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Application_RemoveOverlay, \
          overlay \
        ) \
      )

    // Clear the application layerstack.
    // Usage: FLX_COMMAND_CLEAR_APPLICATION_LAYER_STACK();
    #define FLX_COMMAND_CLEAR_APPLICATION_LAYER_STACK() \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Application_ClearLayerStack \
        ) \
      )

    #pragma endregion

    // Open a window.
    // Pass in the name of the window and the window properties.
    // Usage: FLX_COMMAND_OPEN_WINDOW("WindowName", WindowProps(...));
    #define FLX_COMMAND_OPEN_WINDOW(window_name, props) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::OpenWindow, \
          window_name, \
          props \
        ) \
      )

    // Close a window.
    // Pass in the name of the window.
    // Usage: FLX_COMMAND_CLOSE_WINDOW("WindowName");
    #define FLX_COMMAND_CLOSE_WINDOW(window_name) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::CloseWindow, \
          window_name \
        ) \
      )

    // Close all windows.
    // Usage: FLX_COMMAND_CLOSE_ALL_WINDOWS();
    #define FLX_COMMAND_CLOSE_ALL_WINDOWS() \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::CloseAllWindows \
        ) \
      )

    #pragma region Window
    
    // Add a layer to the window layerstack.
    // Pass in the name of the window and a shared pointer to the layer.
    // Usage: FLX_COMMAND_ADD_WINDOW_LAYER("WindowName", std::make_shared<Layer>());
    #define FLX_COMMAND_ADD_WINDOW_LAYER(window_name, layer) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Window_AddLayer, \
          window_name, \
          layer \
        ) \
      )

    // Add an overlay to the window layerstack.
    // Pass in the name of the window and a shared pointer to the overlay.
    // Usage: FLX_COMMAND_ADD_WINDOW_OVERLAY("WindowName", std::make_shared<Layer>());
    #define FLX_COMMAND_ADD_WINDOW_OVERLAY(window_name, overlay) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Window_AddOverlay, \
          window_name, \
          overlay \
        ) \
      )

    // Remove a layer from the window layerstack.
    // Pass in the name of the window and the layer pointer.
    // Usage: FLX_COMMAND_REMOVE_WINDOW_LAYER("WindowName", Layer);
    #define FLX_COMMAND_REMOVE_WINDOW_LAYER(window_name, layer) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Window_RemoveLayer, \
          window_name, \
          layer \
        ) \
      )

    // Remove an overlay from the window layerstack.
    // Pass in the name of the window and the overlay pointer.
    // Usage: FLX_COMMAND_REMOVE_WINDOW_OVERLAY("WindowName", Overlay);
    #define FLX_COMMAND_REMOVE_WINDOW_OVERLAY(window_name, overlay) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Window_RemoveOverlay, \
          window_name, \
          overlay \
        ) \
      )

    // Clear the window layerstack.
    // Pass in the name of the window.
    // Usage: FLX_COMMAND_CLEAR_WINDOW_LAYER_STACK("WindowName");
    #define FLX_COMMAND_CLEAR_WINDOW_LAYER_STACK(window_name) \
      FlexEngine::Application::QueueCommand( \
        FlexEngine::Application::CommandData( \
          FlexEngine::Application::Command::Window_ClearLayerStack, \
          window_name \
        ) \
      )

    #pragma endregion

    #pragma endregion

  public:
    enum class Command
    {
      NullCommand = 0,

      QuitApplication,

      Application_AddLayer,
      Application_AddOverlay,
      Application_RemoveLayer,
      Application_RemoveOverlay,
      Application_ClearLayerStack,

      OpenWindow,
      CloseWindow,
      CloseAllWindows,

      Window_AddLayer,
      Window_AddOverlay,
      Window_RemoveLayer,
      Window_RemoveOverlay,
      Window_ClearLayerStack,

      LastCommand
    };

    struct __FLX_API CommandData
    {
      Command command_type = Command::LastCommand;
      std::string window_name = "";
      WindowProps window_props = WindowProps::Null;
      std::shared_ptr<Layer> layer = nullptr;

      CommandData(Command type)
        : command_type(type)
      {
      }

      CommandData(Command type, const std::string& _window_name)
        : command_type(type), window_name(_window_name)
      {
      }

      CommandData(Command type, std::shared_ptr<Layer> _layer)
        : command_type(type), layer(_layer)
      {
      }

      CommandData(Command type, const std::string& _window_name, const WindowProps& props)
        : command_type(type), window_name(_window_name), window_props(props)
      {
      }

      CommandData(Command type, const std::string& _window_name, std::shared_ptr<Layer> _layer)
        : command_type(type), window_name(_window_name), layer(_layer)
      {
      }
    };

  private:
    static std::queue<CommandData> m_command_queue;

  public:
    static void QueueCommand(CommandData data) { m_command_queue.push(data); }
    static void ProcessCommands();

    #pragma endregion

    #pragma region LayerStack
    // Uses the Composition over Inheritance principle.
    
  private:
    // Used to manage the logic in the application.
    static LayerStack m_layer_stack;

  public:
    // Returns the layer stack
    static LayerStack& GetLayerStack() { return m_layer_stack; }

  public:
    static void Window_AddLayer(const std::string& window_name, std::shared_ptr<Layer> layer);
    static void Window_AddOverlay(const std::string& window_name, std::shared_ptr<Layer> overlay);
    static void Window_RemoveLayer(const std::string& window_name, const std::string& layer_name);
    static void Window_RemoveOverlay(const std::string& window_name, const std::string& overlay_name);
    static void Window_ClearLayerStack(const std::string& window_name);

    #pragma endregion

    #pragma region Messaging System

  public:
    // Messaging implementation for communication between windows
    // Send a message by calling Send<Type>("test", data)
    // A listener would be checking for the message by calling Receive<Type>("test")
    // Any data can be sent by casting it to std::any
    class __FLX_API MessagingSystem
    {
    public:
      using Messages = std::unordered_map<std::string, std::any>;

    private:
      static Messages m_messages;

    public:
      static Messages& Get() { return m_messages; }
      static void Clear() { m_messages.clear(); }

      template <typename T>
      static void Send(const std::string& message, T data)
      {
        // guard
        if (m_messages.count(message) != 0) return;
        m_messages[message] = std::any(data);
      }
      template <typename T>
      static T Receive(const std::string& message)
      {
        // guard
        if (m_messages.count(message) == 0) return T();
        
        try
        {
          T data = std::any_cast<T>(m_messages[message]);
          m_messages.erase(message);
          return data;
        }
        catch (const std::bad_any_cast& e)
        {
          FLX_CORE_ASSERT(false, std::string("Failed to cast message data while trying to retrieve message. ") + e.what());
          return T();
        }
      }
    };

    #pragma endregion

    #pragma region Application Framework Pattern (Inversion of Control)

  public:
    Application();
    virtual ~Application();

    // This is the only function called by the entrypoint.
    static void Run();

    friend int ::main(int, char**);

    static OpenGLFrameBufferManager FrameBufferManager;

    #pragma endregion

    #pragma region Headless Window (Master Window)

    // The master window is a headless window that is always open.
    // It stores the opengl context that is shared between every window.

  public:

    // Returns the master window, used for opengl context sharing.
    GLFWwindow* GetMasterWindow() { return m_master_window; }

  private:

    static GLFWwindow* m_master_window;

    // INTERNAL FUNCTION
    // Creates the headless window and stores it.
    // This is not a window and also not stored in the window registry.
    void Internal_CreateMasterWindow();

    #pragma endregion
  };

  // Application Framework Pattern (Inversion of Control)
  Application* CreateApplication();
}
