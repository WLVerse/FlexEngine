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
    is_initialized = true;


    #ifdef GAME  // change to USERPROFILE/Documents/ChronoDrift/
    wchar_t documents_path[MAX_PATH];
    DWORD result = GetEnvironmentVariableW(L"USERPROFILE", documents_path, MAX_PATH);

    if (result > 0 && result < MAX_PATH)
    {
      // USERPROFILE/Documents/ChronoDrift/.log
      log_base_path = std::filesystem::path(documents_path) / "Documents" / "ChronoDrift" / ".log";
      log_file_path = log_base_path / "~$flex.log";
    }
    else
    {
      // Fallback to current directory
      log_base_path = std::filesystem::current_path() / ".log";
      log_file_path = log_base_path / "~$flex.log";
    }
    #endif GAME

    // create logs directory
    if (!std::filesystem::exists(log_base_path))
    {
      std::filesystem::create_directories(log_base_path);
    }

    // hide log folder
    SetFileAttributes(log_base_path.c_str(), FILE_ATTRIBUTE_HIDDEN);

    // open log file
    log_stream.open(log_file_path, std::ios::out | std::ios::app); // can't use std::ios::trunc
    FLX_INTERNAL_ASSERT(log_stream.is_open(), "Could not open log file.");

    // hide temporary log file
    SetFileAttributes(log_file_path.c_str(), FILE_ATTRIBUTE_HIDDEN);

    #ifdef _DEBUG
    // file will be opened and updated frequently in debug mode
    log_stream.close();
    #endif

    SetLogLevel();

    FLX_FLOW_BEGINSCOPE();
  }
  Log::~Log()
  {
    FLX_FLOW_ENDSCOPE();

    // close log file to save logs
    log_stream.close();

    #ifdef _DEBUG
    // dump logs if debugging
    Log::DumpLogs();
    #endif

    // remove temporary log file
    FLX_INTERNAL_ASSERT(std::filesystem::remove(log_file_path), "Error removing temporary log file.");

    is_initialized = false;
  }

  Log::LogLevel Log::GetLogLevel()
  {
    return log_level;
  }

  void Log::SetLogLevel(LogLevel level)
  {
    log_level = level;
  }

  void Log::DumpLogs(void)
  {
    // get filename
    std::stringstream filename{};
    filename << datetime << ".log";
    //filename << datetime << "_" << std::setw(3) << std::setfill('0') << current_file_index << ".log";
    std::filesystem::path save_path = log_base_path / filename.str();

    // save log file
    log_stream.close();
    bool success = std::filesystem::copy_file(log_file_path, save_path, std::filesystem::copy_options::overwrite_existing);
    if (success)
    {
      SetFileAttributes(save_path.c_str(), FILE_ATTRIBUTE_NORMAL);
      Info("Log file saved to " + save_path.string());
    }
    else
    {
      Error("Error copying log file.");
    }
  }

  void Log::Internal_IncrementLogFile()
  {
    Log::DumpLogs();

    // clear log stream
    log_stream.seekp(0, std::ios::beg);
    log_stream << std::endl;
    log_stream.seekp(0, std::ios::beg);

    // increment file index
    current_file_index++;

    // reset file size
    current_file_size = 0;
  }

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
    // default passthrough to std::cout if not initialized
    if (!is_initialized)
    {
      std::cout << "The FlexLogger is not initialized. Message: " << message << std::endl;
      return;
    }

    // filter logs based on log level
    if (level < GetLogLevel()) return;

    // create string stream
    std::stringstream ss;

    // print time
    ss << "[" << DateTime::GetFormattedDateTime() << "] ";

    // prepend warning level and set color
    switch (level)
    {
    case LogLevel_Debug:    ss << TAG_COLOR_DEBUG << " Debug"   " " << ANSI_RESET << " -> " << TEXT_COLOR_DEBUG; break;
    case LogLevel_Flow:     ss << TAG_COLOR_FLOW << " Flow"    " " << ANSI_RESET << " -> " << TEXT_COLOR_FLOW; break;
    case LogLevel_Info:     ss << TAG_COLOR_INFO << " Info"    " " << ANSI_RESET << " -> " << TEXT_COLOR_INFO; break;
    case LogLevel_Warning:  ss << TAG_COLOR_WARNING << " Warning" " " << ANSI_RESET << " -> " << TEXT_COLOR_WARNING; break;
    case LogLevel_Error:    ss << TAG_COLOR_ERROR << " Error"   " " << ANSI_RESET << " -> " << TEXT_COLOR_ERROR; break;
    case LogLevel_Fatal:    ss << TAG_COLOR_FATAL << " Fatal"   " " << ANSI_RESET << " -> " << TEXT_COLOR_FATAL; break;
    default:                ss << TAG_COLOR_DEFAULT << " Unknown" " " << ANSI_RESET << " -> " << TEXT_COLOR_DEFAULT; break;
    }

    // append flow scope
    if (level == LogLevel_Flow)
    {
      ss << ANSI_RESET;
      for (int i = 0; i < flow_scope; i++) ss << "| ";
      ss << TEXT_COLOR_FLOW;
    }

    // append message and reset color
    // extra space for fatal messages
    ss
      << std::string((level == LogLevel_Fatal), ' ')
      << message
      << std::string((level == LogLevel_Fatal), ' ')
      << ANSI_RESET
      << "\n"
      ;

    // clean the stream of ANSI color codes for file logging
    std::string log_string = ss.str();
    size_t pos = 0;
    while ((pos = log_string.find("\033[")) != std::string::npos)
    {
      // find m
      size_t end = log_string.find("m", pos);
      log_string.erase(pos, end - pos + 1);
    }

    current_file_size += log_string.size() + 1; // +1 for newline

    // if log file is too large, increment log file
    //if (current_file_size > max_log_file_size)
    //{
    //  Internal_IncrementLogFile();
    //}

    // log to console and file based on warning level
    #ifdef _DEBUG
    std::cout << ss.str();

    // file will be opened and updated frequently in debug mode
    log_stream.open(log_file_path.string(), std::ios::out | std::ios::app);
    log_stream << log_string;
    log_stream.close();
    #else
        // full logs will be saved in release mode
    if (!log_stream.is_open())
    {
      log_stream.open(log_file_path.string(), std::ios::out | std::ios::app);
    }
    log_stream << log_string;
    #endif

    // quit application if fatal
    if (level == LogLevel_Fatal)
    {
      //Log::DumpLogs();
      std::exit(EXIT_FAILURE);
    }
  }
}