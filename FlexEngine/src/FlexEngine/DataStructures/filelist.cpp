// WLVERSE [https://wlverse.web.app]
// filelist.cpp
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

#include "pch.h"

#include "filelist.h"

namespace FlexEngine
{

  #pragma region Passthrough to std::vector

  #pragma region Element access

  FileList::reference               FileList::at(size_type index) { return m_files.at(index); }
  FileList::reference               FileList::operator[](size_type index) { return m_files[index]; }
  FileList::reference               FileList::front() { return m_files.front(); }
  FileList::reference               FileList::back() { return m_files.back(); }
  FileList::pointer                 FileList::data() noexcept { return m_files.data(); }

  #pragma endregion

  #pragma region Iterators

  FileList::iterator                FileList::begin() noexcept { return m_files.begin(); }
  FileList::const_iterator          FileList::cbegin() const noexcept { return m_files.cbegin(); }
  FileList::iterator                FileList::end() noexcept { return m_files.end(); }
  FileList::const_iterator          FileList::cend() const noexcept { return m_files.cend(); }
  FileList::reverse_iterator        FileList::rbegin() noexcept { return m_files.rbegin(); }
  FileList::const_reverse_iterator  FileList::crbegin() const noexcept { return m_files.crbegin(); }
  FileList::reverse_iterator        FileList::rend() noexcept { return m_files.rend(); }
  FileList::const_reverse_iterator  FileList::crend() const noexcept { return m_files.crend(); }

  #pragma endregion

  #pragma region Capacity

  bool                              FileList::empty() const noexcept { return m_files.empty(); }
  FileList::size_type               FileList::size() const noexcept { return m_files.size(); }
  FileList::size_type               FileList::max_size() const noexcept { return m_files.max_size(); }
  void                              FileList::reserve(size_type new_capacity) { m_files.reserve(new_capacity); }
  FileList::size_type               FileList::capacity() const noexcept { return m_files.capacity(); }
  void                              FileList::shrink_to_fit() { m_files.shrink_to_fit(); }

  #pragma endregion

  #pragma region Modifiers

  void                              FileList::clear() noexcept { m_files.clear(); }
  FileList::iterator                FileList::insert(const_iterator position, const value_type& value) { return m_files.insert(position, value); }
  FileList::iterator                FileList::insert(const_iterator position, value_type&& value) { return m_files.insert(position, value); }
  FileList::iterator                FileList::insert(const_iterator position, size_type count, const value_type& value) { return m_files.insert(position, count, value); }
  template <typename InputIt>
  FileList::iterator                FileList::insert(const_iterator position, InputIt first, InputIt last) { return m_files.insert(position, first, last); }
  FileList::iterator                FileList::insert(const_iterator position, std::initializer_list<value_type> ilist) { return m_files.insert(position, ilist); }
  template <typename... Args>
  FileList::iterator                FileList::emplace(const_iterator position, Args&&... args) { return m_files.emplace(position, args...); }
  FileList::iterator                FileList::erase(const_iterator position) { return m_files.erase(position); }
  FileList::iterator                FileList::erase(const_iterator first, const_iterator last) { return m_files.erase(first, last); }
  void                              FileList::push_back(const value_type& value) { m_files.push_back(value); }
  void                              FileList::push_back(value_type&& value) { m_files.push_back(value); }
  template <typename... Args>
  FileList::reference               FileList::emplace_back(Args&&... args) { return m_files.emplace_back(args...); }
  void                              FileList::pop_back() { m_files.pop_back(); }
  void                              FileList::resize(size_type count) { m_files.resize(count); }
  void                              FileList::resize(size_type count, const value_type& value) { m_files.resize(count, value); }
  void                              FileList::swap(FileList& other) noexcept { m_files.swap(other.m_files); }

  #pragma endregion

  #pragma region Non-member functions

  bool operator==(const FileList& lhs, const FileList& rhs) noexcept { return lhs.m_files == rhs.m_files; }

  #pragma endregion

  #pragma endregion

  #pragma region Helper functions

  void FileList::each(std::function<void(File&)> func)
  {
    for (auto& path : m_files)
    {
      File& file = FlexEngine::File::Open(path);
      func(file);
    }
  }

  FileList FileList::find_if(std::function<bool(const Path&)> predicate)
  {
    FileList found;

    for (auto& file : m_files)
    {
      if (predicate(file))
      {
        found.push_back(file);
      }
    }

    return found;
  }

  FileList FileList::GetAllFilesInDirectory(const Path& directory)
  {
    // guard
    if (!directory.is_directory())
    {
      Log::Error("Attempted to get files from a non-directory: " + directory.string());
      return {};
    }

    FileList files;

    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
      // guard
      if (entry.is_directory()) continue;

      files.push_back({ entry.path() });
    }

    return files;
  }

  FileList FileList::GetAllFilesInDirectoryRecursively(const Path& directory)
  {
    // guard
    if (!directory.is_directory())
    {
      Log::Error("Attempted to get files from a non-directory: " + directory.string());
      return {};
    }

    FileList files;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory))
    {
      // guard
      if (entry.is_directory()) continue;

      files.push_back({ entry.path() });
    }

    return files;
  }

  FileList FileList::Browse(
    const std::string& title,
    const std::string& initial_directory,
    const std::string& file_name,
    const wchar_t* filter,
    bool allow_file_create,
    bool allow_multi_select,
    DWORD max_file_count
  )
  {
    // truncate all strings to 260 characters
    std::wstring title_w = std::wstring(title.begin(), title.end());
    std::wstring initial_directory_w = std::wstring(initial_directory.begin(), initial_directory.end());
    std::wstring file_name_w = std::wstring(file_name.begin(), file_name.end());

    // Initialize OPENFILENAME structure
    OPENFILENAME ofn = { 0 };

    // Sanitize input
    if (filter == nullptr) filter = L"All Files (*.*)\0*.*\0";
    if (!allow_multi_select || max_file_count == 0) max_file_count = 1;
    else if (max_file_count > 100) max_file_count = 100;

    // Initialize buffer for file name
    
    // The buffer is limited to 100 MAX_PATH files, which usually actually allows for more files
    // 0.5f is an arbitrary factor, since most file names are shorter than MAX_PATH
    DWORD file_name_buffer_size = static_cast<DWORD>(max_file_count * MAX_PATH * 0.5f);

    // Allocate buffer on the heap because it can be quite large
    wchar_t* file_name_buffer = new wchar_t[file_name_buffer_size];
    ZeroMemory(file_name_buffer, file_name_buffer_size * sizeof(wchar_t));
    wcscpy_s(file_name_buffer, file_name_buffer_size, file_name_w.c_str());

    wchar_t initial_dir[MAX_PATH] = { 0 };
    wcscpy_s(initial_dir, initial_directory_w.c_str());

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrTitle = title_w.c_str();
    ofn.lpstrInitialDir = initial_dir;
    ofn.lpstrFile = file_name_buffer;
    ofn.nMaxFile = file_name_buffer_size;
    ofn.lpstrFilter = filter;

    ofn.Flags = OFN_EXPLORER;

    if (!allow_file_create) ofn.Flags |= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    // Disable multi-select if file creation is allowed
    else allow_multi_select = false;

    if (allow_multi_select) ofn.Flags |= OFN_ALLOWMULTISELECT;

    FileList files;

    // Show the dialog and get the selected file(s)
    if (GetOpenFileName(&ofn))
    {
      // Convert the buffer to a string
      std::wstring file_name_buffer_w(file_name_buffer);

      // Check if multiple files were selected
      if (allow_multi_select && file_name_buffer_w.find_first_of(L'\0') != std::wstring::npos)
      {
        // The first string is the directory
        // The rest are the files
        // Example: C:\\selected\\directorypath\0file1.txt\0file2.txt\0file3.txt\0\0
        std::vector<std::wstring> file_names;
        size_t start = 0;
        size_t end = file_name_buffer_w.find_first_of(L'\0');
        while (end != std::wstring::npos)
        {
          file_names.push_back(file_name_buffer_w.substr(start, end - start));
          start = end + 1;
          end = file_name_buffer_w.find_first_of(L'\0', start);
        }

        // Get the files
        for (size_t i = 1; i < file_names.size(); ++i)
        {
          files.push_back(Path(file_names[0] + file_names[i]));
        }
      }
      else
      {
        // Single file was selected
        files.push_back(Path(file_name_buffer_w));
      }
    }

    // Cleanup
    delete[] file_name_buffer;

    return files;
  }

  #pragma endregion

}
