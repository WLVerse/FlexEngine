// WLVERSE [https://wlverse.web.app]
// simd.h
// 
// Wrapper for SIMD operations to make it more readable.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <immintrin.h>

// Wrapper for SIMD operations.
// 
// __m128 is a 128-bit SIMD register type.
// ps means packed single-precision floating-point values.
// pd means packed double-precision floating-point values.

#define SIMD_128BIT __m128

#define SIMD_ADD(A, B) _mm_add_ps(A, B)
#define SIMD_SUB(A, B) _mm_sub_ps(A, B)
#define SIMD_MUL(A, B) _mm_mul_ps(A, B)
#define SIMD_DIV(A, B) _mm_div_ps(A, B)

#define SIMD_SET(X, Y, Z, W) _mm_set_ps(W, Z, Y, X)
#define SIMD_GET(OUT, RESULT) _mm_storeu_ps(OUT, RESULT)

#define SIMD_VECTOR_GET(OUT_X, OUT_Y, OUT_Z, FUNCTION) \
  float _simd_out[4]{}; \
  FUNCTION; \
  OUT_X = _simd_out[3]; \
  OUT_Y = _simd_out[2]; \
  OUT_Z = _simd_out[1];

//#define SIMD_VECTOR_GET(OUT_X, OUT_Y, OUT_Z, OUT_W, FUNCTION) \
//  float _simd_out[4]{}; \
//  FUNCTION; \
//  OUT_X = _simd_out[3]; \
//  OUT_Y = _simd_out[2]; \
//  OUT_Z = _simd_out[1]; \
//  OUT_W = _simd_out[0];

#define SIMD_VECTOR_ADD(AX, AY, AZ, AW, BX, BY, BZ, BW) \
  SIMD_GET(_simd_out, \
    SIMD_ADD( \
      SIMD_SET(AX, AY, AZ, AW), \
      SIMD_SET(BX, BY, BZ, BW) \
    ) \
  )
