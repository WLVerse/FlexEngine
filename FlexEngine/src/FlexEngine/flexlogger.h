// WLVERSE [https://wlverse.web.app]
// flexlogger.h
// 
// Debug logger implementation for logging debug messages,
// flow of the application, informational messages, warnings,
// errors, and fatal errors.
// 
// The logger will dump the logs to a file when the application closes.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

// These macros are used to track the flow of the application
// Putting these into the code will make it easier to see if the application is behaving as expected
// The FLX_FLOW_BEGINSCOPE() and FLX_FLOW_ENDSCOPE() macros will add indentations to the flow
// if you expect them to be called in pairs

#define FLX_FLOW_FUNCTION()   FlexEngine::Log::Flow(__FUNCTION__)
#define FLX_FLOW_BEGINSCOPE() FLX_FLOW_FUNCTION(); FlexEngine::Log::UpdateFlowScope(1)
#define FLX_FLOW_ENDSCOPE()   FlexEngine::Log::UpdateFlowScope(-1); FLX_FLOW_FUNCTION()

namespace FlexEngine
{
  class __FLX_API Log
  {
  public:
    Log();
    ~Log();

    // User-defined debug messages
    static void Debug(const char* message) { Internal_Logger(LogLevel_Debug, message); }
    static void Debug(std::string message) { Debug(message.c_str()); }
    static void Debug(std::stringstream& ss) { Debug(ss.str()); }

    // Messages for debugging the flow of the application
    // Prefer the FLX_FLOW_FUNCTION() macro
    static void Flow(const char* message) { Internal_Logger(LogLevel_Flow, message); }
    static void Flow(std::string message) { Flow(message.c_str()); }
    static void Flow(std::stringstream& ss) { Flow(ss.str()); }

    // Informational messages
    static void Info(const char* message) { Internal_Logger(LogLevel_Info, message); }
    static void Info(std::string message) { Info(message.c_str()); }
    static void Info(std::stringstream& ss) { Info(ss.str()); }

    // Warn if it could cause issues when not addressed
    static void Warning(const char* message) { Internal_Logger(LogLevel_Warning, message); }
    static void Warning(std::string message) { Warning(message.c_str()); }
    static void Warning(std::stringstream& ss) { Warning(ss.str()); }

    // Warn about potentially damaging errors, systems can recover if dealt with
    static void Error(const char* message) { Internal_Logger(LogLevel_Error, message); }
    static void Error(std::string message) { Error(message.c_str()); }
    static void Error(std::stringstream& ss) { Error(ss.str()); }

    // Kills the application asap to prevent it from damaging the system
    static void Fatal(const char* message) { Internal_Logger(LogLevel_Fatal, message); }
    static void Fatal(std::string message) { Fatal(message.c_str()); }
    static void Fatal(std::stringstream& ss) { Fatal(ss.str()); }

    // This flag is set to true when a fatal error is logged
    // It is used to determine if the application should dump logs when closing
    static bool IsFatal(void) { return is_fatal; }

    // Adds indentations to the flow to make it easier to read
    // Use the FLX_FLOW_BEGINSCOPE() and FLX_FLOW_ENDSCOPE() macros
    // This is not meant to be used directly
    static void UpdateFlowScope(int indent) { flow_scope += indent; }

    // Log levels enum for filtering logs
    // All logs are logged by default on debug mode
    // Only logs with the same or higher level than Flow will be logged in release mode
    // A custom log level can be set in the application
    enum LogLevel
    {
      LogLevel_All = 0,
      LogLevel_Debug,
      LogLevel_Flow,
      LogLevel_Info,
      LogLevel_Warning,
      LogLevel_Error,
      LogLevel_Fatal
    };

    // Get the current log level
    static LogLevel GetLogLevel();

    #ifdef _DEBUG
    // Set the log level for the application
    // All logs are logged by default on debug mode
    // Only logs with the same or higher level than Info will be logged in release mode
    static void SetLogLevel(LogLevel level = LogLevel_All);
    #else
    // Set the log level for the application
    // All logs are logged by default on debug mode
    // Only logs with the same or higher level than Info will be logged in release mode
    static void SetLogLevel(LogLevel level = LogLevel_Info);
    #endif

    // Dumps the logs to a file
    // Copies the hidden log_stream to an unhidden named file
    static void DumpLogs(void);

  private:
    static bool is_initialized;
    static std::filesystem::path log_base_path;
    static std::filesystem::path log_file_path;
    static std::fstream log_stream;
    static bool is_fatal;
    static int flow_scope;
    static LogLevel log_level;
    static const std::string datetime; // The cached datetime when the logger started

    #pragma region File Splitter

    // Maximum log size before it is dumped to a file
    static const size_t max_log_file_size;

    static int current_file_index;
    static size_t current_file_size;

    // INTERNAL FUNCTION
    // Increments the log file number
    static void Internal_IncrementLogFile();

    #pragma endregion

    // INTERNAL FUNCTION
    // This handles the logging of messages
    static void Internal_Logger(LogLevel level, const char* message);
  };

}
