// WLVERSE [https://wlverse.web.app]
// flexlogger.cpp
//
// Debug logger implementation for logging debug messages, flow of the
// application, informational messages, warnings, errors, and fatal errors. 
// 
// The logger will dump the logs to a file when the application closes. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include <Windows.h> // SetFileAttributes

#include "Utilities/ansi_color.h"
#include "Utilities/datetime.h"

#include "application.h"

// Helper macros for colorizing console output
#pragma region Colors

#define TAG_COLOR_DEBUG     ANSI_COLOR(ANSI_FG_BRIGHT_WHITE ";" ANSI_BG_MAGENTA)
#define TAG_COLOR_FLOW      ANSI_COLOR(ANSI_FG_BRIGHT_WHITE ";" ANSI_BG_BLUE)
#define TAG_COLOR_INFO      ANSI_COLOR(ANSI_FG_BLACK ";" ANSI_BG_BRIGHT_WHITE)
#define TAG_COLOR_WARNING   ANSI_COLOR(ANSI_FG_BLACK ";" ANSI_BG_YELLOW)
#define TAG_COLOR_ERROR     ANSI_COLOR(ANSI_FG_BLACK ";" ANSI_BG_BRIGHT_RED)
#define TAG_COLOR_FATAL     ANSI_COLOR(ANSI_FG_BRIGHT_WHITE ";" ANSI_BG_RED ";" ANSI_UNDERLINE_ON)
#define TAG_COLOR_DEFAULT   ANSI_RESET

#define TEXT_COLOR_DEBUG    ANSI_COLOR(ANSI_FG_WHITE)
#define TEXT_COLOR_FLOW     ANSI_COLOR(ANSI_FG_BRIGHT_BLUE)
#define TEXT_COLOR_INFO     ANSI_COLOR(ANSI_FG_BRIGHT_WHITE)
#define TEXT_COLOR_WARNING  ANSI_COLOR(ANSI_FG_BRIGHT_YELLOW)
#define TEXT_COLOR_ERROR    ANSI_COLOR(ANSI_FG_BRIGHT_RED)
#define TEXT_COLOR_FATAL    ANSI_COLOR(ANSI_FG_BRIGHT_WHITE ";" ANSI_BG_RED)
#define TEXT_COLOR_DEFAULT  ANSI_RESET

#pragma endregion

namespace FlexEngine
{

  // static member initialization
  bool Log::is_initialized = false;
  std::filesystem::path Log::log_base_path{ std::filesystem::current_path() / ".log" }; // same path as executable
  std::filesystem::path Log::log_file_path{ log_base_path / "~$flex.log" };
  std::fstream Log::log_stream;
  bool Log::is_fatal = false;
  int Log::flow_scope = 0;
  Log::LogLevel Log::log_level = LogLevel_All;
  const std::string Log::datetime = DateTime::GetFormattedDateTime("%Y-%m-%d-%H-%M-%S");

  // static member initialization (file splitter)
  //const size_t Log::max_log_file_size = 1024 * 1024 * 10; // 10MB
  //const size_t Log::max_log_file_size = 20;
  int Log::current_file_index = 0;
  size_t Log::current_file_size = 0;

  Log::Log()
  {
    FLX_FLOW_BEGINSCOPE();
  }
  Log::~Log()
  {
  }

  Log::LogLevel Log::GetLogLevel()
  {
    return log_level;
  }

  void Log::SetLogLevel(LogLevel level)
  {
  }

  void Log::DumpLogs(void)
  {
  }

  void Log::Internal_IncrementLogFile()
  {}

  // Internal logger function
  // 
  // Flow:
  // 1. Checks if the logger is initialized
  // 2. Filters logs based on log level
  // 3. Prepends log level and sets color
  // 4. Appends flow scope
  // 5. Appends message and resets color
  // 6. Logs to console and file based on warning level
  // 7. Quits application if fatal
  void Log::Internal_Logger(LogLevel level, const char* message)
  {
  }
}