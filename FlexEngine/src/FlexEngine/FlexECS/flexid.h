// WLVERSE [https://wlverse.web.app]
// flexid.h
//
// A loaded unique identifier management system. 
// The first 32 bits are the ID, the next 28 bits are the generation, and the last
// 4 bits are the flags. 
// The ID is used to identify the entity, the generation is used to check if the
// entity is still valid, and the flags are used to store additional information. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include <cstdint> // uint64_t uint8_t
#include <vector>

// Helper macros.
#define CONCAT(LHS, RHS) LHS##RHS
#define FLAG(NAME) CONCAT(Flag_, NAME)

namespace FlexEngine
{
  namespace ID
  {

    // Define the flags' purpose.
    // 
    // IsDirty: The entity has been modified.
    // IsActive: The entity is active in the scene graph.
    // IsActiveSelf: The entity itself is active but its parents may not be.
    // IsSleeping: The entity is sleeping to save resources, only performing minor calculations.
    #pragma region Flags

    #define _FLX_ECS_ID_FLAG_ONE     IsDirty
    #define _FLX_ECS_ID_FLAG_ONE_    is_dirty

    #define _FLX_ECS_ID_FLAG_TWO     IsActive
    #define _FLX_ECS_ID_FLAG_TWO_    is_active

    #define _FLX_ECS_ID_FLAG_THREE   IsActiveSelf
    #define _FLX_ECS_ID_FLAG_THREE_  is_active_self

    #define _FLX_ECS_ID_FLAG_FOUR    IsSleeping
    #define _FLX_ECS_ID_FLAG_FOUR_   is_sleeping

    #pragma endregion

    #pragma region Macros

    // Set up the ID system locally in a class.
    #define FLX_ID_SETUP \
      uint64_t _flx_id_next = 1; \
      std::vector<uint64_t> _flx_id_unused;

    #pragma endregion

    #pragma region Constants

    constexpr uint32_t MASK_ID = 0xFFFFFFFF;
    constexpr uint32_t MASK_GENERATION = 0xFFFFFFF;
    constexpr uint8_t MASK_FLAGS = 0xF;

    constexpr uint64_t SHIFT_ID = 0;
    constexpr uint64_t SHIFT_GENERATION = 32;
    constexpr uint64_t SHIFT_FLAGS = 60;

    #pragma endregion

    #pragma region Enums

    enum Flags : uint8_t
    {
      FLAG(None)                    = 0,
      FLAG(_FLX_ECS_ID_FLAG_ONE)   = 1 << 0,
      FLAG(_FLX_ECS_ID_FLAG_TWO)   = 1 << 1,
      FLAG(_FLX_ECS_ID_FLAG_THREE) = 1 << 2,
      FLAG(_FLX_ECS_ID_FLAG_FOUR)  = 1 << 3
    };

    // Explicitly define the flag functions for easy access.

    __FLX_API bool _FLX_ECS_ID_FLAG_ONE(uint64_t id);
    __FLX_API bool _FLX_ECS_ID_FLAG_TWO(uint64_t id);
    __FLX_API bool _FLX_ECS_ID_FLAG_THREE(uint64_t id);
    __FLX_API bool _FLX_ECS_ID_FLAG_FOUR(uint64_t id);

    #pragma endregion

    #pragma region Constructors

    __FLX_API uint64_t Create(const uint8_t flags, uint64_t& next_id, std::vector<uint64_t>& unused_ids);
    __FLX_API uint64_t Create(
      const bool _FLX_ECS_ID_FLAG_ONE_,
      const bool _FLX_ECS_ID_FLAG_TWO_,
      const bool _FLX_ECS_ID_FLAG_THREE_,
      const bool _FLX_ECS_ID_FLAG_FOUR_,
      uint64_t& next_id,
      std::vector<uint64_t>& unused_ids
    );

    __FLX_API void Destroy(uint64_t& id, std::vector<uint64_t>& unused_ids);

    #pragma endregion

    #pragma region Getters

    __FLX_API void Decompose(const uint64_t id, uint32_t* out_id = nullptr, uint32_t* out_generation = nullptr, uint8_t* out_flags = nullptr);

    __FLX_API uint32_t GetID(const uint64_t id);
    __FLX_API uint32_t GetGeneration(const uint64_t id);
    __FLX_API uint8_t GetFlags(const uint64_t id);

    #pragma endregion

    // These functions edit in place.
    #pragma region Setters

    //__FLX_API void SetID(uint64_t& id, uint32_t value);
    //__FLX_API void SetGeneration(uint64_t& id, uint32_t value);

    __FLX_API void SetFlags(uint64_t& id, const uint8_t value);
    __FLX_API void SetFlags(
      uint64_t& id,
      const bool _FLX_ECS_ID_FLAG_ONE_,
      const bool _FLX_ECS_ID_FLAG_TWO_ = false,
      const bool _FLX_ECS_ID_FLAG_THREE_ = false,
      const bool _FLX_ECS_ID_FLAG_FOUR_ = false
    );

    #pragma endregion

  }
}