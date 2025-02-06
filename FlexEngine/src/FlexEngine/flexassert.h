// WLVERSE [https://wlverse.web.app]
// flexassert.h
//
// This file contains the assertion macros used in the FlexEngine. 
// These don't work like the standard assert.h, but safely exits the application
// using log fatal. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flexlogger.h" // <iostream> <string>

// asserts before the application is functional
#define FLX_INTERNAL_ASSERT(EXPRESSION, MESSAGE) {                                   \
  if (!(EXPRESSION))                                                                 \
  {                                                                                  \
    std::cerr << "[FLX_INTERNAL_ASSERT] Assertion Failed: " << MESSAGE << std::endl; \
    std::exit(EXIT_FAILURE);                                                         \
  }                                                                                  \
}

// asserts for the application core
#define FLX_CORE_ASSERT(EXPRESSION, MESSAGE) {                                             \
  if (!(EXPRESSION))                                                                       \
  {                                                                                        \
    FlexEngine::Log::Fatal(std::string("[FLX_CORE_ASSERT] Assertion Failed: ") + MESSAGE); \
  }                                                                                        \
}

// asserts for regular use
#define FLX_ASSERT(EXPRESSION, MESSAGE) {                                             \
  if (!(EXPRESSION))                                                                  \
  {                                                                                   \
    FlexEngine::Log::Fatal(std::string("[FLX_ASSERT] Assertion Failed: ") + MESSAGE); \
  }                                                                                   \
}

// asserts specifically for checking pointers
#define FLX_NULLPTR_ASSERT(POINTER, MESSAGE) {                                                \
  if (POINTER == nullptr)                                                                     \
  {                                                                                           \
    FlexEngine::Log::Fatal(std::string("[FLX_NULLPTR_ASSERT] Assertion Failed: ") + MESSAGE); \
  }                                                                                           \
}