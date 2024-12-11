// WLVERSE [https://wlverse.web.app]
// matrix4x4.h
// 
// A 4x4 matrix.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "matrix1x1.h"
#include "vector4.h"

namespace FlexEngine
{

  // FlexEngine::Matrix4x4 (along with glm and OpenGL) is column-major.
  // | 0  4  8 12 |
  // | 1  5  9 13 |
  // | 2  6 10 14 |
  // | 3  7 11 15 |
  struct __FLX_API Matrix4x4
  { FLX_REFL_SERIALIZABLE

    // Using directives

    using value_type = Matrix1x1;
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

    union __FLX_ALIGNAS(64)
    {
      #pragma warning(push)
      #pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
      struct
      {
        value_type m00, m01, m02, m03;
        value_type m10, m11, m12, m13;
        value_type m20, m21, m22, m23;
        value_type m30, m31, m32, m33;
      };
      struct
      {
        Vector4 m0, m1, m2, m3;
      };
      #pragma warning(pop)
      value_type data[16];
    };

#pragma region Standard Functions

    // Shorthand for common matrices

    static const Matrix4x4 Zero;
    static const Matrix4x4 Identity;

    // Conversion operators


    // Stringify the matrix

    std::string ToString() const;
    std::string ToPrettyString() const; // Pretty print
    operator std::string() const;

#ifdef _DEBUG
    void Dump() const;
#endif

#pragma endregion

#pragma region Constructors

    Matrix4x4(
      value_type _m00 = 0.0f, value_type _m01 = 0.0f, value_type _m02 = 0.0f, value_type _m03 = 0.0f,
      value_type _m10 = 0.0f, value_type _m11 = 0.0f, value_type _m12 = 0.0f, value_type _m13 = 0.0f,
      value_type _m20 = 0.0f, value_type _m21 = 0.0f, value_type _m22 = 0.0f, value_type _m23 = 0.0f,
      value_type _m30 = 0.0f, value_type _m31 = 0.0f, value_type _m32 = 0.0f, value_type _m33 = 0.0f
    );
    Matrix4x4(
      const Vector4& _m0,
      const Vector4& _m1 = Vector4::Zero,
      const Vector4& _m2 = Vector4::Zero,
      const Vector4& _m3 = Vector4::Zero
    );
    Matrix4x4(const Matrix4x4& other);

#pragma endregion

#pragma region Operator Overloading

    // - =
    // += -= *= /=
    // == !=
    // + - * /

    //Matrix4x4 operator-() const;

    Matrix4x4& operator=(const Matrix4x4& other);

    Matrix4x4& operator+=(const Matrix4x4& other);
    //Matrix4x4& operator+=(const_value_type value);

    Matrix4x4& operator-=(const Matrix4x4& other);
    //Matrix4x4& operator-=(const_value_type value);

    Matrix4x4& operator*=(const Matrix4x4& other);
    Matrix4x4& operator*=(const_value_type& value);

    //Matrix4x4& operator/=(const Matrix4x4& other);
    Matrix4x4& operator/=(const_value_type value);

    bool operator==(const Matrix4x4& other) const;
    bool operator!=(const Matrix4x4& other) const;

    Matrix4x4 Transpose() const;
    value_type Determinant() const;
    Matrix4x4 Inverse() const;

#pragma endregion

#pragma region Passthrough Functions

    // Accessors

    reference at(const size_type index);
    const_reference at(const size_type index) const;
    reference operator[](const size_type index);
    const_reference operator[](const size_type index) const;

    // Special Matrix Accessors

    reference at(const size_type row, const size_type column);
    const_reference at(const size_type row, const size_type column) const;
    reference operator()(const size_type row, const size_type column);
    const_reference operator()(const size_type row, const size_type column) const;

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

#pragma region Transformation Functions

    // Common

    Matrix4x4& Translate(const Vector3& translation);
    Matrix4x4& Rotate(const_value_type radians, const Vector3& rotation_axis);
    Matrix4x4& RotateDeg(const_value_type degrees, const Vector3& rotation_axis);
    Matrix4x4& RotateX(const_value_type radians);
    Matrix4x4& RotateY(const_value_type radians);
    Matrix4x4& RotateZ(const_value_type radians);
    Matrix4x4& RotateXDeg(const_value_type degrees);
    Matrix4x4& RotateYDeg(const_value_type degrees);
    Matrix4x4& RotateZDeg(const_value_type degrees);
    Matrix4x4& Scale(const Vector3& scale);

    // Static

    static Matrix4x4 Translate(const Matrix4x4& matrix, const Vector3& translation);
    static Matrix4x4 Rotate(const Matrix4x4& matrix, const_value_type radians, const Vector3& rotation_axis);
    static Matrix4x4 RotateDeg(const Matrix4x4& matrix, const_value_type degrees, const Vector3& rotation_axis);
    static Matrix4x4 RotateX(const Matrix4x4& matrix, const_value_type radians);
    static Matrix4x4 RotateY(const Matrix4x4& matrix, const_value_type radians);
    static Matrix4x4 RotateZ(const Matrix4x4& matrix, const_value_type radians);
    static Matrix4x4 RotateXDeg(const Matrix4x4& matrix, const_value_type degrees);
    static Matrix4x4 RotateYDeg(const Matrix4x4& matrix, const_value_type degrees);
    static Matrix4x4 RotateZDeg(const Matrix4x4& matrix, const_value_type degrees);
    static Matrix4x4 Scale(const Matrix4x4& matrix, const Vector3& scale);

    // Others

    static Matrix4x4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
    static Matrix4x4 Perspective(
      const_value_type fov, const_value_type aspect,
      const_value_type near, const_value_type far
    );
    static Matrix4x4 Orthographic(
      const_value_type left, const_value_type right, const_value_type bottom, const_value_type top,
      const_value_type near, const_value_type far
    );

#pragma endregion


  };

#pragma region Matrix4x4 Helper Fns

  __FLX_API Matrix4x4 operator+(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b);
  //__FLX_API Matrix4x4 operator+(Matrix4x4::const_value_type value, const Matrix4x4& matrix);
  //__FLX_API Matrix4x4 operator+(const Matrix4x4& matrix, Matrix4x4::const_value_type value);

  __FLX_API Matrix4x4 operator-(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b);
  //__FLX_API Matrix4x4 operator-(Matrix4x4::const_value_type value, const Matrix4x4& matrix);
  //__FLX_API Matrix4x4 operator-(const Matrix4x4& matrix, Matrix4x4::const_value_type value);

  __FLX_API Matrix4x4 operator*(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b);
  __FLX_API Matrix4x4 operator*(Matrix4x4::const_value_type value, const Matrix4x4& matrix);
  __FLX_API Matrix4x4 operator*(const Matrix4x4& matrix, Matrix4x4::const_value_type value);

  //__FLX_API Matrix4x4 operator/(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b);
  //__FLX_API Matrix4x4 operator/(Matrix4x4::const_value_type value, const Matrix4x4& matrix);
  __FLX_API Matrix4x4 operator/(const Matrix4x4& matrix, Matrix4x4::const_value_type value);

  __FLX_API std::istream& operator>>(std::istream& is, Matrix4x4& matrix);
  __FLX_API std::ostream& operator<<(std::ostream& os, const Matrix4x4& matrix);

#pragma endregion

}