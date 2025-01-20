// WLVERSE [https://wlverse.web.app]
// scene.cpp
// 
// The FlexEngine Entity-Component-System (ECS) implementation
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "datastructures.h"

namespace FlexEngine
{
  namespace FlexECS
  {
    // static member initialization
    std::shared_ptr<Scene> Scene::s_active_scene = nullptr;
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


    #pragma region Scene Management Functions

    std::shared_ptr<Scene> Scene::CreateScene()
    {
      FLX_FLOW_FUNCTION();

      // set itself as the active scene unless there is already an active scene
      if (s_active_scene == nullptr)
      {
        s_active_scene = std::make_shared<Scene>(Scene::Null);
        return s_active_scene;
      }
      else
      {
        return std::make_shared<Scene>(Scene::Null);
      }
    }

    std::shared_ptr<Scene> Scene::GetActiveScene()
    {
      // create a new scene if there isn't one
      if (s_active_scene == nullptr)
      {
        Log::Warning("No active scene found. Creating a new scene.");
        CreateScene();
      }
      return s_active_scene;
    }

    void Scene::SetActiveScene(const Scene& scene)
    {
      SetActiveScene(std::make_shared<Scene>(scene));
    }
    void Scene::SetActiveScene(std::shared_ptr<Scene> scene)
    {
      FLX_FLOW_FUNCTION();

      // guard
      if (scene == nullptr)
      {
        Log::Warning("Use the Scene::Null to set the scene to null instead.");
        SetActiveScene(Scene::Null);
        return;
      }

      s_active_scene = scene;
    }

    #pragma endregion


    #pragma region Entity Management Functions

    // Creates a new entity by giving it a name
    Entity Scene::CreateEntity(const std::string& name)
    {
      FLX_FLOW_FUNCTION();

      using T = StringIndex;

      // manually register a name component
      // this is to register the entity in the entity index and archetype
      ComponentID component = Reflection::TypeResolver<T>::Get()->name;

      // type erasure
      T data_copy = FLX_STRING_NEW(name);
      void* data_copy_ptr = reinterpret_cast<void*>(&data_copy);
      ComponentData<void> data_ptr = Internal_CreateComponentData(sizeof(T), data_copy_ptr);

      // Get the archetype for the entity
      ComponentIDList type = { component };

      // create a new archetype if it doesn't exist
      if (ARCHETYPE_INDEX.count(type) == 0)
      {
        Entity::Internal_CreateArchetype(type);
      }

      // create entity id
      EntityID entity_id = ID::Create(ID::Flags::Flag_None, Scene::GetActiveScene()->_flx_id_next, Scene::GetActiveScene()->_flx_id_unused);

      // update entity vector
      Archetype& archetype = ARCHETYPE_INDEX[type];
      archetype.entities.push_back(entity_id);

      // update entity records
      EntityRecord entity_record = { &archetype, archetype.id, archetype.entities.size() - 1 };
      ENTITY_INDEX[entity_id] = entity_record;

      // store the component data in the archetype
      //ArchetypeMap& archetype_map = COMPONENT_INDEX[component];
      //ArchetypeRecord& archetype_record = archetype_map[archetype.id];
      //archetype.archetype_table[archetype_record.column].push_back(data_ptr);
      archetype.archetype_table[0].push_back(data_ptr); // there is only one component in this archetype

      return entity_id;
    }

    void Scene::DestroyEntity(EntityID entity)
    {
      FLX_FLOW_FUNCTION();

      // guard: entity does not exist
      if (ENTITY_INDEX.count(entity) == 0)
      {
        Log::Warning("Attempted to destroy entity that does not exist.");
        return;
      }

      // Get the important data
      // The entity's archetype and row are needed to remove the entity from the archetype
      EntityRecord& entity_record = ENTITY_INDEX[entity];
      Archetype& archetype = *entity_record.archetype;
      std::size_t row = entity_record.row;

      // Remove the entity from the source archetype's columns and entities vector
      // The same code is being used in Internal_MoveEntity
      std::size_t last_row_index = archetype.archetype_table[0].size() - 1;

      // Handle the case where the entity is the last entity in the archetype
      if (row == last_row_index)
      {
        for (std::size_t i = 0; i < archetype.archetype_table.size(); i++)
        {
          archetype.archetype_table[i].pop_back();
        }
      }
      else
      {
        // Swap the entity with the last entity in the archetype and pop it
        // Using swap-and-pop is more performant than erase() since it requires shifting
        // all subsequent elements forward.
        // O(1) complexity for swap-and-pop vs O(n) complexity for erase()
        for (std::size_t i = 0; i < archetype.archetype_table.size(); i++)
        {
          std::swap(archetype.archetype_table[i][row], archetype.archetype_table[i][last_row_index]);
          archetype.archetype_table[i].pop_back();
        }

        // Update entity_index for the swapped entity if necessary
        if (row < last_row_index)
        {
          EntityID swapped_entity = archetype.entities[last_row_index];
          ENTITY_INDEX[swapped_entity].row = row;

          // Replace the entity's row in the entities vector
          archetype.entities[row] = swapped_entity;
        }
      }

      // Pop the entity from the entities vector
      archetype.entities.pop_back();

      // Remove the entity from the entity index
      ENTITY_INDEX.erase(entity);

      // Destroy the entity id
      ID::Destroy(entity, Scene::GetActiveScene()->_flx_id_unused);
    }

    void Scene::SetEntityFlags(EntityID& entity, const uint8_t flags)
    {
      EntityID updated_entity = entity;
      ID::SetFlags(updated_entity, flags);

      // update the entity in the archetype entity vector
      EntityRecord& entity_record = ENTITY_INDEX[entity];
      Archetype& archetype = *entity_record.archetype;
      std::size_t row = entity_record.row;
      archetype.entities[row] = updated_entity;

      // move the entity record to the new key
      ENTITY_INDEX[updated_entity] = entity_record;
      ENTITY_INDEX.erase(entity);

      entity = updated_entity;
    }

    /*!
      \brief Clones entity via archetype row copy.
      \param entity_to_copy Entity to clone.
      \return EntityID of the cloned entity.
    */
    EntityID Scene::CloneEntity(EntityID entity_to_copy)
    {
      // Get the archetype of the entity to copy
      EntityRecord& entity_record = ENTITY_INDEX[entity_to_copy];
      Archetype& archetype = *entity_record.archetype;

      // First we need to assign this new entity an ID
      EntityID new_entity = ID::Create(ID::Flags::Flag_None, Scene::GetActiveScene()->_flx_id_next, Scene::GetActiveScene()->_flx_id_unused);

      // Secondly, we update the scene's archetype by telling it we want to add one more entity of this index...
      archetype.entities.push_back(new_entity);

      // ... then update entity records of this new entity
      EntityRecord new_entity_record = { &archetype, archetype.id, archetype.entities.size() - 1 };
      ENTITY_INDEX[new_entity] = new_entity_record;

      // Now, after the setup is complete, we copy the entire row over
      for (std::size_t i{}; i < archetype.archetype_table.size(); i++)
      {
        // Perform deep copy, after all, the data inside is actually just a pointer, so we need to reserve memory to store the new one.
        std::pair<size_t, void*> old_data = Internal_GetComponentData(archetype.archetype_table[i][entity_record.row]);
        ComponentData<void> new_data_instance = Internal_CreateComponentData(old_data.first, old_data.second);
        archetype.archetype_table[i].push_back(new_data_instance);
      }

      return new_entity;
    }

    /*!
      \brief Saves an entity as a .flxprefab file
      \param entityToSave ID of entity to save as prefab.
      \param prefabName Name of the prefab file.
    */
    void Scene::SaveEntityAsPrefab(EntityID entityToSave, const std::string& prefabName)
    {
      // Get the current entity to write to prefab
      EntityRecord& entity_record = ENTITY_INDEX[entityToSave];
      Archetype& archetype = *entity_record.archetype;

      // Create a new prefab file in asset manager directory, then open this file
      std::string file_name = prefabName + ".flxprefab";
      Path dir = Path::current("assets\\prefabs");
      Path prefab_path = File::Create(dir, file_name);
      File& prefab_file = File::Open(prefab_path);

      // Concat in sstream before writing it to the formatter.
      std::stringstream data_stream;
      for (std::size_t i{}; i < archetype.archetype_table.size(); i++) // For component in the archetype...
      {
        // Automatic serialization as long as a type is provided.
        Reflection::TypeDescriptor* type = TYPE_DESCRIPTOR_LOOKUP[archetype.type[i]];
        type->Serialize(Internal_GetComponentData(archetype.archetype_table[i][entity_record.row]).second, data_stream);

        if (i != archetype.archetype_table.size() - 1) data_stream << ","; // Add a comma to separate components.
      }

      // Wrap data in formatter
      FlxFmtFile formatter = FlexFormatter::Create(data_stream.str(), true);
      std::string file_contents = formatter.Save();
      prefab_file.Write(file_contents);

      // ... and close the file
      File::Close(prefab_path);
    }

    #pragma endregion


    #pragma region Scene Serialization Functions

    // save the scene to a File
    // the flx formatter is wrapped here
    // todo: ComponentData<void> needs to be serialized separately
    // after doing explicit serialization for each component, we can then
    // let the reflection system handle the rest of the serialization
    // how this is implemented is by simply looping through every single component and saving them in a vector
    // where reflection can then serialize them
    void Scene::Save(File& file)
    {
      // convert the scene to a serialized archetype
      // This is done to prevent the reflection system from serializing the ComponentData<void> pointers.
      Internal_ConvertToSerializedArchetype();

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

    // static function
    std::shared_ptr<Scene> Scene::Load(File& file)
    {
      Reflection::TypeDescriptor* type_desc = Reflection::TypeResolver<FlexECS::Scene>::Get();

      // get scene data
      FlxFmtFile flxfmtfile = FlexFormatter::Parse(file, FlxFmtFileType::Scene);
      if (flxfmtfile == FlxFmtFile::Null) return std::make_shared<Scene>(Scene::Null);

      // deserialize
      Document document;
      document.Parse(flxfmtfile.data.c_str());
      if (document.HasParseError())
      {
        Log::Error("Failed to parse scene data.");
        return std::make_shared<Scene>(Scene::Null);
      }

      std::shared_ptr<Scene> deserialized_scene = std::make_shared<Scene>();
      type_desc->Deserialize(deserialized_scene.get(), document);

      // convert the serialized archetype to the scene's archetype
      deserialized_scene->Internal_ConvertFromSerializedArchetype();

      // relink entity archetype pointers
      deserialized_scene->Internal_RelinkEntityArchetypePointers();

      return deserialized_scene;
    }

    void Scene::SaveActiveScene(File& file)
    {
      Scene::GetActiveScene()->Save(file);
    }

    void Scene::Internal_ConvertToSerializedArchetype()
    {
      _archetype_index.clear();

      for (auto& [type, archetype] : archetype_index)
      {
        _Archetype _archetype;
        _archetype.id = archetype.id;
        _archetype.type = archetype.type;
        _archetype.entities = archetype.entities;
        _archetype.edges = archetype.edges;

        // Convert the archetype_table to a vector of strings
        // The type vector is a list of the typedescriptors which can be
        // used to serialize the data

        // For each type in the archetype
        for (std::size_t i = 0; i < _archetype.type.size(); i++)
        {
          // Add a new row to the archetype_table
          _archetype.archetype_table.push_back(std::vector<std::string>());

          // For each entity in the archetype
          for (std::size_t j = 0; j < archetype.archetype_table[i].size(); j++)
          {
            std::stringstream ss;

            // Get the type descriptor
            Reflection::TypeDescriptor* type_desc = TYPE_DESCRIPTOR_LOOKUP[_archetype.type[i]];

            // Get the component data
            void* data = Internal_GetComponentData(archetype.archetype_table[i][j]).second;

            // Serialize the component data
            type_desc->Serialize(data, ss);

            _archetype.archetype_table[i].push_back(ss.str());
          }
        }

        _archetype_index[archetype.type] = _archetype;
      
      }
    
    }

    void Scene::Internal_ConvertFromSerializedArchetype()
    {
      archetype_index.clear();
      for (auto& [_type, _archetype] : _archetype_index)
      {
        Archetype archetype;
        archetype.id = _archetype.id;
        archetype.type = _archetype.type;
        archetype.entities = _archetype.entities;
        archetype.edges = _archetype.edges;

        // Convert the archetype_table from a vector of strings to a vector of ComponentData<void>
        for (std::size_t i = 0; i < _archetype.archetype_table.size(); i++)
        {
          archetype.archetype_table.push_back(std::vector<ComponentData<void>>());

          for (std::size_t j = 0; j < _archetype.archetype_table[i].size(); j++)
          {
            // Convert the string into json
            Document document;
            document.Parse(_archetype.archetype_table[i][j].c_str());
            
            // Get the type descriptor
            Reflection::TypeDescriptor* type_desc = TYPE_DESCRIPTOR_LOOKUP[_archetype.type[i]];

            // Deserialize the component data
            // TODO: How can I know the size of the data?
            void* data = new char[type_desc->size];
            type_desc->Deserialize(data, document);

            // Create a new ComponentData<void>
            ComponentData<void> data_ptr = Internal_CreateComponentData(type_desc->size, data);

            delete data;

            archetype.archetype_table[i].push_back(data_ptr);
          }
        }
        archetype_index[archetype.type] = archetype;
      }
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