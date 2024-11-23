#include "datastructures.h"

namespace FlexEngine
{
  namespace FlexECS
  {

    #pragma region Reflection

    FLX_REFL_REGISTER_START(Archetype)
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
      FLX_REFL_REGISTER_PROPERTY(archetype_index)
      FLX_REFL_REGISTER_PROPERTY(entity_index)
      FLX_REFL_REGISTER_PROPERTY(component_index)
      FLX_REFL_REGISTER_PROPERTY(string_storage)
      FLX_REFL_REGISTER_PROPERTY(string_storage_free_list)
    FLX_REFL_REGISTER_END;

    #pragma endregion

    __FLX_API ComponentData<void> Internal_CreateComponentData(std::size_t size, void* data)
    {
      // Create a new data structure
      void* ptr = new char[sizeof(std::size_t) + size];
      if (!ptr)
      {
        FLX_ASSERT(false, "Failed to allocate memory for component data!");
        return nullptr;
      }

      // Copy the size of the data
      memcpy(ptr, &size, sizeof(std::size_t));

      // Copy the data
      memcpy(reinterpret_cast<std::size_t*>(ptr) + 1, data, size);

      return ComponentData<void>(ptr, [](void* ptr) { delete[] reinterpret_cast<char*>(ptr); });
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