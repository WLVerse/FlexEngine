// WLVERSE [https://wlverse.web.app]
// flexid.cpp
// 
// A loaded unique identifier management system.
// The first 32 bits are the ID, the next 28 bits are the generation, and the last 4 bits are the flags.
// The ID is used to identify the entity, the generation is used to check if the entity is still valid,
// and the flags are used to store additional information.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "flexid.h"

#include <vector>

namespace FlexEngine
{
  namespace ID
  {

    #pragma region Internal Functions

    static void Internal_SetGeneration(uint64_t& id, const uint32_t value)
    {
      id &= ~(static_cast<uint64_t>(MASK_GENERATION) << SHIFT_GENERATION);
      id |= (static_cast<uint64_t>(value) << SHIFT_GENERATION);
    }

    static void Internal_IncrementGeneration(uint64_t& id)
    {
      Internal_SetGeneration(id, GetGeneration(id) + 1);
    }

    static void Internal_ResetFlags(uint64_t& id)
    {
      id &= ~(static_cast<uint64_t>(MASK_FLAGS) << SHIFT_FLAGS);
    }

    #pragma endregion

    #pragma region Constructors

    __FLX_API uint64_t Create(const uint8_t flags, uint64_t& next_id, std::vector<uint64_t>& unused_ids)
    {
      uint64_t id = 0;

      // If there are any unused IDs, use them first.
      if (!unused_ids.empty())
      {
        id = unused_ids.back();
        unused_ids.pop_back();
      }
      // Otherwise, create a new ID.
      else
      {
        id = next_id++;
      }

      // Increment the generation and set the flags.
      Internal_IncrementGeneration(id);
      SetFlags(id, flags);

      return id;
    }

    __FLX_API uint64_t Create(
      const bool _FLX_ECS_ID_FLAG_ONE_,
      const bool _FLX_ECS_ID_FLAG_TWO_,
      const bool _FLX_ECS_ID_FLAG_THREE_,
      const bool _FLX_ECS_ID_FLAG_FOUR_,
      uint64_t& next_id,
      std::vector<uint64_t>& unused_ids
    )
    {
      return Create(
        (_FLX_ECS_ID_FLAG_ONE_   ? FLAG(_FLX_ECS_ID_FLAG_ONE)   : FLAG(None)) |
        (_FLX_ECS_ID_FLAG_TWO_   ? FLAG(_FLX_ECS_ID_FLAG_TWO)   : FLAG(None)) |
        (_FLX_ECS_ID_FLAG_THREE_ ? FLAG(_FLX_ECS_ID_FLAG_THREE) : FLAG(None)) |
        (_FLX_ECS_ID_FLAG_FOUR_  ? FLAG(_FLX_ECS_ID_FLAG_FOUR)  : FLAG(None)),
        next_id, unused_ids
      );
    }

    __FLX_API void Destroy(uint64_t& id, std::vector<uint64_t>& unused_ids)
    {
      unused_ids.push_back(id);
      id = 0ULL;
    }

    #pragma endregion

    #pragma region Getters

    __FLX_API void Decompose(const uint64_t id, uint32_t* out_id, uint32_t* out_generation, uint8_t* out_flags)
    {
      if (out_id) *out_id = GetID(id);
      if (out_generation) *out_generation = GetGeneration(id);
      if (out_flags) *out_flags = GetFlags(id);
    }

    __FLX_API uint32_t GetID(const uint64_t id)
    {
      return (id & MASK_ID);
    }

    __FLX_API uint32_t GetGeneration(const uint64_t id)
    {
      return (id & (static_cast<uint64_t>(MASK_GENERATION) << SHIFT_GENERATION)) >> SHIFT_GENERATION;
    }

    __FLX_API uint8_t GetFlags(const uint64_t id)
    {
      return (id & (static_cast<uint64_t>(MASK_FLAGS) << SHIFT_FLAGS)) >> SHIFT_FLAGS;
    }

    #pragma endregion

    #pragma region Setters

    __FLX_API void SetFlags(uint64_t& id, const uint8_t value)
    {
      Internal_ResetFlags(id);
      id |= (static_cast<uint64_t>(value) << SHIFT_FLAGS);
    }

    __FLX_API void SetFlags(
      uint64_t& id,
      const bool _FLX_ECS_ID_FLAG_ONE_,
      const bool _FLX_ECS_ID_FLAG_TWO_,
      const bool _FLX_ECS_ID_FLAG_THREE_,
      const bool _FLX_ECS_ID_FLAG_FOUR_)
    {
      SetFlags(
        id,
        static_cast<uint8_t>(
          (_FLX_ECS_ID_FLAG_ONE_   ? FLAG(_FLX_ECS_ID_FLAG_ONE)   : FLAG(None)) |
          (_FLX_ECS_ID_FLAG_TWO_   ? FLAG(_FLX_ECS_ID_FLAG_TWO)   : FLAG(None)) |
          (_FLX_ECS_ID_FLAG_THREE_ ? FLAG(_FLX_ECS_ID_FLAG_THREE) : FLAG(None)) |
          (_FLX_ECS_ID_FLAG_FOUR_  ? FLAG(_FLX_ECS_ID_FLAG_FOUR)  : FLAG(None))
        )
      );
    }

    #pragma endregion

    #pragma region Flag Checkers

    __FLX_API bool _FLX_ECS_ID_FLAG_ONE    (uint64_t id) { return (id & ( static_cast<uint64_t>( FLAG(_FLX_ECS_ID_FLAG_ONE)   ) << SHIFT_FLAGS )) != 0; }
    __FLX_API bool _FLX_ECS_ID_FLAG_TWO    (uint64_t id) { return (id & ( static_cast<uint64_t>( FLAG(_FLX_ECS_ID_FLAG_TWO)   ) << SHIFT_FLAGS )) != 0; }
    __FLX_API bool _FLX_ECS_ID_FLAG_THREE  (uint64_t id) { return (id & ( static_cast<uint64_t>( FLAG(_FLX_ECS_ID_FLAG_THREE) ) << SHIFT_FLAGS )) != 0; }
    __FLX_API bool _FLX_ECS_ID_FLAG_FOUR   (uint64_t id) { return (id & ( static_cast<uint64_t>( FLAG(_FLX_ECS_ID_FLAG_FOUR)  ) << SHIFT_FLAGS )) != 0; }

    #pragma endregion

  }
}
