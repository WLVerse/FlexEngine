// WLVERSE [https://wlverse.web.app]
// entrypoint.h
// 
// 
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <memory>

#include "flexlogger.h"
#include "application.h"
#include "DataStructures/freequeue.h"

// WinMain for release mode because it doesn't use the console.
#ifdef NDEBUG
//int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  return main(0, nullptr);
}
#endif

// Create an application.
// Use it by creating a new class that inherits from FlexEngine::Application and override the CreateApplication function.
extern FlexEngine::Application* FlexEngine::CreateApplication();

int main(int, char**)
{
  // Enable run-time memory check for debug builds.
#ifdef _DEBUG
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  // Ensure that the FreeQueue is run at the end of the program.
  // This is necessary to ensure that any std::exit calls can be properly handled
  // by the application and logger before the program exits.
  std::atexit(FlexEngine::FreeQueue::Free);

  // Create the logger
  auto log = new FlexEngine::Log();
  FlexEngine::FreeQueue::Push([log]() { delete log; });
  
  // Create the application
  auto app = FlexEngine::CreateApplication();
  FlexEngine::FreeQueue::Push([app]() { delete app; });

  // Run the application
  app->Run();

  // Exit the program
  std::exit(0);
}
