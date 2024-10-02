/** WLVerse
 * \file flexprefs.cpp
 *
 * \brief
 *   Implementation of flexprefs via rapidjson
 *
 * \authors
 *   [100%] Yew Chong (yewchong.k\@digipen.edu)
 *
 * \par All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
 */
#include "pch.h"

#include "flexprefs.h"

#include "FlexEngine/flexformatter.h" // rapidjson

constexpr const char* SAVE_FILE_NAME = "flxprefs.json"; 
constexpr const char* container_name = "flexprefs"; // Name of the object that will contain all the key value pairs

namespace FlexEngine
{
  // static member initialization
  Document FlexPrefs::m_document;

  void FlexPrefs::Load()
  {
    // If FlexPref doesn't exist, create it.
    std::ifstream ifs(Path::current("assets/saves/").append(SAVE_FILE_NAME).c_str());
    if (!ifs.is_open())
    {
      Log::Warning("Failed to open FlexPrefs");
      Internal_Create();
      return;
    }
    
    // Wrap stream with rapidjson
    IStreamWrapper isw(ifs);

    m_document.ParseStream(isw);
    if (m_document.HasParseError()) 
      Log::Error("Failed to parse FlexPrefs");

    ifs.close();

    Log::Info("Loaded FlexPrefs successfully.");
  }

  void FlexPrefs::Save(bool prettify)
  {
    // Check for validity of file to write to, if not writeable, create a new one.
    std::ofstream ofs(Path::current().append(SAVE_FILE_NAME).c_str());
    if (!ofs.is_open())
    {
      Log::Warning("Failed to open FlexPrefs: Creating a default one...");
      Internal_Create();
    }
    OStreamWrapper osw(ofs);

    // pretty writer outputs json with indentation and spacing
    if (prettify)
    {
      PrettyWriter<OStreamWrapper> prettywriter(osw);
      m_document.Accept(prettywriter);
    }
    else
    {
      Writer<OStreamWrapper> writer(osw);
      m_document.Accept(writer);
    }

    Log::Info("Saved FlexPrefs successfully.");
  }

  void FlexPrefs::DeleteAll()
  {
    m_document.RemoveAllMembers();
    Internal_Create(); // Ensure to reset back to our empty state
    Save();
  }

  bool FlexPrefs::HasKey(const std::string& key)
  {
    return m_document[container_name].HasMember(key);
  }

  void FlexPrefs::DeleteKey(const std::string& key)
  {
    if (HasKey(key))
    {
      m_document.RemoveMember(key);
    }
    else Log::Warning("Attmpted to delete a key that does not exist: " + key);
  }

  #pragma region Getters

  float FlexPrefs::GetFloat(const std::string& key, float default_value)
  {
    return HasKey(key) ? m_document[container_name][key].GetFloat() : default_value;
  }

  int FlexPrefs::GetInt(const std::string& key, int default_value)
  {
    return HasKey(key) ? m_document[container_name][key].GetInt() : default_value;
  }

  std::string FlexPrefs::GetString(const std::string& key, const std::string& default_value)
  {
    return HasKey(key) ? m_document[container_name][key].GetString() : default_value;
  }

  bool FlexPrefs::GetBool(const std::string& key, bool default_value)
  {
    return HasKey(key) ? m_document[container_name][key].GetBool() : default_value;
  }

  #pragma endregion

  #pragma region Setters

  void FlexPrefs::SetFloat(const std::string& key, float value)
  {
    if (HasKey(key))
    {
      // If key already exists, update the value
      m_document[container_name][key].SetFloat(value);
    }
    else
    {
      m_document[container_name].AddMember(Value(key.c_str(), m_document.GetAllocator()).Move(),
                                Value(value).Move(),
                                m_document.GetAllocator());
    }
  }

  void FlexPrefs::SetInt(const std::string& key, int value)
  {
    if (HasKey(key))
    {
      // If key already exists, update the value
      m_document[container_name][key].SetInt(value);
    }
    else
    {
      m_document[container_name].AddMember(Value(key.c_str(), m_document.GetAllocator()).Move(),
                                    Value(value).Move(),
                                    m_document.GetAllocator());
    }
  }

  void FlexPrefs::SetString(const std::string& key, const std::string& value)
  {
    if (HasKey(key))
    {
      m_document[container_name][key].SetString(value.c_str(), m_document.GetAllocator());
    }
    else
    {
      m_document[container_name].AddMember(Value(key.c_str(), m_document.GetAllocator()).Move(),
                                    Value(value.c_str(), m_document.GetAllocator()).Move(),
                                    m_document.GetAllocator());
    }
  }

  void FlexPrefs::SetBool(const std::string& key, bool value)
  {
    if (HasKey(key))
    {
      m_document[container_name][key].SetBool(value);
    }
    else
    {
      m_document[container_name].AddMember(Value(key.c_str(), m_document.GetAllocator()).Move(),
                                    Value(value).Move(),
                                    m_document.GetAllocator());
    }
  }

  #pragma endregion

  void FlexPrefs::Internal_Create()
  {
    Path working_dir = Path::current("assets\\saves");
    Path file_path = File::Create(working_dir, SAVE_FILE_NAME);
    File& new_file = File::Open(file_path);

    // Create a document using rapidjson format
    m_document.SetObject();

    // Layout will be in the form of key value pairs contained in an object, 
    // so we simply create an object expecting key value pairs to be inserted in the future.
    Value obj(kObjectType);
    m_document.AddMember("flexprefs", obj, m_document.GetAllocator()); // Unfortunately cannot be using constexpr here

    // Write to document
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    m_document.Accept(writer);

    // Write the entire JSON to a file.
    new_file.Write(buffer.GetString());
    new_file.Close(file_path);
  }
}

#pragma region Unit Testing code that I dun wanna delete cuz I typed so much, can be integrated into a nice unit test suite later.
/*
  Log::Info("Test attempting to load nothing in the folder at first and then setting data and saving.");
  FlexPrefs::Load();
  FlexPrefs::SetBool("test", false);
  FlexPrefs::SetString("tester", "Hello2 World!");
  FlexPrefs::SetInt("testint", 1243);
  FlexPrefs::SetFloat("testfloat", 123.4516f);
  FlexPrefs::Save();
  Log::Info("Passed setting data and saving.");

  Log::Info("Test updating values in the file.");
  FlexPrefs::Load();
  FlexPrefs::SetBool("test", true);
  FlexPrefs::SetString("tester", "Hello World!");
  FlexPrefs::SetInt("testint", 123);
  FlexPrefs::SetFloat("testfloat", 123.456f);
  FlexPrefs::Save();

  Log::Info("File at this point should exist and be valid, test reading it back...");
  FlexPrefs::Load();
  if (FlexPrefs::GetBool("test", false) == false) Log::Warning("Failed to get bool");
  if (FlexPrefs::GetString("tester", "") != "Hello World!") Log::Warning("Failed to get string");
  if (FlexPrefs::GetInt("testint", 0) != 123) Log::Warning("Failed to get int");
  if (FlexPrefs::GetFloat("testfloat", 0.0f) != 123.456f) Log::Warning("Failed to get float");
  Log::Info("Passed reading from written file.");
      
  Log::Info("Deleting all keys...");
  FlexPrefs::DeleteAll();
      
  Log::Info("Testing for failure to delete keys");
  FlexPrefs::Load();
  if (FlexPrefs::HasKey("test")) Log::Warning("Failed to delete bool key");
  if (FlexPrefs::HasKey("tester")) Log::Warning("Failed to delete str key");
  if (FlexPrefs::HasKey("testint")) Log::Warning("Failed to delete int key");
  if (FlexPrefs::HasKey("testfloat")) Log::Warning("Failed to delete float key");
  Log::Info("Passed deleting all keys.");

  Log::Info("Testing for DeleteKey on non existent key - One warning expected");
  FlexPrefs::DeleteKey("test"); // should prompt warning
  Log::Info("Passed DeleteKey on non existent key.");
*/
#pragma endregion