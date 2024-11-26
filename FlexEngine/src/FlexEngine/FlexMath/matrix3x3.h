// WLVERSE [https://wlverse.web.app]
// matrix3x3.h
// 
// 
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "matrix1x1.h"
#include "vector3.h"

namespace FlexEngine
{

    // FlexEngine::Matrix3x3 (along with glm and OpenGL) is column-major.
    // | 0  3  6 |
    // | 1  4  7 |
    // | 2  5  8 |
    struct __FLX_API Matrix3x3
    {
        FLX_REFL_SERIALIZABLE

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

        union //__FLX_ALIGNAS(64)
        {
            #pragma warning(push)
            #pragma warning(disable: 4201) // nonstandard extension used: nameless struct/union
            struct
            {
                value_type m00, m01, m02;
                value_type m10, m11, m12;
                value_type m20, m21, m22;
            };
            struct
            {
                Vector3 m0, m1, m2;
            };
            #pragma warning(pop)
            value_type data[9];
        };

        #pragma region Standard Functions

        // Shorthand for common matrices

        static const Matrix3x3 Zero;
        static const Matrix3x3 Identity;

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

        Matrix3x3(
          value_type _m00 = 0.0f, value_type _m01 = 0.0f, value_type _m02 = 0.0f,
          value_type _m10 = 0.0f, value_type _m11 = 0.0f, value_type _m12 = 0.0f,
          value_type _m20 = 0.0f, value_type _m21 = 0.0f, value_type _m22 = 0.0f
        );
        Matrix3x3(
          const Vector3& _m0,
          const Vector3& _m1 = Vector3::Zero,
          const Vector3& _m2 = Vector3::Zero
        );
        Matrix3x3(const Matrix3x3& other);

        #pragma endregion

        #pragma region Operator Overloading

        // - =
        // += -= *= /=
        // == !=
        // + - * /

        Matrix3x3& operator=(const Matrix3x3& other);
        Matrix3x3& operator+=(const Matrix3x3& other);
        Matrix3x3& operator-=(const Matrix3x3& other);
        Matrix3x3& operator*=(const Matrix3x3& other);
        Matrix3x3& operator*=(const_value_type& value);
        Matrix3x3& operator/=(const_value_type value);

        bool operator==(const Matrix3x3& other) const;
        bool operator!=(const Matrix3x3& other) const;

        Matrix3x3 Transpose() const;
        value_type Determinant() const;
        Matrix3x3 Inverse() const;

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

        // Capacity

        constexpr size_type size() const;

        #pragma endregion

        #pragma region Transformation Functions

        // Common

        Matrix3x3& Translate(const Vector3& translation);
        Matrix3x3& Rotate(const_value_type radians, const Vector3& rotation_axis);
        Matrix3x3& RotateDeg(const_value_type degrees, const Vector3& rotation_axis);
        Matrix3x3& RotateX(const_value_type radians);
        Matrix3x3& RotateY(const_value_type radians);
        Matrix3x3& RotateZ(const_value_type radians);
        Matrix3x3& RotateXDeg(const_value_type degrees);
        Matrix3x3& RotateYDeg(const_value_type degrees);
        Matrix3x3& RotateZDeg(const_value_type degrees);
        Matrix3x3& Scale(const Vector3& scale);

        // Static

        static Matrix3x3 Translate(const Matrix3x3& matrix, const Vector3& translation);
        static Matrix3x3 Rotate(const Matrix3x3& matrix, const_value_type radians, const Vector3& rotation_axis);
        static Matrix3x3 RotateDeg(const Matrix3x3& matrix, const_value_type degrees, const Vector3& rotation_axis);
        static Matrix3x3 RotateX(const Matrix3x3& matrix, const_value_type radians);
        static Matrix3x3 RotateY(const Matrix3x3& matrix, const_value_type radians);
        static Matrix3x3 RotateZ(const Matrix3x3& matrix, const_value_type radians);
        static Matrix3x3 RotateXDeg(const Matrix3x3& matrix, const_value_type degrees);
        static Matrix3x3 RotateYDeg(const Matrix3x3& matrix, const_value_type degrees);
        static Matrix3x3 RotateZDeg(const Matrix3x3& matrix, const_value_type degrees);
        static Matrix3x3 Scale(const Matrix3x3& matrix, const Vector3& scale);

        // Others

        static Matrix3x3 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
        static Matrix3x3 Perspective(
          const_value_type fov, const_value_type aspect,
          const_value_type near, const_value_type far
        );
        static Matrix3x3 Orthographic(
          const_value_type left, const_value_type right, const_value_type bottom, const_value_type top,
          const_value_type near, const_value_type far
        );

        #pragma endregion


    };

    #pragma region Matrix3x3 Helper Fns

    __FLX_API Matrix3x3 operator+(const Matrix3x3& matrix_a, const Matrix3x3& matrix_b);
    __FLX_API Matrix3x3 operator-(const Matrix3x3& matrix_a, const Matrix3x3& matrix_b);
    __FLX_API Matrix3x3 operator*(const Matrix3x3& matrix_a, const Matrix3x3& matrix_b);
    __FLX_API Matrix3x3 operator*(Matrix3x3::const_value_type value, const Matrix3x3& matrix);
    __FLX_API Matrix3x3 operator*(const Matrix3x3& matrix, Matrix3x3::const_value_type value);
    __FLX_API Matrix3x3 operator/(const Matrix3x3& matrix, Matrix3x3::const_value_type value);

    __FLX_API std::istream& operator>>(std::istream& is, Matrix3x3& matrix);
    __FLX_API std::ostream& operator<<(std::ostream& os, const Matrix3x3& matrix);

    #pragma endregion

}