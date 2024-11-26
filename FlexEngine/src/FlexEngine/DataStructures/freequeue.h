// WLVERSE [https://wlverse.web.app]
// freequeue.h
// 
// 
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "functionqueue.h"

namespace FlexEngine
{

  // Specialization of FunctionQueue
  // Queues functions for freeing anything that needs to be freed
  // 
  // This is designed for objects that should be freed before the program ends to prevent memory leaks.
  class __FLX_API FreeQueue
  {
    static FunctionQueue m_queue;

  public:
    // static class
    FreeQueue() = delete;
    FreeQueue(const FreeQueue&) = delete;
    FreeQueue(FreeQueue&&) = delete;
    FreeQueue& operator=(const FreeQueue&) = delete;
    FreeQueue& operator=(FreeQueue&&) = delete;

    // Push a freeing function to the queue when the object is created
    // Use an id if you want to remove the function from the queue later
    // Usage: FreeQueue::Push([&obj]() { delete obj; }, "Object");
    static void Push(const std::function<void()>& function, const std::string& id = "");

    // Removes all functions from the queue based on its id.
    // Useful for removing a specific function from the free queue when it is no longer needed,
    // for example when an object is already freed by another manager.
    static void Remove(const std::string& id);

    // Specialization of Remove().
    // It also runs the function immediately after removing it from the queue.
    static void RemoveAndExecute(const std::string& id);
    
    // Runs all the freeing functions in the queue
    static void Free();

#ifdef _DEBUG
    // passthrough
    static void Dump();
#endif
  };

}