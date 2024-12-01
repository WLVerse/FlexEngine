// WLVERSE [https://wlverse.web.app]
// flxdata.cpp
// 
// For now, to create file need to use like this...
// Asset::FlxData flex("/data/flxer.flxdata");
// flex.SetBool("test", true);
// flex.SetString("tester", "Hello2 World!");
// flex.SetInt("testint", 1243);
// flex.Save();
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "pch.h" // Logger
#include "flxdata.h"
#include "FlexEngine/flexformatter.h" // formatter

namespace FlexEngine
{
  namespace Asset
  {
    // Initializes as "\data\thing.flxdata for example
    FlxData::FlxData(std::string const& key) : SAVE_FILE_NAME(key)
    {
      Load();
    }

    void FlxData::Load()
    {
      // If FlxData doesn't exist, create it.
      std::ifstream ifs(Path::current("assets" + SAVE_FILE_NAME).c_str()); // asset/SAVE_FILE_NAME
      if (!ifs.is_open())
      {
        Log::Warning("Failed to open flxdata, creating a new one with name: " + SAVE_FILE_NAME);
        Internal_Create(SAVE_FILE_NAME);
        return;
      }

      // Wrap stream with rapidjson
      IStreamWrapper isw(ifs);

      document.ParseStream(isw);
      if (document.HasParseError())
        Log::Error("Failed to parse flxdata");

      ifs.close();

      Log::Info("Loaded flxdata: " + SAVE_FILE_NAME + " successfully.");
    }

    void FlxData::Save(bool prettify)
    {
      // Check for validity of file to write to, if not writeable, create a new one.
      std::ofstream ofs(Path::current("assets" + SAVE_FILE_NAME).c_str());
      if (!ofs.is_open())
      {
        Log::Fatal("Failed to open flxdata: Creating a default one with name: " + SAVE_FILE_NAME);
        Internal_Create(SAVE_FILE_NAME);
      }
      OStreamWrapper osw(ofs);

      // pretty writer outputs json with indentation and spacing
      if (prettify)
      {
        PrettyWriter<OStreamWrapper> prettywriter(osw);
        document.Accept(prettywriter);
      }
      else
      {
        Writer<OStreamWrapper> writer(osw);
        document.Accept(writer);
      }

      Log::Info("Saved flxdata " + SAVE_FILE_NAME + " successfully.");
    }

    #pragma region Getters

    float FlxData::GetFloat(const std::string& key, float default_value)
    {
      return HasKey(key) ? document[header][key].GetFloat() : default_value;
    }

    int FlxData::GetInt(const std::string& key, int default_value)
    {
      return HasKey(key) ? document[header][key].GetInt() : default_value;
    }

    std::string FlxData::GetString(const std::string& key, const std::string& default_value)
    {
      return HasKey(key) ? document[header][key].GetString() : default_value;
    }

    bool FlxData::GetBool(const std::string& key, bool default_value)
    {
      return HasKey(key) ? document[header][key].GetBool() : default_value;
    }

    #pragma endregion

    #pragma region Setters

    void FlxData::SetFloat(const std::string& key, float value)
    {
      if (HasKey(key))
      {
        // If key already exists, update the value
        document[header][key].SetFloat(value);
      }
      else
      {
        document[header].AddMember(Value(key.c_str(), document.GetAllocator()).Move(),
                                  Value(value).Move(),
                                  document.GetAllocator());
      }
    }

    void FlxData::SetInt(const std::string& key, int value)
    {
      if (HasKey(key))
      {
        // If key already exists, update the value
        document[header][key].SetInt(value);
      }
      else
      {
        document[header].AddMember(Value(key.c_str(), document.GetAllocator()).Move(),
                                      Value(value).Move(),
                                      document.GetAllocator());
      }
    }

    void FlxData::SetString(const std::string& key, const std::string& value)
    {
      if (HasKey(key))
      {
        document[header][key].SetString(value.c_str(), document.GetAllocator());
      }
      else
      {
        document[header].AddMember(Value(key.c_str(), document.GetAllocator()).Move(),
                                      Value(value.c_str(), document.GetAllocator()).Move(),
                                      document.GetAllocator());
      }
    }

    void FlxData::SetBool(const std::string& key, bool value)
    {
      if (HasKey(key))
      {
        document[header][key].SetBool(value);
      }
      else
      {
        document[header].AddMember(Value(key.c_str(), document.GetAllocator()).Move(),
                                      Value(value).Move(),
                                      document.GetAllocator());
      }
    }

    bool FlxData::HasKey(const std::string& key) const
    {
      return document[header].HasMember(key);
    }

    void FlxData::DeleteKey(const std::string& key)
    {
      if (HasKey(key))
      {
        document.RemoveMember(key);
      }
      else Log::Warning("Attmpted to delete a key that does not exist: " + key);
    }

    #pragma endregion

    void FlxData::Internal_Create(std::string const& name)
    {
      Path working_dir = Path::current("assets");
      Path file_path = File::Create(working_dir, name);
      File& new_file = File::Open(file_path);

      // Create a document using rapidjson format
      document.SetObject();

      // Layout will be in the form of key value pairs contained in an object, 
      // so we simply create an object expecting key value pairs to be inserted in the future.
      Value obj(kObjectType);
      document.AddMember("flxdata", obj, document.GetAllocator()); // Unfortunately cannot be using constexpr here

      // Write to document
      rapidjson::StringBuffer buffer;
      rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
      document.Accept(writer);

      // Write the entire JSON to a file.
      new_file.Write(buffer.GetString());
      new_file.Close(file_path);
    }
  }
}