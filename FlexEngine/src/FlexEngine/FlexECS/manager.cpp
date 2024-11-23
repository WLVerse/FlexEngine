#include "datastructures.h"

namespace FlexEngine
{
  namespace FlexECS
  {
    // static member initialization
    //std::shared_ptr<Scene> Manager::active_scene = nullptr;

    #pragma region Scene Management Functions

    std::shared_ptr<Scene> Manager::CreateScene()
    {
      FLX_FLOW_FUNCTION();

      // set itself as the active scene unless there is already an active scene
      if (active_scene == nullptr)
      {
        active_scene = std::make_shared<Scene>(Scene::Null);
        return active_scene;
      }
      else
      {
        return std::make_shared<Scene>(Scene::Null);
      }
    }

    std::shared_ptr<Scene> Manager::GetActiveScene()
    {
      // create a new scene if there isn't one
      if (active_scene == nullptr)
      {
        Log::Warning("No active scene found. Creating a new scene.");
        CreateScene();
      }
      return active_scene;
    }

    void Manager::SetActiveScene(const Scene& scene)
    {
      SetActiveScene(std::make_shared<Scene>(scene));
    }
    void Manager::SetActiveScene(std::shared_ptr<Scene> scene)
    {
      FLX_FLOW_FUNCTION();

      // guard
      if (scene == nullptr)
      {
        Log::Warning("Use the Scene::Null to set the scene to null instead.");
        SetActiveScene(Scene::Null);
        return;
      }

      active_scene = scene;
    }

    #pragma endregion


    #pragma region Entity Management Functions

    // Creates a new entity by giving it a name
    Entity Manager::CreateEntity(const std::string& name)
    {
      FLX_FLOW_FUNCTION();

      using T = Scene::StringIndex;

      // manually register a name component
      // this is to register the entity in the entity index and archetype
      ComponentID component = Reflection::TypeResolver<T>::Get()->name;

      // type erasure
      T data_copy = Manager::GetActiveScene()->Internal_StringStorage_New(name);
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
      EntityID entity_id = ID::Create(ID::Flags::Flag_None, Manager::GetActiveScene()->_flx_id_next, Manager::GetActiveScene()->_flx_id_unused);

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

    void Manager::DestroyEntity(EntityID entity)
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
      ID::Destroy(entity, Manager::GetActiveScene()->_flx_id_unused);
    }

    void Manager::SetEntityFlags(EntityID& entity, const uint8_t flags)
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
    EntityID Manager::CloneEntity(EntityID entity_to_copy)
    {
      // Get the archetype of the entity to copy
      EntityRecord& entity_record = ENTITY_INDEX[entity_to_copy];
      Archetype& archetype = *entity_record.archetype;

      // First we need to assign this new entity an ID
      EntityID new_entity = ID::Create(ID::Flags::Flag_None, Manager::GetActiveScene()->_flx_id_next, Manager::GetActiveScene()->_flx_id_unused);
      
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
    void Manager::SaveEntityAsPrefab(EntityID entityToSave, const std::string& prefabName)
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

    // static function
    std::shared_ptr<Scene> Manager::Load(File& file)
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

      // relink entity archetype pointers
      deserialized_scene->Internal_RelinkEntityArchetypePointers();

      return deserialized_scene;
    }

    void Manager::SaveActiveScene(File& file)
    {
      Manager::GetActiveScene()->Save(file);
    }

    #pragma endregion

  }
}