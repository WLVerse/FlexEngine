// WLVERSE [https://wlverse.web.app]
// vector3.h
// 
// A 3D vector.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

//#include "Wrapper/simd.h"
#include "vector2.h"

namespace FlexEngine
{

  struct __FLX_API Vector3
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

    #pragma warning(push)
    #pragma warning(disable: 4324) // structure was padded due to alignment specifier
    union __FLX_ALIGNAS(16)
    {
    #pragma warning(push)
    #pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
      struct { value_type x, y, z; };
      struct { value_type r, g, b; };
      struct { value_type u, v, w; };
      struct { value_type s, t, p; };
      struct { value_type first, second, third; };
    #pragma warning(pop)
      value_type data[3];
    };
    #pragma warning(pop)

#pragma region Standard Functions

    // Shorthand for common vectors
    // 
    // Unity uses left-handed coordinate system while OpenGL uses right-handed coordinate system.
    // These are following OpenGL right-handedness despite using Unity naming conventions.

    static const Vector3 Zero;
    static const Vector3 One;
    static const Vector3 Up;
    static const Vector3 Down;
    static const Vector3 Left;
    static const Vector3 Right;
    static const Vector3 Forward;
    static const Vector3 Back;

    // Conversion operators

    operator bool() const;
    operator Vector1() const;
    operator Vector2() const;

    // Swizzle support

    Vector3 Swizzle(const std::string& swizzle = "xyz") const;
    static void Swizzle(Vector3& other, const std::string& swizzle = "xyz");

    // Stringify the vector

    std::string ToString() const;
    operator std::string() const;

#ifdef _DEBUG
    void Dump() const;
#endif

#pragma endregion

#pragma region Constructors

    Vector3(value_type _x = 0.0f, value_type _y = 0.0f, value_type _z = 0.0f);
    Vector3(const Vector3& other);
    Vector3(const Vector2& xy, value_type _z = 0.0f);
    Vector3(value_type _x, const Vector2& yz);

#pragma endregion

#pragma region Operator Overloading

    // - =
    // += -= *= /=
    // == !=
    // + - * /

    Vector3 operator-() const;

    Vector3& operator=(const Vector3& other);

    Vector3& operator+=(const Vector3& other);
    Vector3& operator+=(const_value_type value);

    Vector3& operator-=(const Vector3& other);
    Vector3& operator-=(const_value_type value);

    Vector3& operator*=(const Vector3& other);
    Vector3& operator*=(const_value_type value);

    Vector3& operator/=(const Vector3& other);
    Vector3& operator/=(const_value_type value);

    bool operator==(const Vector3& other) const;
    bool operator!=(const Vector3& other) const;

    value_type Magnitude() const;
    value_type Length() const;
    value_type LengthSqr() const;

    Vector3& Normalize();
    static Vector3 Normalize(const Vector3& other);

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

#pragma region Vector3 Helper Fns

  __FLX_API Vector3 operator+(const Vector3& point_a, const Vector3& point_b);
  __FLX_API Vector3 operator+(Vector3::const_value_type value, const Vector3& point);
  __FLX_API Vector3 operator+(const Vector3& point, Vector3::const_value_type value);

  __FLX_API Vector3 operator-(const Vector3& point_a, const Vector3& point_b);
  __FLX_API Vector3 operator-(Vector3::const_value_type value, const Vector3& point);
  __FLX_API Vector3 operator-(const Vector3& point, Vector3::const_value_type value);

  __FLX_API Vector3::value_type Dot(const Vector3& point_a, const Vector3& point_b);
  __FLX_API Vector3 operator*(const Vector3& point_a, const Vector3& point_b);
  __FLX_API Vector3 operator*(Vector3::const_value_type value, const Vector3& point);
  __FLX_API Vector3 operator*(const Vector3& point, Vector3::const_value_type value);

  //__FLX_API Vector3 operator/(const Vector3& point, const_value_type value);
  __FLX_API Vector3 operator/(Vector3::const_value_type value, const Vector3& point);
  __FLX_API Vector3 operator/(const Vector3& point, Vector3::const_value_type value);

  __FLX_API Vector3 Cross(const Vector3& a, const Vector3& b);

  __FLX_API std::istream& operator>>(std::istream& is, Vector3& point);
  __FLX_API std::ostream& operator<<(std::ostream& os, const Vector3& point);

  __FLX_API Vector3 Lerp(const Vector3& a, const Vector3& b, Vector3::const_value_type t);

#pragma endregion

#pragma region mathconversions Overloads

  __FLX_API Vector3 radians(const Vector3& degrees);
  __FLX_API Vector3 degrees(const Vector3& radians);

#pragma endregion

}