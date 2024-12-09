#include "path.h"

namespace FlexEngine
{

  namespace Extensions
  {
    // Match these file extensions with the FlxFmtFileType enum (flexformatter.h)
    const std::unordered_map<std::string, std::set<std::string>> safe =
    {
      // FlexEngine files
      { "flx", { ".flxscene", ".flxscript", ".flxprefab", ".flxmaterial" } },
      // FlexEngine binary files
      { "flb", { ".flbscene", ".flbscript", ".flbprefab", ".flbmaterial" } },
      // Data files (txt, json, etc.)
      { "data", { ".txt", ".json", ".csv", ".yaml", ".ini", ".cfg", ".log", ".dat" } },
      // Shader files (glsl, frag, etc.)
      { "shader", { ".glsl", ".hlsl", ".frag", ".vert" }},
      // Image files (jpg, png, etc.)
      { "image", { ".jpg", ".jpeg", ".png" } },
      // Video files (mp4, avi, etc.)
      { "video", { ".mp4" } },
      // Audio files (mp3, wav, etc.)
      { "audio", { ".mp3", ".wav", ".ogg", ".flac" } },
      // Model files (obj, fbx, etc.)
      { "model", { ".obj", ".mtl", ".fbx", ".gltf", ".glb" } },
    };
  }

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

    // Check if the file is safe/supported
    if (normalized_path.has_extension())
    {
      // Get the extension
      std::string extension = normalized_path.extension().string();

      // Check if the extension is safe
      bool is_safe = false;
      for (const auto& [category, extensions] : Extensions::safe)
      {
        if (extensions.count(extension) != 0)
        {
          is_safe = true;
          break;
        }
      }

      // Throw an exception if the extension is not safe
      if (!is_safe) throw std::invalid_argument("Unsupported file extension: " + extension);
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
    return std::filesystem::current_path() / path_to_append;
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
