// WLVERSE [https://wlverse.web.app]
// file.h
// 
// Provides a registry and management functions for files.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "path.h" // <filesystem> <iostream> <string> <exception> <unordered_map> <set>

#include <fstream>

namespace FlexEngine
{

  // Structure to represent a file
  // Files are defined by their path and data
  // There can never be two files with the same path
  // This is enforced by the FileRegistry
  // Outside of the class, all the files are handled by references to the FileRegistry
  class __FLX_API File
  {
    #pragma region File Registry

    // File Registry
    // Keeps track of all files, making sure there are no duplicates
    using FileRegistry = std::unordered_map<Path, File>;
    static FileRegistry s_file_registry;
    #define FILE_REGISTRY s_file_registry

    #pragma endregion

  public:
    Path path;
    std::string data;

    #pragma region File Registry Management Functions

    // Adds a file to the registry if it doesn't exist
    // Otherwise, returns the existing file
    // Usage: File& file_txt = File::Open("path/to/file.txt");
    static File& Open(const Path& path);

    // Removes a file from the registry
    // This doesn't have to be called at all
    static void Close(const Path& path);

    #pragma endregion

    #pragma region File Management Functions

    std::string Read();
    void Write(const std::string& _data);

    // Use Open() to get the file
    static Path Create(const Path& path, const std::string& filename);

    // Note: No support for deleting directories because it's dangerous
    void Delete();

    #pragma endregion
    
    #pragma region Operator Overloads

    operator std::filesystem::path() const;

    #pragma endregion

  };

  #pragma region Non-member Operator Overloads

  bool operator==(const File& lhs, const File& rhs);
  bool operator!=(const File& lhs, const File& rhs);
  bool operator<(const File& lhs, const File& rhs);
  bool operator>(const File& lhs, const File& rhs);

  #pragma endregion

}
