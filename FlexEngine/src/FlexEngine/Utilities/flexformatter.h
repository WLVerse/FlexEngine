// WLVERSE [https://wlverse.web.app]
// flexformatter.h
// 
// Handles the FlexFormat file specification.
// 
// - Use FlexFormatter::Create() to create a new FlxFmtFile.
// - Use FlexFormatter::Parse() to parse a FlxFmtFile from a FlexEngine::File.
// - Use FlxFmtFile.Save() to increment the save version and update the last edited date.
// 
// Example usage:
// FlxFmtFile flxfmtfile_scene = FlexFormatter::Parse(file_scene, FlxFmtFileType::Scene);
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "Utilities/datetime.h" // <iostream> <chrono> <string>
#include "Utilities/file.h" // <filesystem> <iostream> <string> <exception> <unordered_map> <set> <fstream>

#include <sstream>

#include <RapidJSON/document.h>
#include <RapidJSON/istreamwrapper.h>
#include <RapidJSON/ostreamwrapper.h>
#include <RapidJSON/writer.h>
#include <RapidJSON/prettywriter.h>
using namespace rapidjson;

namespace rapidjson
{
  const char* GetParseErrorString(ParseErrorCode code);
}

// Flex Formatter metadata.
// Do not change these values!
#define FLXFMT_NAME     "flxfmt"
#define FLXFMT_VERSION  1

namespace FlexEngine
{

  #pragma region FlxFmtFileType

  // Match this enum with the file extensions in Extensions::safe (path.cpp)
  enum class __FLX_API FlxFmtFileType
  {
    Other = 0,
    Scene,
    Script,
    Prefab,
    Material
  };
  // Lookup the string representation of the FlxFmtFileType enum.
  __FLX_API FlxFmtFileType FlxFmtFileType_Lookup(const std::string& file_type);
  __FLX_API std::string FlxFmtFileType_ReverseLookup(FlxFmtFileType file_type);

  #pragma endregion

  #pragma region FlxFmtMetadata

  // Contains metadata for the FlexFormat file.
  class __FLX_API FlxFmtMetadata
  {
  public:
    using Version = unsigned int;

    std::string format = FLXFMT_NAME;
    Version format_version = FLXFMT_VERSION;
    Date created = Date::Now();
    Date last_edited = Date::Now();
    Version save_version = 0;                           // 0 = version not saved
    FlxFmtFileType file_type = FlxFmtFileType::Other;
  };

  #pragma endregion

  #pragma region FlxFmtFile

  // Used internally to store the FlexFormat file.
  // Wraps around the FlexEngine::File class to provide additional functionality.
  class __FLX_API FlxFmtFile
  {
  public:
    FlxFmtMetadata metadata;
    std::string data = "";

    // Converts into a std::string for writing to a file.
    // Use Save() instead to increment the save version.
    // This is more for debugging purposes.
    std::string ToString() const;

    // Increments the save version and updates the last edited date.
    // Usage: File.Write(FlxFmtFile.Save());
    std::string Save();

    // Null file for error handling.
    static FlxFmtFile Null;

    #pragma region Operator Overloads

    bool operator==(const FlxFmtFile& other) const;
    bool operator!=(const FlxFmtFile& other) const;

    #pragma endregion
  };

  #pragma endregion

  #pragma region FlexFormatter

  // Formatter for the FlexFormat file specification.
  // This is a helper class for reading and writing FlexFormat files.
  class __FLX_API FlexFormatter
  {
  public:
    // Creates a new FlxFmtFile wrapper.
    static FlxFmtFile Create(const std::string& data = "", bool save_version_enabled = false);

    // Parses the format into a FlxFmtFile using RapidJSON.
    // If there is a parse error, an empty FlxFmtFile is returned.
    // Usage: FlxFmtFile flxfmtfile_scene = FlexFormatter::Parse(file_scene, FlxFmtFileType::Scene);
    static FlxFmtFile Parse(FlexEngine::File& file, FlxFmtFileType expected_file_type);
  };

  #pragma endregion

}