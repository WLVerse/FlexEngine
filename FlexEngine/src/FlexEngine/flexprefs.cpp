#include "pch.h"
#include "flexprefs.h"
#include "FlexEngine/Utilities/flexformatter.h" // rapidjson

constexpr const char* SAVE_FILE_NAME = "flxprefs.json";
constexpr const char* container_name = "flexprefs"; // The object containing all key-value pairs

namespace FlexEngine
{
    // Static member initialization.
    Document FlexPrefs::m_document;

    // Helper to get the full file path (ensuring consistency)
    std::string GetSaveFilePath()
    {
        // Adjust the path as needed. All functions now use the same path.
        return Path::current("assets/saves/").append(SAVE_FILE_NAME);
    }

    void FlexPrefs::Load()
    {
        std::string filePath = GetSaveFilePath();
        std::ifstream ifs(filePath.c_str());
        if (!ifs.is_open())
        {
            Log::Warning("Failed to open FlexPrefs at " + filePath);
            Internal_Create();
            return;
        }

        IStreamWrapper isw(ifs);
        m_document.ParseStream(isw);
        if (m_document.HasParseError())
        {
            Log::Error("Failed to parse FlexPrefs; recreating default.");
            Internal_Create();
            return;
        }
        ifs.close();

        // Ensure the JSON container exists and is an object.
        if (!m_document.HasMember(container_name) || !m_document[container_name].IsObject())
        {
            Log::Warning("Missing or invalid container in FlexPrefs; recreating default.");
            Internal_Create();
        }

        Log::Info("Loaded FlexPrefs successfully from " + filePath);
    }

    void FlexPrefs::Save(bool prettify)
    {
        std::string filePath = GetSaveFilePath();
        std::ofstream ofs(filePath.c_str());
        if (!ofs.is_open())
        {
            Log::Warning("Failed to open FlexPrefs for saving at " + filePath + "; recreating default.");
            Internal_Create();
            return;
        }

        OStreamWrapper osw(ofs);
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
        ofs.close();

        Log::Info("Saved FlexPrefs successfully to " + filePath);
    }

    void FlexPrefs::DeleteAll()
    {
        m_document.RemoveAllMembers();
        Internal_Create(); // Reset to our default state.
        Save();
    }

    bool FlexPrefs::HasKey(const std::string& key)
    {
        // Check that our container exists first.
        if (!m_document.HasMember(container_name))
            return false;
        return m_document[container_name].HasMember(key);
    }

    void FlexPrefs::DeleteKey(const std::string& key)
    {
        if (m_document.HasMember(container_name) && m_document[container_name].HasMember(key))
        {
            m_document[container_name].RemoveMember(key);
        }
        else
        {
            Log::Warning("Attempted to delete a key that does not exist: " + key);
        }
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
            m_document[container_name][key].SetFloat(value);
        }
        else
        {
            m_document[container_name].AddMember(
                Value(key.c_str(), m_document.GetAllocator()).Move(),
                Value(value).Move(),
                m_document.GetAllocator());
        }
    }

    void FlexPrefs::SetInt(const std::string& key, int value)
    {
        if (HasKey(key))
        {
            m_document[container_name][key].SetInt(value);
        }
        else
        {
            m_document[container_name].AddMember(
                Value(key.c_str(), m_document.GetAllocator()).Move(),
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
            m_document[container_name].AddMember(
                Value(key.c_str(), m_document.GetAllocator()).Move(),
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
            m_document[container_name].AddMember(
                Value(key.c_str(), m_document.GetAllocator()).Move(),
                Value(value).Move(),
                m_document.GetAllocator());
        }
    }

    #pragma endregion

    void FlexPrefs::Internal_Create()
    {
        std::string dirPath = Path::current("assets/saves/");
        std::string filePath = dirPath;
        filePath.append(SAVE_FILE_NAME);

        // Create the file if it doesn't exist.
        Path working_dir = Path::current("assets\\saves");
        Path file_path = File::Create(working_dir, SAVE_FILE_NAME);
        File& new_file = File::Open(file_path);

        // Reset document.
        m_document.SetObject();

        // Create our container object.
        Value obj(kObjectType);
        m_document.AddMember(Value(container_name, m_document.GetAllocator()).Move(),
                             obj,
                             m_document.GetAllocator());

        // Write to document.
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        m_document.Accept(writer);

        // Write the JSON string to the file.
        new_file.Write(buffer.GetString());
        new_file.Close(file_path);

        Log::Info("Created default FlexPrefs file at " + GetSaveFilePath());
    }
}
