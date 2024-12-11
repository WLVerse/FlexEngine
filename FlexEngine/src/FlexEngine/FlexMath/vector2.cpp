// WLVERSE [https://wlverse.web.app]
// vector2.cpp
// 
// A 2D vector.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "vector2.h"

namespace FlexEngine
{

#pragma region Reflection

  FLX_REFL_REGISTER_START(Vector2)
    FLX_REFL_REGISTER_PROPERTY(x)
    FLX_REFL_REGISTER_PROPERTY(y)
  FLX_REFL_REGISTER_END;

#pragma endregion
  
#pragma region Standard Functions

  const Vector2 Vector2::Zero   = {  0,  0 };
  const Vector2 Vector2::One    = {  1,  1 };
  const Vector2 Vector2::Up     = {  0,  1 };
  const Vector2 Vector2::Down   = {  0, -1 };
  const Vector2 Vector2::Left   = { -1,  0 };
  const Vector2 Vector2::Right  = {  1,  0 };

  Vector2::operator bool() const { return *this != Zero; }
  Vector2::operator Vector1() const { return { x }; }

  Vector2 Vector2::Swizzle(const std::string& swizzle) const
  {
    // build new vector based on swizzle
    Vector2 new_vector = Vector2::Zero;
    for (size_type i = 0; i < std::min(swizzle.size(), size()); ++i)
    {
      switch (swizzle[i])
      {
      case '0': case 'x': case 'r': case 's': new_vector[i] = x; break;
      case '1': case 'y': case 'g': case 't': new_vector[i] = y; break;
      default: new_vector[i] = data[i]; break;
      }
    }

    return new_vector;
  }

  void Vector2::Swizzle(Vector2& other, const std::string& swizzle)
  {
    other = other.Swizzle(swizzle);
  }

  std::string Vector2::ToString() const
  {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
  }
  Vector2::operator std::string() const { return ToString(); }

#ifdef _DEBUG
  void Vector2::Dump() const { Log::Debug(ToString()); }
#endif

#pragma endregion

#pragma region Constructors

  Vector2::Vector2(value_type _x, value_type _y)
  {
    x = _x;
    y = _y;
  }

  Vector2::Vector2(const Vector2& other)
  {
    x = other.x;
    y = other.y;
  }

#pragma endregion

#pragma region Operator Overloading

  // - =
  // += -= *= /=
  // == !=
  // + - * /

  Vector2 Vector2::operator-() const
  {
    return { -x, -y };
  }

  Vector2& Vector2::operator=(const Vector2& other)
  {
    x = other.x;
    y = other.y;
    return *this;
  }

  Vector2& Vector2::operator+=(const Vector2& other)
  {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vector2& Vector2::operator+=(const_value_type value)
  {
    x += value;
    y += value;
    return *this;
  }

  Vector2& Vector2::operator-=(const Vector2& other)
  {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vector2& Vector2::operator-=(const_value_type value)
  {
    x -= value;
    y -= value;
    return *this;
  }

  Vector2& Vector2::operator*=(const Vector2& other)
  {
    x *= other.x;
    y *= other.y;
    return *this;
  }

  Vector2& Vector2::operator*=(const_value_type value)
  {
    x *= value;
    y *= value;
    return *this;
  }

  Vector2& Vector2::operator/=(const Vector2& other)
  {
    if (other.x == 0 || other.y == 0) return *this;
    x /= other.x;
    y /= other.y;
    return *this;
  }

  Vector2& Vector2::operator/=(const_value_type value)
  {
    if (value == 0) return *this;
    const_value_type inv = 1 / value;
    x *= inv;
    y *= inv;
    return *this;
  }

  bool Vector2::operator==(const Vector2& other) const
  {
    if constexpr (std::is_same_v<value_type, float>)
    {
      return abs(x - other.x) < EPSILONf && abs(y - other.y) < EPSILONf;
    }
    else if constexpr (std::is_same_v<value_type, double>)
    {
      return abs(x - other.x) < EPSILON && abs(y - other.y) < EPSILON;
    }
  }

  bool Vector2::operator!=(const Vector2& other) const
  {
    return !(*this == other);
  }

  Vector2 Vector2::Rotate(const_value_type radians) const
  {
    const_value_type cos = std::cos(radians);
    const_value_type sin = std::sin(radians);
    return { x * cos - y * sin, x * sin + y * cos };
  }

  Vector2 Vector2::RotateDeg(const_value_type degrees) const
  {
    return Rotate(radians(degrees));
  }

  Vector2::value_type Vector2::Magnitude() const
  {
    return std::sqrt(x * x + y * y);
  }

  Vector2::value_type Vector2::Length() const
  {
    return Magnitude();
  }

  Vector2::value_type Vector2::LengthSqr() const
  {
    return x * x + y * y;
  }

  Vector2& Vector2::Normalize()
  {
    const_value_type length = Magnitude();
    if (length == 0) return *this;
    const_value_type inv = 1 / length;
    return *this *= inv;
  }

  Vector2 Vector2::Normalize(const Vector2& other)
  {
    Vector2 result = other;
    return result.Normalize();
  }

#pragma endregion

#pragma region Passthrough Functions

  Vector2::reference Vector2::at(const Vector2::size_type index) { return data[index]; }
  Vector2::const_reference Vector2::at(const Vector2::size_type index) const { return data[index]; }
  Vector2::reference Vector2::operator[](const Vector2::size_type index) { return at(index); }
  Vector2::const_reference Vector2::operator[](const Vector2::size_type index) const { return at(index); }

  Vector2::iterator                 Vector2::begin()          { return data; }
  Vector2::const_iterator           Vector2::begin() const    { return data; }
  Vector2::const_iterator           Vector2::cbegin() const   { return data; }
  Vector2::iterator                 Vector2::end()            { return data + size(); }
  Vector2::const_iterator           Vector2::end() const      { return data + size(); }
  Vector2::const_iterator           Vector2::cend() const     { return data + size(); }
  //Vector2::reverse_iterator         Vector2::rbegin()         { return data + size() - 1; }
  //Vector2::const_reverse_iterator   Vector2::rbegin() const   { return data + size() - 1; }
  //Vector2::const_reverse_iterator   Vector2::crbegin() const  { return data + size() - 1; }
  //Vector2::reverse_iterator         Vector2::rend()           { return data - 1; }
  //Vector2::const_reverse_iterator   Vector2::rend() const     { return data - 1; }
  //Vector2::const_reverse_iterator   Vector2::crend() const    { return data - 1; }

  constexpr Vector2::size_type Vector2::size() const { return 2; }

#pragma endregion

#pragma region Vector2 Helper Fns

  Vector2 operator+(const Vector2& point_a, const Vector2& point_b)
  {
    return { point_a.x + point_b.x, point_a.y + point_b.y };
  }

  Vector2 operator+(Vector2::const_value_type value, const Vector2& point)
  {
    return { point.x + value, point.y + value };
  }

  Vector2 operator+(const Vector2& point, Vector2::const_value_type value)
  {
    return { point.x + value, point.y + value };
  }

  Vector2 operator-(const Vector2& point_a, const Vector2& point_b)
  {
    return { point_a.x - point_b.x, point_a.y - point_b.y };
  }

  Vector2 operator-(Vector2::const_value_type value, const Vector2& point)
  {
    return { value - point.x, value - point.y };
  }

  Vector2 operator-(const Vector2& point, Vector2::const_value_type value)
  {
    return { point.x - value, point.y - value };
  }

  // Dot product of two vectors
  Vector2::value_type Dot(const Vector2& a, const Vector2& b)
  {
    return a.x * b.x + a.y * b.y;
  }

  Vector2 operator*(const Vector2& point_a, const Vector2& point_b)
  {
    return { point_a.x * point_b.x, point_a.y * point_b.y };
  }

  Vector2 operator*(const Vector2& point, Vector2::const_value_type value)
  {
    return { point.x * value, point.y * value };
  }

  Vector2 operator*(Vector2::const_value_type value, const Vector2& point)
  {
    return { point.x * value, point.y * value };
  }

  //Vector2 operator/(const Vector2& point_a, const Vector2& point_b)
  //{
  //  return { point_a.x / point_b.x, point_a.y / point_b.y };
  //}

  Vector2 operator/(Vector2::const_value_type value, const Vector2& point)
  {
    if (point.x == 0 || point.y == 0) return { 0, 0 };
    return { value / point.x, value / point.y };
  }

  Vector2 operator/(const Vector2& point, Vector2::const_value_type value)
  {
    if (value == 0) return { 0, 0 };
    Vector2::const_value_type inv = 1 / value;
    return { point.x * inv, point.y * inv };
  }

  // Cross product of two vectors
  // Two crossed vectors return a scalar
  Vector2::value_type Cross(const Vector2& a, const Vector2& b)
  {
    return a.x * b.y - a.y * b.x;
  }

  // Cross product of a vector with a scalar
  // with a vector v and scalar a, both returning a vector
  Vector2 Cross(const Vector2& v, Vector2::value_type a)
  {
    return { a * v.y, -a * v.x };
  }

  Vector2 Cross(Vector2::value_type a, const Vector2& v)
  {
    return { -a * v.y, a * v.x };
  }

  Vector2::value_type Distance(const Vector2& a, const Vector2& b)
  {
    return (a - b).Magnitude();
  }

  std::istream& operator>>(std::istream& is, Vector2& point)
  {
    for (Vector2::size_type i = 0; i < point.size(); ++i)
    {
      is >> point[i];
    }
    return is;
  }

  std::ostream& operator<<(std::ostream& os, const Vector2& point)
  {
    for (Vector2::size_type i = 0; i < point.size(); ++i)
    {
      os << point[i];
      if (i < point.size() - 1) os << ' ';
    }
    return os;
  }

  Vector2 Lerp(const Vector2& a, const Vector2& b, Vector2::value_type t)
  {
    return a + (b - a) * t;
  }

#pragma endregion

#pragma region mathconversions Overloads

  Vector2 radians(const Vector2& degrees)
  {
    return { radians(degrees.x), radians(degrees.y) };
  }

  Vector2 degrees(const Vector2& radians)
  {
    return { degrees(radians.x), degrees(radians.y) };
  }

#pragma endregion

}