// WLVERSE [https://wlverse.web.app]
// quaternion.h
//
// Quaternions are purely magic. 
// If you're insane and want to understand them, here are some good places to
// start: 
// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles 
// 
// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/ 
// https://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/ 
// 
// https://stackoverflow.com/questions/62943083/interpolate-between-two-quaternions-the-long-way 
// 
// Note that the quaternion is stored in the xyzw format, which is the standard
// for most modern libraries. 
// The epsilon value is redefined in the cpp, as opposed to using EPSILONf. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "mathfunctions.h"
#include "vector4.h"
#include "matrix4x4.h"

namespace FlexEngine
{

  // Note: Uses the xyzw standard
  struct __FLX_API Quaternion
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

    union __FLX_ALIGNAS(16)
    {
    #pragma warning(push)
    #pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
      struct { value_type x, y, z, w; };
    #pragma warning(pop)
      value_type data[4];
    };

#pragma region Standard Functions

    // Shorthand for common vectors

    static const Quaternion Zero;
    static const Quaternion Identity;

    // Conversion operators

    operator bool() const;

    operator Vector3() const;
    Vector3 ToEulerAngles() const;

    operator Matrix4x4() const;
    Matrix4x4 ToRotationMatrix() const;

    // Swizzle support

    //Vector4 Swizzle(const std::string& swizzle = "xyzw") const;
    //static void Swizzle(Vector4& other, const std::string& swizzle = "xyzw");

    // Stringify the vector

    std::string ToString() const;
    operator std::string() const;

#ifdef _DEBUG
    void Dump() const;
#endif

#pragma endregion

#pragma region Constructors

    Quaternion(value_type _x = 0.0f, value_type _y = 0.0f, value_type _z = 0.0f, value_type _w = 0.0f);
    Quaternion(const Quaternion& other);

    Quaternion(const Vector4& other);

    static Quaternion FromEulerAngles(const Vector3& angles_in_radians);
    static Quaternion FromEulerAnglesDeg(const Vector3& angles_in_degrees);

#pragma endregion

#pragma region Operator Overloading

    // - =
    // += -= *= /=
    // == !=
    // + - * /

    Quaternion operator-() const;

    Quaternion& operator=(const Quaternion& other);

    Quaternion& operator+=(const Quaternion& other);
    Quaternion& operator+=(const_value_type value);

    Quaternion& operator-=(const Quaternion& other);
    Quaternion& operator-=(const_value_type value);

    Quaternion& operator*=(const Quaternion& other);
    Quaternion& operator*=(const_value_type& value);

    Quaternion& operator/=(const Quaternion& other);
    Quaternion& operator/=(const_value_type value);

    bool operator==(const Quaternion& other) const;
    bool operator!=(const Quaternion& other) const;

    value_type Magnitude() const;
    value_type Length() const;
    value_type LengthSqr() const;
    Quaternion& Normalize();
    static Quaternion Normalize(const Quaternion& other);

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

#pragma region Quaternion Helper Fns

  __FLX_API Quaternion operator+(const Quaternion& point_a, const Quaternion& point_b);
  __FLX_API Quaternion operator+(Quaternion::const_value_type value, const Quaternion& point);
  __FLX_API Quaternion operator+(const Quaternion& point, Quaternion::const_value_type value);

  __FLX_API Quaternion operator-(const Quaternion& point_a, const Quaternion& point_b);
  __FLX_API Quaternion operator-(Quaternion::const_value_type value, const Quaternion& point);
  __FLX_API Quaternion operator-(const Quaternion& point, Quaternion::const_value_type value);

  __FLX_API Quaternion operator*(const Quaternion& point_a, const Quaternion& point_b);
  __FLX_API Quaternion operator*(Quaternion::const_value_type value, const Quaternion& point);
  __FLX_API Quaternion operator*(const Quaternion& point, Quaternion::const_value_type value);

  __FLX_API Quaternion operator/(const Quaternion& point_a, const Quaternion& point_b);
  __FLX_API Quaternion operator/(Quaternion::const_value_type value, const Quaternion& point);
  __FLX_API Quaternion operator/(const Quaternion& point, Quaternion::const_value_type value);

  __FLX_API std::istream& operator>>(std::istream& is, Quaternion& point);
  __FLX_API std::ostream& operator<<(std::ostream& os, const Quaternion& point);

  __FLX_API Quaternion::value_type Dot(const Quaternion& a, const Quaternion& b);
  __FLX_API Quaternion Cross(const Quaternion& a, const Quaternion& b);

  // Uses spherical linear interpolation to interpolate between two quaternions
  __FLX_API Quaternion Lerp(const Quaternion& a, const Quaternion& b, Quaternion::const_value_type t);
  __FLX_API Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, Quaternion::const_value_type max_angle);

  __FLX_API bool IsSameRotation(const Quaternion& a, const Quaternion& b, Quaternion::const_value_type epsilon = EPSILONf);

#pragma endregion

}