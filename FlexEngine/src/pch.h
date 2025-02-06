// WLVERSE [https://wlverse.web.app]
// pch.h
//
// FlexEngine: Pre-compiled Header File 
// https://en.cppreference.com/w/cpp/header 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <cstdint>                              // Fixed-size integer types

#include <iostream>                             // Several standard stream objects
#include <string>                               // std::basic_string class template
#include <sstream>                              // std::basic_stringstream, std::basic_istringstream, std::basic_ostringstream class templates and several typedefs
#include <fstream>                              // Input/output stream class to operate on files
#include <filesystem>                           // Interface to the file system

#include <array>                                // std::array container
#include <vector>                               // std::vector container
#include <list>                                 // std::list container
#include <deque>                                // std::deque container
#include <queue>                                // std::queue, std::priority_queue container adaptors
#include <set>                                  // std::set and std::multiset associative containers
#include <map>                                  // std::map and std::multimap associative containers
#include <unordered_set>                        // std::unordered_set and std::unordered_multiset unordered associative containers
#include <unordered_map>                        // std::unordered_map and std::unordered_multimap unordered associative containers

#include <variant>                              // Store types from a fixed set of types with type-safe access

#include <memory>                               // High-level memory management utilities
#include <utility>                              // Various utility components
#include <algorithm>                            // Algorithms that operate on ranges
#include <functional>                           // Function objects, Function invocations, Bind operations and Reference wrappers

#include "FlexEngine/DataStructures/filelist.h" // File manager for handling files and directories
                                                // <filesystem> <iostream> <string> <exception> <unordered_map> <set> <fstream>
#include "FlexEngine/Utilities/datetime.h"      // Date and time utilities
                                                // <iostream> <chrono> <string>
#include "FlexEngine/flexassert.h"              // Wrapper for cassert with additional functionality
                                                // FLX_INTERNAL_ASSERT, FLX_CORE_ASSERT, FLX_ASSERT, FLX_NULLPTR_ASSERT

#include "FlexEngine/flexlogger.h"              // Logging class for debugging and error handling
                                                // <filesystem> <fstream> <string>
#include "FlexEngine/Utilities/timer.h"         // Scoped timer to measure time taken by functions to execute
                                                // <iostream> <chrono>