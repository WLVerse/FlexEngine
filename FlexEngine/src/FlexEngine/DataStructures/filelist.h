// WLVERSE [https://wlverse.web.app]
// filelist.h
// 
// Specialization of std::vector for Path
// 
// Note that this does not store the files themselves,
// but rather the paths to the files. Use the each() function
// to automatically apply a function to each file in the list.
// 
// Using the composition and passthrough specialization pattern
// Referenced from: https://en.cppreference.com/w/cpp/container/vector
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"
#include "flx_windows.h" // DWORD

#include "Utilities/file.h" // <filesystem> <iostream> <string> <exception> <unordered_map> <set> <fstream>

#include <vector>
#include <functional>

namespace FlexEngine
{

  // Specialization of std::vector for Path
  // 
  // Note that this does not store the files themselves,
  // but rather the paths to the files. Use the each() function
  // to automatically apply a function to each file in the list.
  // 
  // Using the composition and passthrough specialization pattern
  // Referenced from: https://en.cppreference.com/w/cpp/container/vector
  class __FLX_API FileList
  {
    std::vector<Path> m_files;

  public:

    #pragma region Passthrough to std::vector

    #pragma region Type aliases

    using value_type              = std::vector<Path>::value_type;
    using allocator_type          = std::vector<Path>::allocator_type;
    using size_type               = std::vector<Path>::size_type;
    using difference_type         = std::vector<Path>::difference_type;
    using reference               = std::vector<Path>::reference;
    using const_reference         = std::vector<Path>::const_reference;
    using pointer                 = std::vector<Path>::pointer;
    using const_pointer           = std::vector<Path>::const_pointer;
    using iterator                = std::vector<Path>::iterator;
    using const_iterator          = std::vector<Path>::const_iterator;
    using reverse_iterator        = std::vector<Path>::reverse_iterator;
    using const_reverse_iterator  = std::vector<Path>::const_reverse_iterator;

    #pragma endregion

    #pragma region Element access

    reference                     at(size_type index);
    reference                     operator[](size_type index);
    reference                     front();
    reference                     back();
    pointer                       data() noexcept;

    #pragma endregion

    #pragma region Iterators

    iterator                      begin() noexcept;
    const_iterator                cbegin() const noexcept;
    iterator                      end() noexcept;
    const_iterator                cend() const noexcept;
    reverse_iterator              rbegin() noexcept;
    const_reverse_iterator        crbegin() const noexcept;
    reverse_iterator              rend() noexcept;
    const_reverse_iterator        crend() const noexcept;

    #pragma endregion

    #pragma region Capacity

    bool                          empty() const noexcept;
    size_type                     size() const noexcept;
    size_type                     max_size() const noexcept;
    void                          reserve(size_type new_capacity);
    size_type                     capacity() const noexcept;
    void                          shrink_to_fit();

    #pragma endregion

    #pragma region Modifiers

    void                          clear() noexcept;
    iterator                      insert(const_iterator position, const value_type& value);
    iterator                      insert(const_iterator position, value_type&& value);
    iterator                      insert(const_iterator position, size_type count, const value_type& value);
    template <typename InputIt>
    iterator                      insert(const_iterator position, InputIt first, InputIt last);
    iterator                      insert(const_iterator position, std::initializer_list<value_type> ilist);
    template <typename... Args>
    iterator                      emplace(const_iterator position, Args&&... args);
    iterator                      erase(const_iterator position);
    iterator                      erase(const_iterator first, const_iterator last);
    void                          push_back(const value_type& value);
    void                          push_back(value_type&& value);
    template <typename... Args>
    reference                     emplace_back(Args&&... args);
    void                          pop_back();
    void                          resize(size_type count);
    void                          resize(size_type count, const value_type& value);
    void                          swap(FileList& other) noexcept;

    #pragma endregion

    #pragma region Non-member functions

    // friend to allow access to m_files

    friend bool operator==(const FileList& lhs, const FileList& rhs) noexcept;

    #pragma endregion

    #pragma endregion

    // Helper functions

    // Apply a function to each file in the list.
    // This function does NOT read or write to the files,
    // that functionality is left to the function passed in.
    void each(std::function<void(File&)> func);

    // Find all the files that satisfies the predicate.
    // Best used to find files with a certain extension or name.
    // Use each() to find files with a certain content within the file.
    FileList find_if(std::function<bool(const Path&)> predicate);

    // Gets all the files in the directory.
    // Use GetAllFilesInDirectoryRecursively() to get all the files in
    // subdirectories as well.
    static FileList GetAllFilesInDirectory(const Path& directory);

    // This recursively finds all the files in a directory,
    // including all the files in subdirectories.
    static FileList GetAllFilesInDirectoryRecursively(const Path& directory);

    // Browse for a file using the file explorer
    static FileList Browse(
      const std::string& title = "Select a file.",
      const std::string& initial_directory = Path::current(),
      const std::string& file_name = "",
      const wchar_t* filter = L"All Files (*.*)\0*.*\0",
      bool allow_file_create = false,
      bool allow_multi_select = true,
      DWORD max_file_count = 100
    );

  };

}
