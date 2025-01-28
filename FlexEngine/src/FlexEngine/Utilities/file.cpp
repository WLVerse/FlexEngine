// WLVERSE [https://wlverse.web.app]
// file.cpp
// 
// Structure to represent a file
// Files are defined by their path and data
// There can never be two files with the same path
// This is enforced by the FileRegistry
// Outside of the class, all the files are handled by references to the FileRegistry
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "pch.h"

#include "file.h"

namespace FlexEngine
{

  // static member initialization
  File::FileRegistry File::s_file_registry;

  #pragma region File Registry Management Functions

  File& File::Open(const Path& path)
  {
    // check if the file already exists
    if (FILE_REGISTRY.count(path) == 0)
    {
      // create a new file
      FILE_REGISTRY[path] = { path, "" };

      // file reading should be handled by the user
      // this is to prevent reading files that are not needed
    }
    return FILE_REGISTRY[path];
  }

  void File::Close(const Path& path)
  {
    // check if the file exists
    if (FILE_REGISTRY.count(path) != 0)
    {
      // delete the file
      FILE_REGISTRY.erase(path);
    }
  }

  #pragma endregion

  #pragma region File Management Functions

  std::string File::Read()
  {
    FLX_FLOW_FUNCTION();
    FLX_SCOPED_TIMER(__FUNCTION__ + std::string(" ") + std::to_string(path));

    // guard
    if (!path.is_file())
    {
      Log::Warning(
        "Attempted to read a non-file. This is probably a folder/directory. "
        "Path:" + std::to_string(path)
      );
      return data;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
      Log::Error("The file did not open while trying to read it. " + std::to_string(path));
      return data;
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // Read the entire contents of the file into a string
    data.resize(size);
    file.read(data.data(), size);

    if (file.fail())
    {
      Log::Error("Failed to read file: " + std::to_string(path));
      return data = ""; // return an empty string in case data was partially read/corrupted
    }

    return data;
  }

  void File::Write(const std::string& _data)
  {
    FLX_FLOW_FUNCTION();
    FLX_SCOPED_TIMER(__FUNCTION__ + std::string(" ") + std::to_string(path));

    // guard
    if (!path.is_file())
    {
      Log::Warning("Attempted to write to a non-file. Path: " + std::to_string(path));
      return;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
      Log::Info("Creating file: " + std::to_string(path));
      auto new_path = Create(path.parent_path(), path.filename().string()); // this is unsafe
      file.open(new_path, std::ios::binary);
    }

    data = _data;
    file << data;

    if (file.fail())
    {
      Log::Error("Failed to write to file: " + std::to_string(path));
      return;
    }

#ifdef _DEBUG
    Log::Debug("Successfully wrote to file: " + std::to_string(path));
#endif
  }

  Path File::Create(const Path& path, const std::string& filename)
  {
    FLX_FLOW_FUNCTION();
    FLX_SCOPED_TIMER(__FUNCTION__ + std::string(" ") + std::to_string(path) + Path::separator + filename);

    // guard: path is not a directory
    if (!path.is_directory())
    {
      Log::Warning("Attempted to create a file in a non-directory. Path: " + std::to_string(path));
      return path;
    }

    // guard: filename
    if (filename.empty() || filename.find_first_of(Path::invalid_characters) != std::string::npos)
    {
      Log::Warning(
        "Attempted to create a file with an invalid filename. "
        "Path: " + std::to_string(path) + Path::separator + filename
      );
      return path;
    }

    // guard: length
    if ((path.string().length() + filename.length()) >= 260)
    {
      Log::Warning(
        "Attempted to create a file with a filename that is too long. "
        "Path" + std::to_string(path) + Path::separator + filename +
        " (" + std::to_string(path.string().length() + filename.length()) + ")"
      );
      return path;
    }

    // create the file
    Path new_path = path.get() / filename;
    std::ofstream file(new_path, std::ios::binary);
    if (!file.is_open())
    {
      Log::Error("Failed to create file: " + std::to_string(new_path));
      return path;
    }
    file.close();

    // double check
    if (!new_path.exists())
    {
      Log::Error("Failed to create file: " + std::to_string(new_path));
      return path;
    }

#ifdef _DEBUG
    Log::Debug("Successfully created file: " + std::to_string(new_path));
#endif

    return new_path;
  }

  void File::Delete()
  {
    FLX_FLOW_FUNCTION();
    FLX_SCOPED_TIMER(__FUNCTION__ + std::string(" ") + std::to_string(path));

    // guard
    if (!path.is_file())
    {
      Log::Warning("Attempted to delete a non-file: " + std::to_string(path));
      return;
    }

    std::error_code errorcode;
    std::filesystem::remove(path, errorcode);

    if (errorcode)
    {
      Log::Error("Failed to delete file: " + std::to_string(path));
      return;
    }

#ifdef _DEBUG
    Log::Debug("Successfully deleted file: " + std::to_string(path));
#endif
  }

  #pragma endregion

  #pragma region Operator Overloads

  File::operator std::filesystem::path() const { return path; }

  bool operator==(const File& lhs, const File& rhs) { return lhs.path == rhs.path; }
  bool operator!=(const File& lhs, const File& rhs) { return lhs.path != rhs.path; }
  bool operator<(const File& lhs, const File& rhs) { return lhs.path < rhs.path; }
  bool operator>(const File& lhs, const File& rhs) { return lhs.path > rhs.path; }

  #pragma endregion


}
