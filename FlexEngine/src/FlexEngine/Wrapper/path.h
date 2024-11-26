// WLVERSE [https://wlverse.web.app]
// path.h
// 
// Wraps std::filesystem::path to ensure that they're always valid.
// Provides additional functionality to check if the path is safe from a hardcoded list.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include <filesystem>
#include <iostream> // std::ostream, std::istream
#include <string>
#include <exception> // std::invalid_argument
#include <unordered_map>
#include <set>

namespace FlexEngine
{

  namespace Extensions
  {
    // All the extensions that are considered safe
    // Do not use this directly, use the FLX_EXTENSIONS macro instead
    // 
    // Extensions have a "." in front, e.g. ".txt"
    // Categories: "flx", "flb", "data", "shader", "image", "video", "audio", "model"
    __FLX_API extern const std::unordered_map<std::string, std::set<std::string>> safe;

    #pragma region Helper Macros

    // Helper macro to get the safe extensions for a category
    // Categories: "flx", "flb", "data", "shader", "image", "video", "audio", "model"
    // Usage: FLX_EXTENSIONS("category")
    #define FLX_EXTENSIONS(CATEGORY) FlexEngine::Extensions::safe.at(CATEGORY)

    // Helper macro to check if an extension is safe
    // Categories: "flx", "flb", "data", "shader", "image", "video", "audio", "model"
    // Usage: if (FLX_EXTENSIONS_CHECK_SAFETY("image", ".png"))
    #define FLX_EXTENSIONS_CHECK_SAFETY(CATEGORY, EXTENSION) (FLX_EXTENSIONS(CATEGORY).count(EXTENSION) != 0)

    #pragma endregion
  }

  // Wrapper for std::filesystem::path
  // Path is always an absolute path from the root directory
  // This means that the path is always normalized and valid
  // 
  // This is written like a std library class
  // Throws an exception if the path is invalid
  class __FLX_API Path
  {
    std::filesystem::path m_path;

  public:

    // Preferred separator for the current platform
    static constexpr char separator = static_cast<char>(std::filesystem::path::preferred_separator);

    // Invalid characters for paths
    static constexpr char invalid_characters[] = { '<', '>', ':', '"', '/', '\\', '|', '?', '*' };

    // Constructors

    Path() = default;
    Path(const std::filesystem::path& _path);

    // Getter

    std::filesystem::path get() const noexcept;

    #pragma region Extra Queries

    bool is_file() const noexcept;
    bool is_directory() const noexcept;

    bool exists() noexcept;
    static bool exists(const Path& _path) noexcept;
    static Path current();

    #pragma endregion

    #pragma region Passthrough Functions

    #pragma region Concatenation

    Path append(const std::string& _path) const;
    Path& operator/=(const std::string& _path);

    #pragma endregion

    #pragma region Modifiers

    void clear() noexcept;
    //void make_preferred() noexcept;
    //void remove_filename();
    //void replace_filename(const std::string& new_filename);
    //void replace_extension(const std::string& new_extension);
    void swap(Path& other) noexcept;

    #pragma endregion

    #pragma region Format observers

    const std::filesystem::path::value_type* c_str() const noexcept;
    std::string string() const noexcept;
    std::wstring wstring() const noexcept;

    #pragma endregion

    #pragma region Compare

    //int compare(const Path& other) const noexcept;

    #pragma endregion

    #pragma region Generation

    //Path lexically_normal() const;
    //Path lexically_relative(const Path& base) const;
    //Path lexically_proximate(const Path& base) const;

    #pragma endregion

    #pragma region Decomposition

    std::filesystem::path root_name() const;
    std::filesystem::path root_directory() const;
    std::filesystem::path root_path() const;
    std::filesystem::path relative_path() const;
    std::filesystem::path parent_path() const;
    std::filesystem::path filename() const;
    std::filesystem::path stem() const;
    std::filesystem::path extension() const;

    #pragma endregion

    #pragma region Queries

    bool empty() const noexcept;

    // bool has_root_name() const;
    // bool has_root_directory() const;
    // bool has_root_path() const;
    // bool has_relative_path() const;
    // bool has_parent_path() const;
    // bool has_filename() const;
    // bool has_stem() const;
    bool has_extension() const;

    // bool is_absolute() const;
    // bool is_relative() const;

    #pragma endregion

    #pragma region Iterators

    std::filesystem::path::iterator begin() const;
    std::filesystem::path::iterator end() const;

    #pragma endregion

    #pragma endregion

    // Helper Functions

    static Path current(const std::string& path_to_append);

    // Operator Overloads

    operator std::filesystem::path() const noexcept;
    operator std::string() const noexcept;

  };

  // Operator Overloads

  bool operator==(const Path& lhs, const Path& rhs) noexcept;
  bool operator!=(const Path& lhs, const Path& rhs) noexcept;
  bool operator<(const Path& lhs, const Path& rhs) noexcept;
  bool operator>(const Path& lhs, const Path& rhs) noexcept;

  std::ostream& operator<<(std::ostream& os, const Path& p) noexcept;
  std::istream& operator>>(std::istream& is, Path& p) noexcept;

  #pragma region Non-member Passthrough Functions

  #pragma endregion
  
}

namespace std
{
  
  // Specialize std::hash for FlexEngine::Path
  template <>
  struct hash<FlexEngine::Path>
  {
    size_t operator()(const FlexEngine::Path& p) const
    {
      return hash<filesystem::path>()(p.get());
    }
  };

  // Specialize std::equal_to for FlexEngine::Path
  template <>
  struct equal_to<FlexEngine::Path>
  {
    bool operator()(
      const FlexEngine::Path& lhs,
      const FlexEngine::Path& rhs
    ) const
    {
      return lhs == rhs;
    }
  };

  // Overload std::to_string for FlexEngine::Path
  string to_string(const FlexEngine::Path& p);

}
