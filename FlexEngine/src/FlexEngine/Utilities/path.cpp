// WLVERSE [https://wlverse.web.app]
// path.cpp
// 
// Wrapper for std::filesystem::path
// Path is always an absolute path from the root directory
// This means that the path is always normalized and valid
// 
// This is written like a std library class
// Throws an exception if the path is invalid
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "path.h"

namespace FlexEngine
{
  std::filesystem::path Path::m_srcpath = std::filesystem::current_path();

  #pragma region Constructors

  // Constructor to ensure validity of the path
  // Throws an exception if the path is invalid
  Path::Path(const std::filesystem::path& path)
  {
    // Normalize the path
    std::filesystem::path normalized_path = std::filesystem::absolute(path);

    // Check if the new path is valid, if not, throw an exception
    if (!std::filesystem::exists(normalized_path.root_path()))
    {
      throw std::invalid_argument("Invalid file path");
    }

    m_path = normalized_path;
  }

  #pragma endregion

  #pragma region Getter

  std::filesystem::path Path::get() const noexcept
  {
    return m_path;
  }

  #pragma endregion

  #pragma region Extra Queries

  bool Path::is_file() const noexcept
  {
    // check if it has a . in the filename
    return m_path.has_extension();
  }

  bool Path::is_directory() const noexcept
  {
    // check if it has a . in the filename
    return !m_path.has_extension();
    //return std::filesystem::is_directory(m_path);
  }

  bool Path::exists() noexcept
  {
    return std::filesystem::exists(*this);
  }

  bool Path::exists(const Path& path) noexcept
  {
    return std::filesystem::exists(path);
  }

  Path Path::current()
  {
    return std::filesystem::current_path();
  }

  #pragma endregion

  #pragma region Passthrough Functions

  #pragma region Concatenation

  Path Path::append(const std::string& _path) const
  {
    return m_path / _path;
  }
  
  Path& Path::operator/=(const std::string& _path)
  {
    m_path /= _path;
    return *this;
  }

  #pragma endregion

  #pragma region Modifiers

  void Path::clear() noexcept
  {
    m_path.clear();
  }

  void Path::swap(Path& other) noexcept
  {
    m_path.swap(other.m_path);
  }

  #pragma endregion

  #pragma region Format observers

  const std::filesystem::path::value_type* Path::c_str() const noexcept
  {
    return m_path.c_str();
  }

  std::string Path::string() const noexcept
  {
    return m_path.string();
  }

  std::wstring Path::wstring() const noexcept
  {
    return m_path.wstring();
  }

  #pragma endregion

  #pragma region Decomposition

  std::filesystem::path Path::root_name() const
  {
    return m_path.root_name();
  }

  std::filesystem::path Path::root_directory() const
  {
    return m_path.root_directory();
  }

  std::filesystem::path Path::root_path() const
  {
    return m_path.root_path();
  }

  std::filesystem::path Path::relative_path() const
  {
    return m_path.relative_path();
  }

  std::filesystem::path Path::parent_path() const
  {
    return m_path.parent_path();
  }

  std::filesystem::path Path::filename() const
  {
    return m_path.filename();
  }

  std::filesystem::path Path::stem() const
  {
    return m_path.stem();
  }

  std::filesystem::path Path::extension() const
  {
    return m_path.extension();
  }

  #pragma endregion

  #pragma region Queries

  bool Path::empty() const noexcept
  {
    return m_path.empty();
  }

  bool Path::has_extension() const
  {
    return m_path.has_extension();
  }

  #pragma endregion

  #pragma region Iterators

  std::filesystem::path::iterator Path::begin() const
  {
    return m_path.begin();
  }

  std::filesystem::path::iterator Path::end() const
  {
    return m_path.end();
  }

  #pragma endregion

  #pragma endregion

  #pragma region Non-member Passthrough Functions

  #pragma endregion

  #pragma region Helper Functions

  Path Path::current(const std::string& path_to_append)
  {
    if (path_to_append.empty())
    {
      return std::filesystem::current_path();
    }

    // Remove the first character if it is a slash
    std::string path = path_to_append;
    if (path[0] == '/' || path[0] == '\\')
    {
      path.erase(0, 1);
    }

    return std::filesystem::absolute(std::filesystem::current_path() / path);
  }

  Path Path::source(const std::string& path_to_append)
  {
    if (path_to_append.empty())
    {
      return m_srcpath;
    }

    // Remove the first character if it is a slash
    std::string path = path_to_append;
    if (path[0] == '/' || path[0] == '\\')
    {
      path.erase(0, 1);
    }

    return std::filesystem::absolute(m_srcpath / path);
  }

  #pragma endregion

  #pragma region Operator Overloads

  Path::operator std::filesystem::path() const noexcept
  {
    return m_path;
  }

  Path::operator std::string() const noexcept
  {
    return m_path.string();
  }

  bool operator==(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.get() == rhs.get();
  }

  bool operator!=(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.get() != rhs.get();
  }

  bool operator<(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.get() < rhs.get();
  }

  bool operator>(const Path& lhs, const Path& rhs) noexcept
  {
    return lhs.get() > rhs.get();
  }

  std::ostream& operator<<(std::ostream& os, const Path& path) noexcept
  {
    return os << path.get();
  }

  std::istream& operator>>(std::istream& is, Path& path) noexcept
  {
    std::filesystem::path _path;
    is >> _path;
    path = _path;
    return is;
  }

  #pragma endregion

}

namespace std
{
  // Specialize std::to_string for FlexEngine::FilePath
  string to_string(const FlexEngine::Path& p)
  {
    return p.get().string();
  }
}
