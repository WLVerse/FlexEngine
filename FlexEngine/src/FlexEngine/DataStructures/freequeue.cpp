// WLVERSE [https://wlverse.web.app]
// freequeue.cpp
// 
// Specialization of FunctionQueue
// Queues functions for freeing anything that needs to be freed
// 
// This is designed for objects that should be freed before the program ends to prevent memory leaks.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "freequeue.h"

namespace FlexEngine
{

  // static member initialization
  FunctionQueue FreeQueue::m_queue;

  void FreeQueue::Push(const std::function<void()>& function, const std::string& id)
  {
    m_queue.Insert(
      {
        function,
        id,
        -static_cast<int>(m_queue.size())
      }
    );
  }

  void FreeQueue::Remove(const std::string& id)
  {
    m_queue.Remove(id);
  }

  void FreeQueue::RemoveAndExecute(const std::string& id)
  {
    m_queue.RemoveAndExecute(id);
  }

  void FreeQueue::Free()
  {
    m_queue.Flush();
  }

#ifdef _DEBUG
  void FreeQueue::Dump()
  {
    m_queue.Dump();
  }
#endif

}