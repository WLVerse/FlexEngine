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
      Log::Warning("Attempted to read a non-file: " + std::to_string(path));
      return data;
    }

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
      Log::Error("Failed to open file: " + std::to_string(path));
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
      Log::Warning("Attempted to write to a non-file: " + std::to_string(path));
      return;
    }

    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
      Log::Error("Failed to open file: " + std::to_string(path));
      return;
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