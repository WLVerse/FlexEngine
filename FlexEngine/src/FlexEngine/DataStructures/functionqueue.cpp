// WLVERSE [https://wlverse.web.app]
// functionqueue.cpp
//
// A queue for functions that can be run in order of priority. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "functionqueue.h"

namespace FlexEngine
{

  void FunctionQueue::Sort()
  {
    std::sort(m_queue.begin(), m_queue.end());
  }

  void FunctionQueue::Insert(FunctionQueueData data)
  {
    // guard
    if (!data.function) return;

    // insert the function based on its priority
    auto it = std::lower_bound(m_queue.begin(), m_queue.end(), data);
    m_queue.insert(it, data);
  }

  void FunctionQueue::Remove(const std::string& id)
  {
    // guard
    if (m_queue.empty()) return;

    // erase all elements that match the id
    m_queue.erase(std::remove_if(m_queue.begin(), m_queue.end(), [&](const FunctionQueueData& data) { return data.id == id; }), m_queue.end());
  }

  void FunctionQueue::RemoveAndExecute(const std::string& id)
  {
    // guard
    if (m_queue.empty()) return;

    // find the element
    auto it = std::find_if(m_queue.begin(), m_queue.end(), [&](const FunctionQueueData& data) { return data.id == id; });
    if (it != m_queue.end())
    {
      // run the function
      (*it)();
      // remove the element
      m_queue.erase(it);
    }
  }

  void FunctionQueue::Clear()
  {
    m_queue.clear();
  }

  void FunctionQueue::Flush()
  {
    // guard
    if (m_queue.empty()) return;

    // sort by priority
    Sort();
    for (auto& data : m_queue) data();
    m_queue.clear();
  }

#ifdef _DEBUG
  void FunctionQueue::Dump()
  {
    Log::Debug(__FUNCTION__);

    // guard
    if (m_queue.empty())
    {
      Log::Debug("Queue is empty.");
      return;
    }

    // sort by priority
    Sort();
    for (auto& data : m_queue)
    {
      Log::Debug(data);
    }
    Log::Debug("End of dump.");
  }
#endif
}