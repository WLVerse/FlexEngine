// WLVERSE [https://wlverse.web.app]
// entity.cpp
// 
// Implementation of Entity class and functions that assist its management in the ECS
//
// AUTHORS
// [90%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Everything else
// [10%] Kuan Yew Chong (yewchong.k\@digipen.edu)
//  - Update internal_addarchetype for updating caches
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "datastructures.h"

namespace FlexEngine
{
  namespace FlexECS
  {

    #pragma region Reflection

    FLX_REFL_REGISTER_START(Entity)
      FLX_REFL_REGISTER_PROPERTY(entity_id)
      FLX_REFL_REGISTER_END;

    #pragma endregion

    // static member initialization
    Entity Entity::Null = Entity();

    Entity::Entity()
      : entity_id(0)
    {
    }

    Entity::Entity(EntityID id)
      : entity_id(id)
    {
    }

    EntityID& Entity::Get()
    {
      return entity_id;
    }

    #pragma region Operator Overloads

    bool Entity::operator==(const Entity& other) const
    {
      return entity_id == other.entity_id;
    }

    bool Entity::operator!=(const Entity& other) const
    {
      return entity_id != other.entity_id;
    }

    bool Entity::operator<(const Entity& other) const
    {
      // compare their names (in std::string component)
      // makes a copy because GetComponent is not const, gets the name component, and compares the strings
      Entity lhs = *this;
      Entity rhs = other;
      return *(lhs.GetComponent<std::string>()) < *(rhs.GetComponent<std::string>());
    }

    Entity::operator EntityID() const
    {
      return entity_id;
    }

    #pragma endregion

    #pragma region Internal Functions

    // Assume the component is not in the archetype and
    // the ComponentIDList is sorted
    Archetype& Entity::Internal_CreateArchetype(ComponentIDList type)
    {
      // create a new archetype
      Archetype& archetype = ARCHETYPE_INDEX[type];

      archetype.id = ARCHETYPE_INDEX.size() - 1;
      archetype.type = type;
      archetype.archetype_table.reserve(type.size());
      // edges are lazily instantiated

      // create a new archetype record for each component
      for (std::size_t i = 0; i < archetype.type.size(); i++)
      {
        //Log::Flow("Create new column (" + std::to_string(i) + ")");
        COMPONENT_INDEX[archetype.type[i]][archetype.id] = { i };
        archetype.archetype_table.push_back(Column()); // create a column for each component
      }

      // update caches with this archetype if needed
      for (auto& a : Scene::GetActiveScene()->query_cache)
      {
        // check if query is a subset of the new archetype
        bool skip = false;

        for (auto& component_to_find : a.first)
        {
          if (std::find(archetype.type.begin(), archetype.type.end(), component_to_find) == archetype.type.end())
          {
            skip = true;
            break;
          }
        }

        if (!skip)
        {
          a.second.AddPtr(reinterpret_cast<std::vector<FlexEngine::FlexECS::Entity>*>(&archetype.entities));
        }
      }

      // debugging
      std::stringstream ss;
      ss << "Created a new archetype: ";
      for (ComponentID c : archetype.type)
      {
        ss << c;
        if (c != archetype.type.back()) ss << ", ";
      }
      Log::Debug(ss);

      return archetype;
    }


    // Three steps required to move an entity to a new archetype
    // 1. Add the entity to the destination archetype's columns and entities vector
    // 2. Remove the entity from the source archetype's columns and entities vector
    // 3. Update entity_index to reflect the entity's new archetype and row
    // 
    // This is a slower process, if the component just needs to be disabled
    // like in the properties inspector, use flags instead
    void Entity::Internal_MoveEntity(EntityID entity, Archetype& from, size_t from_row, Archetype& to)
    {
      // 1. Add the entity to the destination archetype's columns and entities vector
      // If the destination archetype does not have the component, it is being removed from the entity
      #pragma region Step 1

      for (size_t i = 0; i < from.archetype_table.size(); i++)
      {
        // guard
        // The destination archetype does not have the component
        // This means the component is being removed from the entity
        if (COMPONENT_INDEX[from.type[i]].count(to.id) == 0) continue;

        // Get the source component data
        ComponentData<void> source_data = from.archetype_table[i][from_row];

        // Copy the source component data to the destination archetype
        size_t destination_column_index = COMPONENT_INDEX[from.type[i]][to.id].column;
        to.archetype_table[destination_column_index].push_back(source_data);
      }

      // Add the entity to the entities vector
      to.entities.push_back(entity);

      #pragma endregion


      // 2. Remove the entity from the source archetype's columns and entities vector
      // The same code is being used in DestroyEntity
      // Empty archetypes won't be cleaned up.
      // This is by design to avoid the overhead of creating and destroying archetypes frequently.
      #pragma region Step 2

      size_t last_row_index = from.archetype_table[0].size() - 1;

      // Handle the case where the entity is the last entity in the archetype
      if (from_row == last_row_index)
      {
        for (size_t i = 0; i < from.archetype_table.size(); i++)
        {
          from.archetype_table[i].pop_back();
        }
      }
      else
      {
        // Swap the entity with the last entity in the archetype and pop it
        // Using swap-and-pop is more performant than erase() since it requires shifting
        // all subsequent elements forward.
        // O(1) complexity for swap-and-pop vs O(n) complexity for erase()
        for (size_t i = 0; i < from.archetype_table.size(); i++)
        {
          std::swap(from.archetype_table[i][from_row], from.archetype_table[i][last_row_index]);
          from.archetype_table[i].pop_back();
        }

        // Update entity_index for the swapped entity if necessary
        if (from_row < last_row_index)
        {
          EntityID swapped_entity = from.entities[last_row_index];
          ENTITY_INDEX[swapped_entity].row = from_row;

          // Replace the entity's row in the entities vector
          from.entities[from_row] = swapped_entity;
        }
      }

      // Pop the entity from the entities vector
      from.entities.pop_back();

      #pragma endregion


      // 3. Update entity_index to reflect the entity's new archetype and row
      ENTITY_INDEX[entity].archetype = &to;
      ENTITY_INDEX[entity].archetype_id = to.id;
      ENTITY_INDEX[entity].row = to.entities.size() - 1;
    }

    #pragma endregion

  }
}