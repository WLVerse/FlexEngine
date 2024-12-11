// WLVERSE [https://wlverse.web.app]
// flexformatter.cpp
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

#include "pch.h"

#include "flexformatter.h"

namespace rapidjson
{
  const char* GetParseErrorString(ParseErrorCode code)
  {
    switch (code)
    {
    case rapidjson::kParseErrorNone: return "No error";
    case rapidjson::kParseErrorDocumentEmpty: return "The document is empty";
    case rapidjson::kParseErrorDocumentRootNotSingular: return "The document root must not follow by other values";
    case rapidjson::kParseErrorValueInvalid: return "Invalid value";
    case rapidjson::kParseErrorObjectMissName: return "Missing a name for object member";
    case rapidjson::kParseErrorObjectMissColon: return "Missing a colon after a name of object member";
    case rapidjson::kParseErrorObjectMissCommaOrCurlyBracket: return "Missing a comma or '}' after an object member";
    case rapidjson::kParseErrorArrayMissCommaOrSquareBracket: return "Missing a comma or ']' after an array element";
    case rapidjson::kParseErrorStringUnicodeEscapeInvalidHex: return "Incorrect hex digit after \\u escape in string";
    case rapidjson::kParseErrorStringUnicodeSurrogateInvalid: return "The surrogate pair in string is invalid";
    case rapidjson::kParseErrorStringEscapeInvalid: return "Invalid escape character in string";
    case rapidjson::kParseErrorStringMissQuotationMark: return "Missing a quotation mark in string";
    case rapidjson::kParseErrorStringInvalidEncoding: return "Invalid encoding in string";
    case rapidjson::kParseErrorNumberTooBig: return "Number too big to be stored in double";
    case rapidjson::kParseErrorNumberMissFraction: return "Missing fraction part in number";
    case rapidjson::kParseErrorNumberMissExponent: return "Missing exponent in number";
    case rapidjson::kParseErrorTermination: return "Parsing was terminated";
    case rapidjson::kParseErrorUnspecificSyntaxError: return "Unspecific syntax error";
    default: return "Unknown parsing error";
    }
  }
}

namespace FlexEngine
{

  #pragma region FlxFmtFileType

  FlxFmtFileType FlxFmtFileType_Lookup(const std::string& file_type)
  {
         if (file_type == "scene")      return FlxFmtFileType::Scene;
    else if (file_type == "script")     return FlxFmtFileType::Script;
    else if (file_type == "prefab")     return FlxFmtFileType::Prefab;
    else if (file_type == "material")   return FlxFmtFileType::Material;
    else                                return FlxFmtFileType::Other;
  }

  std::string FlxFmtFileType_ReverseLookup(FlxFmtFileType file_type)
  {
    switch (file_type)
    {
    case FlxFmtFileType::Scene:     return "scene";
    case FlxFmtFileType::Script:    return "script";
    case FlxFmtFileType::Prefab:    return "prefab";
    case FlxFmtFileType::Material:  return "material";
    default:                        return "other";
    }
  }

  #pragma endregion

  #pragma region FlxFmtFile

  // static member initialization
  FlxFmtFile FlxFmtFile::Null = FlxFmtFile();

  std::string FlxFmtFile::ToString() const
  {
    std::string out;
    out = std::string("{") +
      R"("format":)"          + R"(")" + metadata.format                                  + R"(")" + R"(,)" +
      R"("format_version":)"           + std::to_string(metadata.format_version)                   + R"(,)" +
      R"("created":)"         + R"(")" + metadata.created.ToString()                      + R"(")" + R"(,)" +
      R"("last_edited":)"     + R"(")" + metadata.last_edited.ToString()                  + R"(")" + R"(,)" +
      R"("save_version":)"             + std::to_string(metadata.save_version)                     + R"(,)" +
      R"("data":)"            + R"([)" + data                                             + R"(])"
      "}"
    ;
    return out;
  }

  std::string FlxFmtFile::Save()
  {
    if (metadata.save_version != 0) metadata.save_version++;
    metadata.last_edited = Date::Now();
    return ToString();
  }

  #pragma region Operator Overloads

  bool FlxFmtFile::operator==(const FlxFmtFile& other) const
  {
    return
      metadata.format           == other.metadata.format &&
      metadata.format_version   == other.metadata.format_version &&
      metadata.created          == other.metadata.created &&
      metadata.last_edited      == other.metadata.last_edited &&
      metadata.save_version     == other.metadata.save_version &&
      metadata.file_type        == other.metadata.file_type &&
      data                      == other.data
    ;
  }

  bool FlxFmtFile::operator!=(const FlxFmtFile& other) const
  {
    return !(operator==(other));
  }

  #pragma endregion

  #pragma endregion


  #pragma region FlexFormatter

  FlxFmtFile FlexFormatter::Create(const std::string& data, bool save_version_enabled)
  {
    FlxFmtFile file;
    file.metadata.save_version = ((save_version_enabled) ? 1 : 0);
    file.data = data;
    return file;
  }

  FlxFmtFile FlexFormatter::Parse(FlexEngine::File& file, FlxFmtFileType expected_file_type)
  {
    // read file into file.data
    file.Read();

    // guard: wrong file extension
    // only .flx files are supported
    std::string extension = file.path.extension().string();

    if (extension.substr(0, 4) != ".flx")
    {
      Log::Warning("Unsupported file extension: " + file.path.string());
      return FlxFmtFile::Null;
    }

    // get file type from extension
    FlxFmtFileType file_type = FlxFmtFileType::Other;
    extension = extension.substr(4);
    file_type = FlxFmtFileType_Lookup(extension);

    // guard: wrong file type
    if (file_type != expected_file_type)
    {
      Log::Warning("File type mismatch: " + extension);
      Log::Warning("Expected:           " + FlxFmtFileType_ReverseLookup(expected_file_type));
      return FlxFmtFile::Null;
    }

    // guard: empty file
    if (file.data.empty())
    {
      Log::Warning("Empty file: " + file.path.string());
      return FlxFmtFile::Null;
    }

    // parse file into document
    Document document;
    document.Parse(file.data.c_str());

    // check for parse errors
    // if there is an error, return an empty rapidjson::Document
    if (document.HasParseError())
    {
      Log::Error(std::string("Parse error while parsing file into FlxFmtFile: ") + GetParseErrorString(document.GetParseError()));
      return FlxFmtFile::Null;
    }

    // format checking

#if FLXFMT_VERSION == 1

    // metadata validity check
    // format check
    // format version check
    // TODO: versioning updates

    if (
      !document.HasMember("format") ||
      !document.HasMember("format_version") ||
      !document.HasMember("created") ||
      !document.HasMember("last_edited") ||
      !document.HasMember("save_version")
    )
    {
      Log::Warning("Missing metadata in file.");
      return FlxFmtFile::Null;
    }

    std::string format = document["format"].GetString();
    if (format != FLXFMT_NAME)
    {
      Log::Warning("File format mismatch: " + format);
      return FlxFmtFile::Null;
    }

    int format_version = document["format_version"].GetInt();

    if (format_version != FLXFMT_VERSION)
    {
      Log::Info("File format version mismatch: " + std::to_string(format_version));
      Log::Info("Current version:              " + std::to_string(FLXFMT_VERSION));
      if (format_version > FLXFMT_VERSION)
      {
        Log::Warning("Forward compatibility is not supported.");
        return FlxFmtFile::Null;
      }
      else
      {
        Log::Warning("Backward compatibility is not supported.");
        return FlxFmtFile::Null;
      }
    }
#else
    #error "Unsupported FLXFMT_VERSION"
#endif

    // save metadata

    FlxFmtFile flxfmtfile = FlxFmtFile::Null;

    if (format_version == 1)
    {
      flxfmtfile.metadata.format = format;
      flxfmtfile.metadata.format_version = format_version;
      std::string created = document["created"].GetString();
      flxfmtfile.metadata.created = Date(
        std::stoi(created.substr(0, 4)),
        std::stoi(created.substr(5, 2)),
        std::stoi(created.substr(8, 2))
      );
      std::string last_edited = document["last_edited"].GetString();
      flxfmtfile.metadata.last_edited = Date(
        std::stoi(last_edited.substr(0, 4)),
        std::stoi(last_edited.substr(5, 2)),
        std::stoi(last_edited.substr(8, 2))
      );
      flxfmtfile.metadata.save_version = document["save_version"].GetInt();
      flxfmtfile.metadata.file_type = file_type;

      // Find the first [ and last ] and extract the string in between.
      // This is a temporary hack to get the data since the data is not a string.
      // 
      // This requires that the data is the only element in the "data" array.
      // Which is usually the case since the data only stores one thing.
      // 
      // flxfmtfile.data is thus a string representation of the json object which
      // can be parsed using rapidjson into an object.
      std::size_t first_bracket = file.data.find_first_of('[');
      std::size_t last_bracket = file.data.find_last_of(']');
      flxfmtfile.data = file.data.substr(first_bracket + 1, last_bracket - first_bracket - 1);
    }

    return flxfmtfile;
  }

  #pragma endregion

}