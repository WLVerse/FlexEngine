#pragma once

#include "flx_api.h"

#include <filesystem>
#include <fstream>
#include <string>

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
    static bool is_initialized;
    static std::filesystem::path log_base_path;
    static std::filesystem::path log_file_path;
    static std::fstream log_stream;
    static bool is_fatal;
    static int flow_scope;

  public:
    Log();
    ~Log();

    // User-defined debug messages
    static void Debug(const char* message)    { Internal_Logger(_Debug, message); }
    static void Debug(std::string message)    { Debug(message.c_str()); }

    // Messages for debugging the flow of the application
    // Prefer the FLX_FLOW_FUNCTION() macro
    static void Flow(const char* message)     { Internal_Logger(_Flow, message); }
    static void Flow(std::string message)     { Flow(message.c_str()); }

    // Informational messages
    static void Info(const char* message)     { Internal_Logger(_Info, message); }
    static void Info(std::string message)     { Info(message.c_str()); }

    // Warn if it could cause issues when not addressed
    static void Warning(const char* message)  { Internal_Logger(_Warning, message); }
    static void Warning(std::string message)  { Warning(message.c_str()); }

    // Warn about potentially damaging errors, systems can recover if dealt with
    static void Error(const char* message)    { Internal_Logger(_Error, message); }
    static void Error(std::string message)    { Error(message.c_str()); }

    // Kills the application asap to prevent it from damaging the system
    static void Fatal(const char* message)    { Internal_Logger(_Fatal, message); }
    static void Fatal(std::string message)    { Fatal(message.c_str()); }

    // Dumps the logs to a file
    static void DumpLogs(void);

    // This flag is set to true when a fatal error is logged
    // It is used to determine if the application should dump logs when closing
    static bool IsFatal(void) { return is_fatal; }

    // Adds indentations to the flow to make it easier to read
    // Use the FLX_FLOW_BEGINSCOPE() and FLX_FLOW_ENDSCOPE() macros
    // This is not meant to be used directly
    static void UpdateFlowScope(int indent) { flow_scope += indent; }

  private:
    enum WarningLevel
    {
      _Debug = 0,
      _Flow,
      _Info,
      _Warning,
      _Error,
      _Fatal,
      Last = _Fatal
    };

    static void Internal_Logger(WarningLevel level, const char* message);
  };

}
