#pragma once

#include "flexlogger.h" // <filesystem> <fstream> <string>

#include <chrono>

#ifdef _DEBUG
  // Logs the duration automatically when it goes out of scope
  #define FLX_SCOPED_TIMER(TEXT) FlexEngine::Timer timer(TEXT);
  #define FLX_SCOPED_FUNCTION_TIMER() FlexEngine::Timer timer(__FUNCTION__);
#else
  // Disabled in release mode
  #define FLX_SCOPED_TIMER(TEXT)
  #define FLX_SCOPED_FUNCTION_TIMER()
#endif

namespace FlexEngine
{
  // Logs the duration automatically when it goes out of scope
  // Usage: Timer timer("Message");
  class Timer
  {
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<float> duration{ 0 };
    std::string m_log_message;

  public:
    // Constructor for Timer
    // Logs the message and starts the timer
    // Include the message to log when the timer is destroyed
    // Usage: Timer timer("Message");
    Timer(std::string log_message = "")
      : m_log_message(log_message)
    {
      start = std::chrono::steady_clock::now();
    }

    // Logs the message and the duration of the timer
    ~Timer()
    {
      end = std::chrono::steady_clock::now();
      duration = end - start;

      float ms = duration.count() * 1000.0f;
      Log::Debug("[Timer] " + m_log_message + " (" + std::to_string(ms) + "ms)");
    }
  };

}