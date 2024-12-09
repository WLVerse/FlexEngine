#include "pch.h"

#include "flexprefs.h"

#include "FlexEngine/flexformatter.h" // rapidjson

constexpr const char* SAVE_FILE_NAME = "flxprefs.json";

namespace FlexEngine
{
  // static member initialization
  Document FlexPrefs::m_document;

  void FlexPrefs::Load()
  {
  }

  void FlexPrefs::Save(bool prettify)
  {
  }

  void FlexPrefs::DeleteAll()
  {
  }

  bool FlexPrefs::HasKey(const std::string& key)
  {
    return false;
  }

  void FlexPrefs::DeleteKey(const std::string& key)
  {
  }

  #pragma region Getters

  float FlexPrefs::GetFloat(const std::string& key, float default_value)
  {
    return 0.0f;
  }

  int FlexPrefs::GetInt(const std::string& key, int default_value)
  {
    return 0;
  }

  std::string FlexPrefs::GetString(const std::string& key, const std::string& default_value)
  {
    return "";
  }

  bool FlexPrefs::GetBool(const std::string& key, bool default_value)
  {
    return false;
  }

  #pragma endregion

  #pragma region Setters

  void FlexPrefs::SetFloat(const std::string& key, float value)
  {
  }

  void FlexPrefs::SetInt(const std::string& key, int value)
  {
  }

  void FlexPrefs::SetString(const std::string& key, const std::string& value)
  {
  }

  void FlexPrefs::SetBool(const std::string& key, bool value)
  {
  }

  #pragma endregion

  void FlexPrefs::Internal_Create()
  {
  }

}