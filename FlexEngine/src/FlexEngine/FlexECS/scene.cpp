#include "datastructures.h"

namespace FlexEngine
{
  namespace FlexECS
  {
    // static member initialization
    Scene Scene::Null = Scene();


    #pragma region String Storage

    std::string& Scene::Internal_StringStorage_Get(StringIndex index)
    {
      return string_storage[index];
    }

    Scene::StringIndex Scene::Internal_StringStorage_New(const std::string& str)
    {
      StringIndex index = 1;

      // check if there are any free indices
      if (!string_storage_free_list.empty())
      {
        index = string_storage_free_list.back();
        string_storage_free_list.pop_back();

        // store the string
        string_storage[index] = str;
      }
      // get the next index
      else
      {
        // store the string
        string_storage.push_back(str);
        index = string_storage.size() - 1;
      }

      // return the index
      return index;
    }

    void Scene::Internal_StringStorage_Delete(StringIndex index)
    {
      // add the index to the free list
      string_storage_free_list.push_back(index);
    }

    #pragma endregion


    #pragma region Scene Serialization Functions

    // save the scene to a File
    // the flx formatter is wrapped here
    void Scene::Save(File& file)
    {
      Reflection::TypeDescriptor* type_desc = Reflection::TypeResolver<FlexECS::Scene>::Get();

      std::stringstream ss;
      type_desc->Serialize(this, ss);

      // check if we need to create a new flx file or overwrite the existing one
      FlxFmtFile flxfmtfile = FlexFormatter::Parse(file, FlxFmtFileType::Scene);
      if (flxfmtfile == FlxFmtFile::Null)
      {
        // make a new flx file
        flxfmtfile = FlexFormatter::Create(ss.str(), true);
      }
      else
      {
        // update the data
        flxfmtfile.data = ss.str();
      }

      file.Write(flxfmtfile.Save());
    }

    #pragma endregion


    #pragma region Internal Functions

    // relink entity archetype pointers
    // for each entity in the entity index, set the archetype pointer to the archetype in the archetype index
    void Scene::Internal_RelinkEntityArchetypePointers()
    {
      for (auto& [uuid, entity_record] : entity_index)
      {
        auto it = std::find_if(
          archetype_index.begin(), archetype_index.end(),
          [&entity_record](auto& archetype_record)
          {
            return archetype_record.second.id == entity_record.archetype_id;
          }
        );

        if (it != archetype_index.end())
        {
          // relink
          entity_record.archetype = &it->second;
        }
        else
        {
          Log::Error("Entity archetype not found in archetype index.");
        }
      }
    }

    #pragma endregion


#ifdef _DEBUG

    void Scene::Dump() const
    {
      DumpArchetypeIndex();
      DumpEntityIndex();
      DumpComponentIndex();
    }

    void Scene::DumpArchetypeIndex() const
    {
      FLX_FLOW_BEGINSCOPE();
      for (auto& [archetype, archetype_storage] : archetype_index)
      {
        Log::Debug("Archetype: " + std::to_string(archetype_storage.id));
        Log::Debug("- Number of entities: " + std::to_string(archetype_storage.entities.size()));
        Log::Debug("- Number of components: " + std::to_string(archetype_storage.archetype_table.size()));

        for (std::size_t i = 0; i < archetype_storage.archetype_table.size(); i++)
        {
          Log::Debug("  Component(" + std::to_string(i) + "): " + archetype_storage.type[i]);
          //Log::Debug("    Entities in component: " + std::to_string(archetype_storage.archetype_table[i].size()));
        }
      }
      FLX_FLOW_ENDSCOPE();
    }

    void Scene::DumpEntityIndex() const
    {
      FLX_FLOW_BEGINSCOPE();
      for (auto& [id, entity_record] : entity_index)
      {
        Log::Debug("Entity: " + std::to_string(id));
        Log::Debug("  Archetype ID: " + std::to_string(entity_record.archetype->id));
      }
      FLX_FLOW_ENDSCOPE();
    }

    void Scene::DumpComponentIndex() const
    {
      FLX_FLOW_BEGINSCOPE();
      for (auto& [component_id, archetype_map] : component_index)
      {
        Log::Debug("Component: " + component_id);
        for (auto& [archetype, archetype_record] : archetype_map)
        {
          Log::Debug("  Archetype ID: " + std::to_string(archetype));
          Log::Debug("    Column: " + std::to_string(archetype_record.column));
        }
      }
      FLX_FLOW_ENDSCOPE();
    }

#endif

  }
}