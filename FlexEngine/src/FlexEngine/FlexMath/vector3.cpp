// WLVERSE [https://wlverse.web.app]
// vector3.cpp
// 
// A 3D vector.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "vector3.h"

#include <algorithm> // std::min

namespace FlexEngine
{

#pragma region Reflection

  FLX_REFL_REGISTER_START(Vector3)
    FLX_REFL_REGISTER_PROPERTY(x)
    FLX_REFL_REGISTER_PROPERTY(y)
    FLX_REFL_REGISTER_PROPERTY(z)
  FLX_REFL_REGISTER_END;

#pragma endregion
  
#pragma region Standard Functions

  const Vector3 Vector3::Zero     = {  0,  0,  0 };
  const Vector3 Vector3::One      = {  1,  1,  1 };
  const Vector3 Vector3::Up       = {  0,  1,  0 };
  const Vector3 Vector3::Down     = {  0, -1,  0 };
  const Vector3 Vector3::Left     = { -1,  0,  0 };
  const Vector3 Vector3::Right    = {  1,  0,  0 };
  const Vector3 Vector3::Forward  = {  0,  0,  1 };
  const Vector3 Vector3::Back     = {  0,  0, -1 };

  Vector3::operator bool() const { return *this != Zero; }
  Vector3::operator Vector1() const { return { x }; }
  Vector3::operator Vector2() const { return { x, y }; }

  Vector3 Vector3::Swizzle(const std::string& swizzle) const
  {
    // build new vector based on swizzle
    Vector3 new_vector = Vector3::Zero;
    for (size_type i = 0; i < std::min(swizzle.size(), size()); ++i)
    {
      switch (swizzle[i])
      {
      case '0': case 'x': case 'r': case 's': new_vector[i] = x; break;
      case '1': case 'y': case 'g': case 't': new_vector[i] = y; break;
      case '2': case 'z': case 'b': case 'p': new_vector[i] = z; break;
      default: new_vector[i] = data[i]; break;
      }
    }

    return new_vector;
  }

  void Vector3::Swizzle(Vector3& other, const std::string& swizzle)
  {
    other = other.Swizzle(swizzle);
  }

  std::string Vector3::ToString() const
  {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")";
  }
  Vector3::operator std::string() const { return ToString(); }

#ifdef _DEBUG
  void Vector3::Dump() const { Log::Debug(ToString()); }
#endif

#pragma endregion

#pragma region Constructors

  Vector3::Vector3(value_type _x, value_type _y, value_type _z)
  {
    x = _x;
    y = _y;
    z = _z;
  }

  Vector3::Vector3(const Vector3& other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
  }

  Vector3::Vector3(const Vector2& xy, value_type _z)
  {
    x = xy.x;
    y = xy.y;
    z = _z;
  }

  Vector3::Vector3(value_type _x, const Vector2& yz)
  {
    x = _x;
    y = yz.x;
    z = yz.y;
  }

#pragma endregion

#pragma region Operator Overloading

  // - =
  // += -= *= /=
  // == !=
  // + - * /

  Vector3 Vector3::operator-() const
  {
    return { -x, -y, -z };
  }

  Vector3& Vector3::operator=(const Vector3& other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
  }

  Vector3& Vector3::operator+=(const Vector3& other)
  {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }

  Vector3& Vector3::operator+=(const_value_type value)
  {
    x += value;
    y += value;
    z += value;
    return *this;
  }

  Vector3& Vector3::operator-=(const Vector3& other)
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }

  Vector3& Vector3::operator-=(const_value_type value)
  {
    x -= value;
    y -= value;
    z -= value;
    return *this;
  }

  Vector3& Vector3::operator*=(const Vector3& other)
  {
    x *= other.x;
    y *= other.y;
    z *= other.z;
    return *this;
  }

  Vector3& Vector3::operator*=(const_value_type value)
  {
    x *= value;
    y *= value;
    z *= value;
    return *this;
  }

  Vector3& Vector3::operator/=(const Vector3& other)
  {
    if (other.x == 0 || other.y == 0 || other.z == 0) return *this;
    x /= other.x;
    y /= other.y;
    z /= other.z;
    return *this;
  }

  Vector3& Vector3::operator/=(const_value_type value)
  {
    if (value == 0) return *this;
    const_value_type inv = 1 / value;
    x *= inv;
    y *= inv;
    z *= inv;
    return *this;
  }

  bool Vector3::operator==(const Vector3& other) const
  {
    if constexpr (std::is_same_v<value_type, float>)
    {
      return abs(x - other.x) < EPSILONf && abs(y - other.y) < EPSILONf && abs(z - other.z) < EPSILONf;
    }
    else if constexpr (std::is_same_v<value_type, double>)
    {
      return abs(x - other.x) < EPSILON && abs(y - other.y) < EPSILON && abs(z - other.z) < EPSILON;
    }
  }

  bool Vector3::operator!=(const Vector3& other) const
  {
    return !(*this == other);
  }

  Vector3::value_type Vector3::Magnitude() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  Vector3::value_type Vector3::Length() const
  {
    return Magnitude();
  }

  Vector3::value_type Vector3::LengthSqr() const
  {
    return x * x + y * y + z * z;
  }

  Vector3& Vector3::Normalize()
  {
    const_value_type length = Magnitude();
    if (length == 0) return *this;
    const_value_type inv = 1 / length;
    return *this *= inv;
  }

  Vector3 Vector3::Normalize(const Vector3& other)
  {
    Vector3 result = other;
    return result.Normalize();
  }

#pragma endregion

#pragma region Passthrough Functions

  Vector3::reference Vector3::at(const Vector3::size_type index) { return data[index]; }
  Vector3::const_reference Vector3::at(const Vector3::size_type index) const { return data[index]; }
  Vector3::reference Vector3::operator[](const Vector3::size_type index) { return at(index); }
  Vector3::const_reference Vector3::operator[](const Vector3::size_type index) const { return at(index); }

  Vector3::iterator                 Vector3::begin() { return data; }
  Vector3::const_iterator           Vector3::begin() const { return data; }
  Vector3::const_iterator           Vector3::cbegin() const { return data; }
  Vector3::iterator                 Vector3::end() { return data + size(); }
  Vector3::const_iterator           Vector3::end() const { return data + size(); }
  Vector3::const_iterator           Vector3::cend() const { return data + size(); }
  //Vector3::reverse_iterator         Vector3::rbegin()         { return data + size() - 1; }
  //Vector3::const_reverse_iterator   Vector3::rbegin() const   { return data + size() - 1; }
  //Vector3::const_reverse_iterator   Vector3::crbegin() const  { return data + size() - 1; }
  //Vector3::reverse_iterator         Vector3::rend()           { return data - 1; }
  //Vector3::const_reverse_iterator   Vector3::rend() const     { return data - 1; }
  //Vector3::const_reverse_iterator   Vector3::crend() const    { return data - 1; }

  constexpr Vector3::size_type Vector3::size() const { return 3; }

#pragma endregion

#pragma region Vector3 Helper Fns

  Vector3 operator+(const Vector3& point_a, const Vector3& point_b)
  {
    return { point_a.x + point_b.x, point_a.y + point_b.y, point_a.z + point_b.z };
  }

  Vector3 operator+(Vector3::const_value_type value, const Vector3& point)
  {
    return { point.x + value, point.y + value, point.z + value };
  }

  Vector3 operator+(const Vector3& point, Vector3::const_value_type value)
  {
    return { point.x + value, point.y + value, point.z + value };
  }

  Vector3 operator-(const Vector3& point_a, const Vector3& point_b)
  {
    return { point_a.x - point_b.x, point_a.y - point_b.y, point_a.z - point_b.z };
  }

  Vector3 operator-(const Vector3& point, Vector3::const_value_type value)
  {
    return { point.x - value, point.y - value, point.z - value };
  }

  Vector3 operator-(Vector3::const_value_type value, const Vector3& point)
  {
    return { value - point.x, value - point.y, value - point.z };
  }

  Vector3::value_type Dot(const Vector3& point_a, const Vector3& point_b)
  {
    return point_a.x * point_b.x + point_a.y * point_b.y + point_a.z * point_b.z;
  }

  Vector3 operator*(const Vector3& point_a, const Vector3& point_b)
  {
    return { point_a.x * point_b.x, point_a.y * point_b.y, point_a.z * point_b.z };
  }

  Vector3 operator*(Vector3::const_value_type value, const Vector3& point)
  {
    return { point.x * value, point.y * value, point.z * value };
  }

  Vector3 operator*(const Vector3& point, Vector3::const_value_type value)
  {
    return { point.x * value, point.y * value, point.z * value };
  }

  //Vector3 operator/(const Vector3& point, const_value_type value)
  //{
  //  return { point.x / value, point.y / value, point.z / value };
  //}

  Vector3 operator/(Vector3::const_value_type value, const Vector3& point)
  {
    if (point.x == 0 || point.y == 0 || point.z == 0) return { 0, 0, 0 };
    return { value / point.x, value / point.y, value / point.z };
  }

  Vector3 operator/(const Vector3& point, Vector3::const_value_type value)
  {
    if (value == 0) return { 0, 0, 0 };
    Vector3::const_value_type inv = 1 / value;
    return { point.x * inv, point.y * inv, point.z * inv };
  }

  Vector3 Cross(const Vector3& a, const Vector3& b)
  {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
  }

  std::istream& operator>>(std::istream& is, Vector3& point)
  {
    for (Vector3::size_type i = 0; i < point.size(); ++i)
    {
      is >> point[i];
    }
    return is;
  }

  std::ostream& operator<<(std::ostream& os, const Vector3& point)
  {
    for (Vector3::size_type i = 0; i < point.size(); ++i)
    {
      os << point[i];
      if (i < point.size() - 1) os << " ";
    }
    return os;
  }

  Vector3 Lerp(const Vector3& a, const Vector3& b, Vector3::const_value_type t)
  {
    return a + (b - a) * t;
  }

#pragma endregion

#pragma region mathconversions Overloads

  Vector3 radians(const Vector3& degrees)
  {
    return { radians(degrees.x), radians(degrees.y), radians(degrees.z) };
  }

  Vector3 degrees(const Vector3& radians)
  {
    return { degrees(radians.x), degrees(radians.y), degrees(radians.z) };
  }

#pragma endregion

}