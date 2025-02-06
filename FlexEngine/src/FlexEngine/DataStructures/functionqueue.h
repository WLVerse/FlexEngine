// WLVERSE [https://wlverse.web.app]
// functionqueue.h
//
// A queue for functions that can be run in order of priority. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include <string>
#include <vector>
#include <functional>
#include <algorithm>

namespace FlexEngine
{

  struct __FLX_API FunctionQueueData
  {
    std::function<void()> function = nullptr;
    std::string id = "";
    int priority = 0;

    FunctionQueueData(std::function<void()> function, std::string id = "", int priority = 0)
      : function(function), id(id), priority(priority)
    {
    }

    void Run() const { function(); }

    // Functor
    void operator()() const { Run(); }

    // Stringify
    std::string ToString() const { return std::string("[") + std::to_string(priority) + "] " + id + ": " + function.target_type().name(); }
    operator std::string() const { return ToString(); }

    // overload < operator for sorting
    bool operator<(const FunctionQueueData& rhs) const { return priority < rhs.priority; }
  };

  class __FLX_API FunctionQueue
  {
    std::vector<FunctionQueueData> m_queue;

  public:

    #pragma region Passthrough Functions

    FunctionQueueData& operator[](size_t index) { return m_queue[index]; }
    const FunctionQueueData& operator[](size_t index) const { return m_queue[index]; }

    FunctionQueueData& at(size_t index) { return m_queue.at(index); }
    const FunctionQueueData& at(size_t index) const { return m_queue.at(index); }

    size_t size() const { return m_queue.size(); }
    bool empty() const { return m_queue.empty(); }

    std::vector<FunctionQueueData>::iterator begin() { return m_queue.begin(); }
    std::vector<FunctionQueueData>::iterator end() { return m_queue.end(); }
    std::vector<FunctionQueueData>::reverse_iterator rbegin() { return m_queue.rbegin(); }
    std::vector<FunctionQueueData>::reverse_iterator rend() { return m_queue.rend(); }
                
    std::vector<FunctionQueueData>::const_iterator begin() const { return m_queue.begin(); }
    std::vector<FunctionQueueData>::const_iterator end() const { return m_queue.end(); }
    std::vector<FunctionQueueData>::const_reverse_iterator rbegin() const { return m_queue.rbegin(); }
    std::vector<FunctionQueueData>::const_reverse_iterator rend() const { return m_queue.rend(); }
                
    std::vector<FunctionQueueData>::const_iterator cbegin() const { return m_queue.cbegin(); }
    std::vector<FunctionQueueData>::const_iterator cend()	const { return m_queue.cend(); }
    std::vector<FunctionQueueData>::const_reverse_iterator crbegin() const { return m_queue.crbegin(); }
    std::vector<FunctionQueueData>::const_reverse_iterator crend() const { return m_queue.crend(); }

    #pragma endregion
    
    // Add a function to the queue.
    // Lowest priority will be run first.
    // Bind Usage: FunctionQueue::Insert({ std::bind(&GameObject::Draw, gameobject), gameobject.GetZIndex() });
    // Lambda Usage: FunctionQueue::Insert({ [&gameobject]() { gameobject.Draw(); }, 0 });
    void Insert(FunctionQueueData data);

    // Removes a function from the queue based on its id.
    // Useful for removing a specific function from the queue when it is no longer needed.
    // For example when an object is already freed by another manager.
    void Remove(const std::string& id);

    // Specialization of Remove().
    // It also runs the function immediately after removing it from the queue.
    void RemoveAndExecute(const std::string& id);

    // Clears the queue.
    // Useful for when you want to clear the queue without running the functions.
    void Clear();

    // Run all the functions in the queue
    // Functions will be run in order of priority from lowest to highest
    // Functions with the same priority will be run in the order they were added
    void Flush();

    // Sorts the queue based on priority.
    // The list should always be sorted, however this is just a safety measure.
    // This is called automatically by Flush().
    void Sort();

#ifdef _DEBUG
  public:
    // For debugging
    // Logs all the function addresses in the queue
    void Dump();
#endif

  };
}