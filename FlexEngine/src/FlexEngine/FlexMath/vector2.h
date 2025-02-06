// WLVERSE [https://wlverse.web.app]
// vector2.h
//
// A 2D vector. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "mathconstants.h" // PI, EPSILON
#include "mathconversions.h" // radians, degrees

#include "vector1.h"

#include "Reflection/base.h"

#include <cmath> // std::cos, std::sin, std::sqrt
#include <iostream> // std::istream, std::ostream

namespace FlexEngine
{

  struct __FLX_API Vector2
  { FLX_REFL_SERIALIZABLE

    // Using directives

    using value_type = Vector1;
    using const_value_type = const value_type;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const_value_type&;
    using pointer = value_type*;
    using const_pointer = const_value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = iterator;
    using const_reverse_iterator = const_iterator;

    // Data members
    
    union __FLX_ALIGNAS(8)
    {
    #pragma warning(push)
    #pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
      struct { value_type x, y; };
      struct { value_type r, g; };
      struct { value_type u, v; };
      struct { value_type s, t; };
      struct { value_type first, second; };
    #pragma warning(pop)
      value_type data[2];
    };

#pragma region Standard Functions

    // Shorthand for common vectors
    // 
    // Unity uses left-handed coordinate system while OpenGL uses right-handed coordinate system.
    // These are following OpenGL right-handedness despite using Unity naming conventions.

    static const Vector2 Zero;
    static const Vector2 One;
    static const Vector2 Up;
    static const Vector2 Down;
    static const Vector2 Left;
    static const Vector2 Right;

    // Conversion operators

    operator bool() const;
    operator Vector1() const;

    // Swizzle support

    Vector2 Swizzle(const std::string& swizzle = "xy") const;
    static void Swizzle(Vector2& other, const std::string& swizzle = "xy");

    // Stringify the vector

    std::string ToString() const;
    operator std::string() const;

#ifdef _DEBUG
    void Dump() const;
#endif

#pragma endregion

#pragma region Constructors

    Vector2(value_type _x = 0.0f, value_type _y = 0.0f);
    Vector2(const Vector2& other);

#pragma endregion

#pragma region Operator Overloading

    // - =
    // += -= *= /=
    // == !=
    // + - * /

    Vector2 operator-() const;

    Vector2& operator=(const Vector2& other);

    Vector2& operator+=(const Vector2& other);
    Vector2& operator+=(const_value_type value);

    Vector2& operator-=(const Vector2& other);
    Vector2& operator-=(const_value_type value);

    Vector2& operator*=(const Vector2& other);
    Vector2& operator*=(const_value_type value);

    Vector2& operator/=(const Vector2& other);
    Vector2& operator/=(const_value_type value);

    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;

    Vector2 Rotate(const_value_type radians) const;
    Vector2 RotateDeg(const_value_type degrees) const;

    value_type Magnitude() const;
    value_type Length() const;
    value_type LengthSqr() const;

    Vector2& Normalize();
    static Vector2 Normalize(const Vector2& other);

#pragma endregion

#pragma region Passthrough Functions

    // Accessors

    reference at(const size_type index);
    const_reference at(const size_type index) const;
    reference operator[](const size_type index);
    const_reference operator[](const size_type index) const;

    // Iterators

    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;
    //reverse_iterator rbegin();
    //const_reverse_iterator rbegin() const;
    //const_reverse_iterator crbegin() const;
    //reverse_iterator rend();
    //const_reverse_iterator rend() const;
    //const_reverse_iterator crend() const;

    // Capacity

    constexpr size_type size() const;

#pragma endregion

  };

#pragma region Vector2 Helper Fns

  __FLX_API Vector2 operator+(const Vector2& point_a, const Vector2& point_b);
  __FLX_API Vector2 operator+(Vector2::const_value_type value, const Vector2& point);
  __FLX_API Vector2 operator+(const Vector2& point, Vector2::const_value_type value);

  __FLX_API Vector2 operator-(const Vector2& point_a, const Vector2& point_b);
  __FLX_API Vector2 operator-(Vector2::const_value_type value, const Vector2& point);
  __FLX_API Vector2 operator-(const Vector2& point, Vector2::const_value_type value);

  // Dot product of two vectors
  __FLX_API Vector2::value_type Dot(const Vector2& a, const Vector2& b);

  __FLX_API Vector2 operator*(const Vector2& point_a, const Vector2& point_b);
  __FLX_API Vector2 operator*(const Vector2& point, Vector2::const_value_type value);
  __FLX_API Vector2 operator*(Vector2::const_value_type value, const Vector2& point);

  //__FLX_API Vector2 operator/(const Vector2& point_a, const Vector2& point_b);
  __FLX_API Vector2 operator/(Vector2::const_value_type value, const Vector2& point);
  __FLX_API Vector2 operator/(const Vector2& point, Vector2::const_value_type value);

  // Cross product of two vectors
  // Two crossed vectors return a scalar
  __FLX_API Vector2::value_type Cross(const Vector2& a, const Vector2& b);
  // Cross product of a vector with a scalar
  // with a vector v and scalar a, both returning a vector
  __FLX_API Vector2 Cross(const Vector2& v, Vector2::value_type a);
  // Cross product of a vector with a scalar
  // with a vector v and scalar a, both returning a vector
  __FLX_API Vector2 Cross(Vector2::value_type a, const Vector2& v);
  __FLX_API Vector2::value_type Distance(const Vector2& a, const Vector2& b);

  __FLX_API std::istream& operator>>(std::istream& is, Vector2& point);
  __FLX_API std::ostream& operator<<(std::ostream& os, const Vector2& point);

  __FLX_API Vector2 Lerp(const Vector2& a, const Vector2& b, Vector2::value_type t);

#pragma endregion

#pragma region mathconversions Overloads
  
  __FLX_API Vector2 radians(const Vector2& degrees);
  __FLX_API Vector2 degrees(const Vector2& radians);

#pragma endregion

}