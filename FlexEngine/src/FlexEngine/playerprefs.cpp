#include "pch.h"

#include "playerprefs.h"

#include <RapidJSON/istreamwrapper.h>
#include <RapidJSON/ostreamwrapper.h>

#include <RapidJSON/writer.h>
#include <RapidJSON/prettywriter.h>

constexpr const char* PLAYERPREFS_FILE_PATH = "playerprefs.json";

namespace FlexEngine
{
  // static members
  Document PlayerPrefs::m_document;

  void PlayerPrefs::Internal_Create()
  {
    FLX_FLOW_FUNCTION();

    if (m_document.IsObject())
    {
      DeleteAll();
    }
    else
    {
      m_document.SetObject();
    }

    SetString("Secret", "259240");

    // create save file
    std::fstream fs;
    fs.open(PLAYERPREFS_FILE_PATH, std::fstream::out);
    if (!fs.is_open())
    {
      std::cerr << "Failed to create PlayerPrefs save file" << std::endl;
      return;
    }
    fs.close();

    Save();
  }

  void PlayerPrefs::Load()
  {
    FLX_FLOW_FUNCTION();

    FLX_SCOPED_FUNCTION_TIMER();

    std::ifstream ifs(PLAYERPREFS_FILE_PATH);
    if (!ifs.is_open())
    {
      std::cerr << "Failed to open PlayerPrefs" << std::endl;

      // default error handling
      Internal_Create();

      return;
    }
    IStreamWrapper isw(ifs);

    m_document.ParseStream(isw);

    if (m_document.HasParseError())
    {
      std::cerr << "Error: Failed to parse PlayerPrefs\n" << m_document.GetParseError() << std::endl;
    }
    else
    {
      std::cout << "Successfully parsed PlayerPrefs" << std::endl;
    }

    ifs.close();
  }

  void PlayerPrefs::Save(bool prettify)
  {
    FLX_FLOW_FUNCTION();

    FLX_SCOPED_FUNCTION_TIMER();

    std::ofstream ofs(PLAYERPREFS_FILE_PATH);
    if (!ofs.is_open())
    {
      std::cerr << "Failed to open PlayerPrefs" << std::endl;
      return;
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

    std::cout << "Saved PlayerPrefs" << std::endl;

    ofs.close();
  }

  void PlayerPrefs::DeleteAll()
  {
    m_document.RemoveAllMembers();
  }

  bool PlayerPrefs::HasKey(std::string const& key)
  {
    return m_document.HasMember(key.c_str());
  }

  void PlayerPrefs::DeleteKey(std::string const& key)
  {
    m_document.RemoveMember(key.c_str());
  }

  #pragma region Getters

  float PlayerPrefs::GetFloat(std::string const& key, float const& default_value)
  {
    if (!HasKey(key.c_str()) && !m_document[key.c_str()].IsFloat())
    {
      return default_value;
    }
    else
    {
      return m_document[key.c_str()].GetFloat();
    }
  }

  int PlayerPrefs::GetInt(std::string const& key, int const& default_value)
  {
    if (!HasKey(key.c_str()) && !m_document[key.c_str()].IsInt())
    {
      return default_value;
    }
    else
    {
      return m_document[key.c_str()].GetInt();
    }
  }

  std::string PlayerPrefs::GetString(std::string const& key, std::string const& default_value)
  {
    if (!HasKey(key.c_str()) && !m_document[key.c_str()].IsString())
    {
      return default_value;
    }
    else
    {
      return m_document[key.c_str()].GetString();
    }
  }

  bool PlayerPrefs::GetBool(std::string const& key, bool const& default_value)
  {
    if (!HasKey(key.c_str()) && !m_document[key.c_str()].IsBool())
    {
      return default_value;
    }
    else
    {
      return m_document[key.c_str()].GetBool();
    }
  }

  #pragma endregion

  #pragma region Setters

  void PlayerPrefs::SetFloat(std::string const& key, float const& value)
  {
    m_document.AddMember(
      Value(key.c_str(), m_document.GetAllocator()).Move(),
      Value(value).Move(),
      m_document.GetAllocator()
    );
  }

  void PlayerPrefs::SetInt(std::string const& key, int const& value)
  {
    m_document.AddMember(
      Value(key.c_str(), m_document.GetAllocator()).Move(),
      Value(value).Move(),
      m_document.GetAllocator()
    );
  }

  void PlayerPrefs::SetString(std::string const& key, std::string const& value)
  {
    Value value_string;
    value_string.SetString(value.c_str(), static_cast<SizeType>(value.length()), m_document.GetAllocator());

    m_document.AddMember(
      Value(key.c_str(), m_document.GetAllocator()).Move(),
      value_string.Move(),
      m_document.GetAllocator()
    );
  }

  void PlayerPrefs::SetBool(std::string const& key, bool const& value)
  {
    m_document.AddMember(
      Value(key.c_str(), m_document.GetAllocator()).Move(),
      Value(value).Move(),
      m_document.GetAllocator()
    );
  }

  #pragma endregion

}