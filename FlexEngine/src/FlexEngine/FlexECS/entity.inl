// WLVERSE [https://wlverse.web.app]
// entity.inl
// 
// Implementation of inline functions for Entity class. Describes the API for entities in the ECS system.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

template <typename T>
bool FlexEngine::FlexECS::Entity::HasComponent()
{
  // cache the entity id
  EntityID entity = entity_id;

  // get the component id
  ComponentID component = Reflection::TypeResolver<T>::Get()->name;

  // guard: check if the component is in the index
  // provides an early exit because if it's not in the index, it's not in any archetype
  // this also prevents component_index[component] from creating a new entry
  if (COMPONENT_INDEX.count(component) == 0) return false;

  // figure out the archetype for the entity
  EntityRecord& entity_record = ENTITY_INDEX[entity];
  Archetype& archetype = *entity_record.archetype;

  // check if the component is in the archetype
  ArchetypeMap& archetype_map = COMPONENT_INDEX[component];
  return (archetype_map.count(archetype.id) != 0);
}


// Get the archetype and row from the entity_index, then get the column from the component_index.
// Use the column and row to get the component data from the archetype_table.
// This performs two lookups in the entity_index and two lookups in the component_index.
template <typename T>
T* FlexEngine::FlexECS::Entity::GetComponent()
{
  // cache the entity id
  EntityID entity = entity_id;

  // get the component id
  ComponentID component = Reflection::TypeResolver<T>::Get()->name;

  // guard: HasComponent
  // This has some repeated lookups, so it can be further optimized by copying the HasComponent code here
  //if (!HasComponent<T>(entity)) return nullptr;
  #pragma region HasComponent

  // guard: check if the component is in the index
  // provides an early exit because if it's not in the index, it's not in any archetype
  // this also prevents component_index[component] from creating a new entry
  if (COMPONENT_INDEX.count(component) == 0)
  {
    Log::Error("Component not found in the index");
    return nullptr;
  }

  // figure out the archetype for the entity
  EntityRecord& entity_record = ENTITY_INDEX[entity];
  Archetype& archetype = *entity_record.archetype;

  // check if the component is in the archetype
  ArchetypeMap& archetype_map = COMPONENT_INDEX[component];
  if (archetype_map.count(archetype.id) == 0)
  {
    Log::Error("Component not found in the archetype");
    return nullptr;
  }

  #pragma endregion

  // get the component data
  ArchetypeRecord& archetype_record = archetype_map[archetype.id];
  ComponentData<void> component_data = archetype.archetype_table[archetype_record.column][entity_record.row];
  void* data = Internal_GetComponentData(component_data).second;
  T* out_component = reinterpret_cast<T*>(data);
  return out_component;
}

template <typename T>
bool FlexEngine::FlexECS::Entity::TryGetComponent(T* out)
{
  // guard
  if (!HasComponent<T>()) return false;

  out = GetComponent<T>();
  return (out != nullptr);
}


// Steps to add a component to an entity:
// - Perform type erasure on the component data
// - Find or create the archetype that has the component we want to add
//   in addition to the components the entity already had.
// - Insert a new row into the destination archetype.
// - Move overlapping components over to the destination archetype.
// - Remove the entity from the current archetype.
// - Update the entity's archetype and row in entity_index.
template <typename T>
void FlexEngine::FlexECS::Entity::AddComponent(const T& data)
{
  // cache the entity id
  EntityID entity = entity_id;

  // get component id
  ComponentID component = Reflection::TypeResolver<T>::Get()->name;

  // type erasure
  T data_copy = data;
  void* data_copy_ptr = reinterpret_cast<void*>(&data_copy);

  // if the type is std::string, run Internal_CreateComponentData_string instead
  ComponentData<void> data_ptr = std::is_same<T, std::string>::value ? Internal_CreateComponentData_string(data_copy_ptr) : Internal_CreateComponentData(sizeof(T), data_copy_ptr);

  // figure out the current archetype for the entity
  EntityRecord& entity_record = ENTITY_INDEX[entity];
  Archetype& archetype = *entity_record.archetype;


  // find or create the archetype that has the component we want to add
  // in addition to the components the entity already had


  // graph traversal skip
  if (
    archetype.edges.count(component) != 0 &&  // check if the edge exists
    archetype.edges[component].add != nullptr // check if the archetype exists
  )
  {
    // get the archetype
    Archetype& next_archetype = *archetype.edges[component].add;

    // move the entity to the new archetype
    Internal_MoveEntity(entity, archetype, entity_record.row, next_archetype);

    // store the component data in the archetype
    ArchetypeRecord& archetype_record = COMPONENT_INDEX[component][next_archetype.id];
    next_archetype.archetype_table[archetype_record.column].push_back(data_ptr);
  }
  // find or create the archetype
  else
  {

    // create the archetype type
    ComponentIDList new_type = archetype.type; // make copy
    new_type.push_back(component);
    std::sort(new_type.begin(), new_type.end());

    // find the archetype
    if (ARCHETYPE_INDEX.count(new_type) != 0)
    {
      // get the archetype
      Archetype& next_archetype = ARCHETYPE_INDEX[new_type];

      // move the entity to the new archetype
      Internal_MoveEntity(entity, archetype, entity_record.row, next_archetype);

      // store the component data in the archetype
      ArchetypeRecord& archetype_record = COMPONENT_INDEX[component][next_archetype.id];
      next_archetype.archetype_table[archetype_record.column].push_back(data_ptr);

      // update archetype graph
      archetype.edges[component].add = &next_archetype;    // adding the component to the current archetype will lead to the next archetype
      next_archetype.edges[component].remove = &archetype; // removing the component in the next archetype will lead back to the current archetype
    }
    // archetype doesn't exist, create it
    else
    {
      // create a new archetype
      // add the component to the type
      Archetype& new_archetype = Internal_CreateArchetype(new_type);

      // move the entity to the new archetype
      Internal_MoveEntity(entity, archetype, entity_record.row, new_archetype);

      // store the component data in the archetype
      ArchetypeRecord& archetype_record = COMPONENT_INDEX[component][new_archetype.id];
      new_archetype.archetype_table[archetype_record.column].push_back(data_ptr);

      // update archetype graph
      archetype.edges[component].add = &new_archetype;
      new_archetype.edges[component].remove = &archetype;
    }

  }
}

// Do the opposite of AddComponent
template <typename T>
void FlexEngine::FlexECS::Entity::RemoveComponent()
{
  // cache the entity id
  EntityID entity = entity_id;

  // get component id
  ComponentID component = Reflection::TypeResolver<T>::Get()->name;

  // figure out the current archetype for the entity
  EntityRecord& entity_record = ENTITY_INDEX[entity];
  Archetype& archetype = *entity_record.archetype;

  // graph traversal skip
  if (
    archetype.edges.count(component) != 0 &&  // check if the edge exists
    archetype.edges[component].remove != nullptr // check if the archetype exists
  )
  {
    // get the archetype
    Archetype& next_archetype = *archetype.edges[component].remove;

    // move the entity to the new archetype
    Internal_MoveEntity(entity, archetype, entity_record.row, next_archetype);
  }
  // find or create the archetype that is a copy of the current archetype
  // without the component we want to remove
  else
  {

    // create the archetype type
    ComponentIDList new_type = archetype.type; // make copy
    new_type.erase(std::remove(new_type.begin(), new_type.end(), component), new_type.end());
    std::sort(new_type.begin(), new_type.end());

    // find the archetype
    if (ARCHETYPE_INDEX.count(new_type) != 0)
    {
      // get the archetype
      Archetype& next_archetype = ARCHETYPE_INDEX[new_type];

      // move the entity to the new archetype
      Internal_MoveEntity(entity, archetype, entity_record.row, next_archetype);

      // update archetype graph
      archetype.edges[component].remove = &next_archetype; // removing the component in the current archetype will lead to the next archetype
      next_archetype.edges[component].add = &archetype;    // adding the component to the next archetype will lead back to the current archetype
    }
    // archetype doesn't exist, create it
    else
    {
      // create a new archetype
      // add the component to the type
      Archetype& new_archetype = Internal_CreateArchetype(new_type);

      // move the entity to the new archetype
      Internal_MoveEntity(entity, archetype, entity_record.row, new_archetype);

      // update archetype graph
      archetype.edges[component].remove = &new_archetype;
      new_archetype.edges[component].add = &archetype;
    }

  }
}