// WLVERSE [https://wlverse.web.app]
// datastructures.cpp
// 
// The FlexEngine Entity-Component-System (ECS) implementation
// 
// This uses the archetype pattern to store entities and components in a way
// that is cache friendly and allows for efficient queries.
// 
// References:
// https://ajmmertens.medium.com/building-an-ecs-1-where-are-my-entities-and-components-63d07c7da742
// https://ajmmertens.medium.com/building-an-ecs-2-archetypes-and-vectorization-fe21690805f9
// https://www.youtube.com/watch?v=71RSWVyOMEY
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

    #pragma region Reflection

    //FLX_REFL_REGISTER_START(Archetype)
    //  FLX_REFL_REGISTER_PROPERTY(id)
    //  FLX_REFL_REGISTER_PROPERTY(type)
    //  FLX_REFL_REGISTER_PROPERTY(archetype_table)
    //  FLX_REFL_REGISTER_PROPERTY(entities)
    //  //FLX_REFL_REGISTER_PROPERTY(edges)
    //FLX_REFL_REGISTER_END;

    FLX_REFL_REGISTER_START(_Archetype)
      FLX_REFL_REGISTER_PROPERTY(id)
      FLX_REFL_REGISTER_PROPERTY(type)
      FLX_REFL_REGISTER_PROPERTY(archetype_table)
      FLX_REFL_REGISTER_PROPERTY(entities)
      //FLX_REFL_REGISTER_PROPERTY(edges)
    FLX_REFL_REGISTER_END;

    FLX_REFL_REGISTER_START(EntityRecord)
      //FLX_REFL_REGISTER_PROPERTY(archetype)
      FLX_REFL_REGISTER_PROPERTY(archetype_id)
      FLX_REFL_REGISTER_PROPERTY(row)
    FLX_REFL_REGISTER_END;

    FLX_REFL_REGISTER_START(ArchetypeRecord)
      FLX_REFL_REGISTER_PROPERTY(column)
    FLX_REFL_REGISTER_END;

    FLX_REFL_REGISTER_START(Scene)
      FLX_REFL_REGISTER_PROPERTY(_flx_id_next)
      FLX_REFL_REGISTER_PROPERTY(_flx_id_unused)
      //FLX_REFL_REGISTER_PROPERTY(archetype_index)
      FLX_REFL_REGISTER_PROPERTY(_archetype_index)
      FLX_REFL_REGISTER_PROPERTY(entity_index)
      FLX_REFL_REGISTER_PROPERTY(component_index)
    FLX_REFL_REGISTER_END;

    #pragma endregion

    __FLX_API ComponentData<void> Internal_CreateComponentData(std::size_t size, void* data)
    {
      // Create a new data structure
      void* ptr = ::operator new(sizeof(std::size_t) + size);
      if (!ptr)
      {
        FLX_ASSERT(false, "Failed to allocate memory for component data!");
        return nullptr;
      }

      // Copy the size of the data
      memcpy(ptr, &size, sizeof(std::size_t));

      // Copy the data
      memcpy(reinterpret_cast<std::size_t*>(ptr) + 1, data, size);

      return ComponentData<void>(ptr, [](void* ptr) { ::operator delete(ptr); });
    }

    // Special case for std::string
    // Copy the entire std::string object
    __FLX_API ComponentData<void> Internal_CreateComponentData_StdString(void* data)
    {
      // Cast to std::string
      std::string& data_string = *reinterpret_cast<std::string*>(data);

      // Create a new data structure
      std::size_t size = sizeof(std::string);
      void* ptr = new char[sizeof(std::size_t) + size];
      if (!ptr)
      {
        FLX_ASSERT(false, "Failed to allocate memory for component data!");
        return nullptr;
      }

      // Copy the size of the data 
      memcpy(ptr, &size, sizeof(std::size_t));

      // Uses the placement new operator to copy the std::string object
      // This operator constructs an object in the memory allocated by the new operator
      // It is important to note that the std::string object is not copied, but the data inside the object is copied
      void* string_memory = reinterpret_cast<char*>(ptr) + sizeof(size_t);
      new (string_memory) std::string(data_string); // Placement new

      return ComponentData<void>(
        ptr,
        [](void* ptr)
        {
          // Split the ptr first to free the std::string*
          void* data_ptr = reinterpret_cast<void*>(reinterpret_cast<std::size_t*>(ptr) + 1);

          // Doing this will clear the memory allocated for the std::string data
          std::string* data_string = reinterpret_cast<std::string*>(data_ptr);

          // Call the destructor of the std::string object
          data_string->~basic_string();

          delete[] reinterpret_cast<char*>(ptr);
        }
      );
    }

    __FLX_API std::pair<std::size_t, void*> Internal_GetComponentData(ComponentData<void> data)
    {
      std::size_t* size_ptr = reinterpret_cast<std::size_t*>(data.get());
      return {
        // Get the size of the data
        *size_ptr,
        // Get the data pointer
        reinterpret_cast<void*>(size_ptr + 1)
      };
    }

  }
}