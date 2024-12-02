// WLVERSE [https://wlverse.web.app]
// matrix3x3.cpp
// 
// Implementation of the 3x3 matrix class.
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "matrix3x3.h"

namespace FlexEngine
{

    #pragma region Reflection

    FLX_REFL_REGISTER_START(Matrix3x3)
        FLX_REFL_REGISTER_PROPERTY(m00)
        FLX_REFL_REGISTER_PROPERTY(m01)
        FLX_REFL_REGISTER_PROPERTY(m02)
        FLX_REFL_REGISTER_PROPERTY(m10)
        FLX_REFL_REGISTER_PROPERTY(m11)
        FLX_REFL_REGISTER_PROPERTY(m12)
        FLX_REFL_REGISTER_PROPERTY(m20)
        FLX_REFL_REGISTER_PROPERTY(m21)
        FLX_REFL_REGISTER_PROPERTY(m22)
        FLX_REFL_REGISTER_END;

    #pragma endregion

    #pragma region Standard Functions

    const Matrix3x3 Matrix3x3::Zero = {
      0, 0, 0,
      0, 0, 0,
      0, 0, 0
    };

    const Matrix3x3 Matrix3x3::Identity = {
      1, 0, 0,
      0, 1, 0,
      0, 0, 1
    };

    std::string Matrix3x3::ToString() const
    {
        std::string str = "(";
        for (size_type i = 0; i < 9; ++i)
        {
            str += std::to_string(data[i]);
            if (i + 1 != 9)
            {
                str += ", ";
            }
            else
            {
                str += ")";
            }
        }
        return str;
    }
    std::string Matrix3x3::ToPrettyString() const
    {
        std::string str = "Matrix3x3\n";
        for (size_type i = 0; i < 3; ++i)
        {
            str += "(";
            for (size_type j = 0; j < 3; ++j)
            {
                str += std::to_string(data[i * 3 + j]);
                if (j + 1 != 3)
                {
                    str += ", ";
                }
            }
            if (i + 1 != 3)
            {
                str += ")\n";
            }
            else
            {
                str += ")";
            }
        }
        return str;
    }

    Matrix3x3::operator std::string() const { return ToString(); }

    #ifdef _DEBUG
    void Matrix3x3::Dump() const { Log::Debug(ToPrettyString()); }
    #endif

    #pragma endregion

    #pragma region Constructors

    Matrix3x3::Matrix3x3(
      value_type _m00, value_type _m01, value_type _m02,
      value_type _m10, value_type _m11, value_type _m12,
      value_type _m20, value_type _m21, value_type _m22
    )
    {
        m00 = _m00; m01 = _m01; m02 = _m02;
        m10 = _m10; m11 = _m11; m12 = _m12;
        m20 = _m20; m21 = _m21; m22 = _m22;
    }

    Matrix3x3::Matrix3x3(const Vector3& _m0, const Vector3& _m1, const Vector3& _m2)
    {
        m0 = _m0;
        m1 = _m1;
        m2 = _m2;
    }

    Matrix3x3::Matrix3x3(const Matrix3x3& other)
    {
        m00 = other.m00; m01 = other.m01; m02 = other.m02;
        m10 = other.m10; m11 = other.m11; m12 = other.m12;
        m20 = other.m20; m21 = other.m21; m22 = other.m22;
    }

    #pragma endregion

    #pragma region Operator Overloading

    // - =
    // += -= *= /=
    // == !=
    // + - * /

    Matrix3x3& Matrix3x3::operator=(const Matrix3x3& other)
    {
        m00 = other.m00; m01 = other.m01; m02 = other.m02;
        m10 = other.m10; m11 = other.m11; m12 = other.m12;
        m20 = other.m20; m21 = other.m21; m22 = other.m22;
        return *this;
    }

    Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& other)
    {
        return *this = *this + other;
    }

    Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& other)
    {
        return *this = *this - other;
    }

    Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& other)
    {
        return *this = *this * other;
    }

    Matrix3x3& Matrix3x3::operator*=(const_value_type& value)
    {
        return *this = *this * value;
    }

    Matrix3x3& Matrix3x3::operator/=(const_value_type value)
    {
        const_value_type inv = 1.0f / value;
        return *this = *this * inv;
    }

    bool Matrix3x3::operator==(const Matrix3x3& other) const
    {
        return
            (m00 == other.m00 && m01 == other.m01 && m02 == other.m02 &&
            m10 == other.m10 && m11 == other.m11 && m12 == other.m12 &&
            m20 == other.m20 && m21 == other.m21 && m22 == other.m22);
    }

    bool Matrix3x3::operator!=(const Matrix3x3& other) const
    {
        return !(*this == other);
    }

    Matrix3x3 Matrix3x3::Transpose() const
    {
        return {
          m00, m10, m20,
          m01, m11, m21,
          m02, m12, m22
        };
    }

    Matrix3x3::value_type Matrix3x3::Determinant() const
    {
        return
            (m00 * (m11 * m22 - m21 * m12)
            - m10 * (m01 * m22 - m21 * m02)
            + m20 * (m01 * m12 - m11 * m02));
    }

    // Refer to computer_inverse in glm/ext/func_matrix.inl
    // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/detail/func_matrix.inl#L388
    Matrix3x3 Matrix3x3::Inverse() const
    {
        Matrix3x3 inverse;
        inverse.m00 = (m11 * m22 - m21 * m12);
        inverse.m10 = -(m10 * m22 - m20 * m12);
        inverse.m20 = (m10 * m21 - m20 * m11);
        inverse.m01 = -(m01 * m22 - m21 * m02);
        inverse.m11 = (m00 * m22 - m20 * m02);
        inverse.m21 = -(m00 * m21 - m20 * m01);
        inverse.m02 = (m01 * m12 - m11 * m02);
        inverse.m12 = -(m00 * m12 - m10 * m02);
        inverse.m22 = (m00 * m11 - m10 * m01);

        value_type one_over_determinant = 1.0f / 
            (m00 * (m11 * m22 - m21 * m12)
            - m10 * (m01 * m22 - m21 * m02)
            + m20 * (m01 * m12 - m11 * m02));
        return inverse * one_over_determinant;
    }

    #pragma endregion

    #pragma region Passthrough Functions

    Matrix3x3::reference Matrix3x3::at(const Matrix3x3::size_type index) { return data[index]; }
    Matrix3x3::const_reference Matrix3x3::at(const Matrix3x3::size_type index) const { return data[index]; }
    Matrix3x3::reference Matrix3x3::operator[](const Matrix3x3::size_type index) { return at(index); }
    Matrix3x3::const_reference Matrix3x3::operator[](const Matrix3x3::size_type index) const { return at(index); }

    Matrix3x3::reference Matrix3x3::at(const Matrix3x3::size_type row, const Matrix3x3::size_type column) { return data[row * 3 + column]; }
    Matrix3x3::const_reference Matrix3x3::at(const Matrix3x3::size_type row, const Matrix3x3::size_type column) const { return data[row * 3 + column]; }
    Matrix3x3::reference Matrix3x3::operator()(const Matrix3x3::size_type row, const Matrix3x3::size_type column) { return at(row, column); }
    Matrix3x3::const_reference Matrix3x3::operator()(const Matrix3x3::size_type row, const Matrix3x3::size_type column) const { return at(row, column); }

    Matrix3x3::iterator                 Matrix3x3::begin() { return data; }
    Matrix3x3::const_iterator           Matrix3x3::begin() const { return data; }
    Matrix3x3::const_iterator           Matrix3x3::cbegin() const { return data; }
    Matrix3x3::iterator                 Matrix3x3::end() { return data + size(); }
    Matrix3x3::const_iterator           Matrix3x3::end() const { return data + size(); }
    Matrix3x3::const_iterator           Matrix3x3::cend() const { return data + size(); }

    constexpr Matrix3x3::size_type Matrix3x3::size() const { return 9; }

    #pragma endregion

    #pragma region Transformation Functions


    #pragma region Common

    Matrix3x3& Matrix3x3::Translate(const Vector3& translation)
    {
        Matrix3x3 result = *this;
        m2 += m0 * translation.x + m1 * translation.y;
        return *this;
    }

    // Refer to rotate in glm/ext/matrix_transform.inl
    // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_transform.inl#L18
    Matrix3x3& Matrix3x3::Rotate(const_value_type radians, const Vector3& rotation_axis)
    {
        const_value_type a = radians;
        const_value_type c = cos(a);
        const_value_type s = sin(a);

        Vector3 axis = Vector3::Normalize(rotation_axis);

        /*Matrix3x3 rotation;
        result[0][0] = c;
        result[0][1] = -s;
        result[0][2] = axis.x - (c * axis.x - s * axis.y);
        result[1][0] = s;
        result[1][1] = c;
        result[1][2] = axis.y - (s * axis.x + c * axis.y);
        result[2][0] = 0;
        result[2][1] = 0;
        result[2][2] = 1;*/

        Vector3 temp((1.0f - c) * axis);

        Matrix3x3 rotation;

        rotation(0, 0) = c + temp[0] * axis[0];
        rotation(0, 1) = temp[0] * axis[1] + s * axis[2];
        rotation(0, 2) = temp[0] * axis[2] - s * axis[1];

        rotation(1, 0) = temp[1] * axis[0] - s * axis[2];
        rotation(1, 1) = c + temp[1] * axis[1];
        rotation(1, 2) = temp[1] * axis[2] + s * axis[0];

        rotation(2, 0) = temp[2] * axis[0] + s * axis[1];
        rotation(2, 1) = temp[2] * axis[1] - s * axis[0];
        rotation(2, 2) = c + temp[2] * axis[2];

        Matrix3x3 result;

        result.m0 = m0 * rotation(0, 0) + m1 * rotation(0, 1) + m2 * rotation(0, 2);
        result.m1 = m0 * rotation(1, 0) + m1 * rotation(1, 1) + m2 * rotation(1, 2);
        result.m2 = m0 * rotation(2, 0) + m1 * rotation(2, 1) + m2 * rotation(2, 2);
        return *this = result;
    }

    Matrix3x3& Matrix3x3::RotateDeg(const_value_type degrees, const Vector3& rotation_axis)
    {
        return Rotate(radians(degrees), rotation_axis);
    }

    Matrix3x3& Matrix3x3::RotateX(const_value_type radians)
    {
        return Rotate(radians, Vector3::Right);
    }

    Matrix3x3& Matrix3x3::RotateY(const_value_type radians)
    {
        return Rotate(radians, Vector3::Up);
    }

    Matrix3x3& Matrix3x3::RotateZ(const_value_type radians)
    {
        return Rotate(radians, Vector3::Forward);
    }

    Matrix3x3& Matrix3x3::RotateXDeg(const_value_type degrees)
    {
        return RotateX(radians(degrees));
    }

    Matrix3x3& Matrix3x3::RotateYDeg(const_value_type degrees)
    {
        return RotateY(radians(degrees));
    }

    Matrix3x3& Matrix3x3::RotateZDeg(const_value_type degrees)
    {
        return RotateZ(radians(degrees));
    }

    Matrix3x3& Matrix3x3::Scale(const Vector3& scale)
    {
        m0 *= scale.x;
        m1 *= scale.y;
        return *this;
    }

    #pragma endregion

    #pragma region Static

    Matrix3x3 Matrix3x3::Translate(const Matrix3x3& matrix, const Vector3& translation)
    {
        Matrix3x3 result = matrix;
        result.Translate(translation);
        return result;
    }

    Matrix3x3 Matrix3x3::Rotate(const Matrix3x3& matrix, const_value_type radians, const Vector3& rotation_axis)
    {
        Matrix3x3 result = matrix;
        result.Rotate(radians, rotation_axis);
        return result;
    }

    Matrix3x3 Matrix3x3::RotateDeg(const Matrix3x3& matrix, const_value_type degrees, const Vector3& rotation_axis)
    {
        Matrix3x3 result = matrix;
        result.RotateDeg(degrees, rotation_axis);
        return result;
    }

    Matrix3x3 Matrix3x3::RotateX(const Matrix3x3& matrix, const_value_type radians)
    {
        Matrix3x3 result = matrix;
        result.RotateX(radians);
        return result;
    }

    Matrix3x3 Matrix3x3::RotateY(const Matrix3x3& matrix, const_value_type radians)
    {
        Matrix3x3 result = matrix;
        result.RotateY(radians);
        return result;
    }

    Matrix3x3 Matrix3x3::RotateZ(const Matrix3x3& matrix, const_value_type radians)
    {
        Matrix3x3 result = matrix;
        result.RotateZ(radians);
        return result;
    }

    Matrix3x3 Matrix3x3::RotateXDeg(const Matrix3x3& matrix, const Matrix3x3::value_type degrees)
    {
        Matrix3x3 result = matrix;
        result.RotateXDeg(degrees);
        return result;
    }

    Matrix3x3 Matrix3x3::RotateYDeg(const Matrix3x3& matrix, const Matrix3x3::value_type degrees)
    {
        Matrix3x3 result = matrix;
        result.RotateYDeg(degrees);
        return result;
    }

    Matrix3x3 Matrix3x3::RotateZDeg(const Matrix3x3& matrix, const Matrix3x3::value_type degrees)
    {
        Matrix3x3 result = matrix;
        result.RotateZDeg(degrees);
        return result;
    }

    Matrix3x3 Matrix3x3::Scale(const Matrix3x3& matrix, const Vector3& scale)
    {
        Matrix3x3 result = matrix;
        result.Scale(scale);
        return result;
    }

    #pragma endregion

    #pragma region Others

    // Uses the right-handed coordinate system
    // Refer to lookAtRH in glm/ext/matrix_transform.inl
    // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_transform.inl#L153
    Matrix3x3 Matrix3x3::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        const Vector3 f = Vector3::Normalize((center - eye));
        const Vector3 s = Vector3::Normalize(Cross(f, up));
        const Vector3 u = Cross(s, f);

        Matrix3x3 result = Matrix3x3::Identity;

        result.m00 = s.x;
        result.m10 = s.y;
        result.m20 = s.z;
        result.m01 = u.x;
        result.m11 = u.y;
        result.m21 = u.z;
        result.m02 = -f.x;
        result.m12 = -f.y;
        result.m22 = -f.z;

        return result;
    }

    // Uses the right-handed coordinate system
    // -1 to 1 depth clipping
    // Refer to perspectiveRH_NO in glm/ext/matrix_clip_space.inl
    // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_clip_space.inl#L249
    Matrix3x3 Matrix3x3::Perspective(
      const_value_type fov, const_value_type aspect_ratio,
      const_value_type near, const_value_type far
    )
    {
        const_value_type f = tan(fov / 2);
        Matrix3x3 result = Matrix3x3::Zero;

        #ifdef DEPTH_CLIP_ZERO_TO_ONE
        result.m00 = 1 / (aspect_ratio * f);
        result.m11 = 1 / f;
        result.m22 = far / (near - far);
        #else
        result.m00 = 1 / (aspect_ratio * f);
        result.m11 = 1 / f;
        result.m22 = -(far + near) / (far - near);
        #endif

        return result;
    }

    // Uses the right-handed coordinate system
    // -1 to 1 depth clipping
    // Refer to orthoRH_NO in glm/ext/matrix_clip_space.inl
    // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_clip_space.inl#L55
    Matrix3x3 Matrix3x3::Orthographic(
      const_value_type left, const_value_type right, const_value_type bottom, const_value_type top,
      const_value_type near, const_value_type far
    )
    {
        Matrix3x3 result = Matrix3x3::Identity;

        #ifdef DEPTH_CLIP_ZERO_TO_ONE
        result.m00 = 2 / (right - left);
        result.m11 = 2 / (top - bottom);
        result.m22 = 1 / (far - near);
        #else
        result.m00 = 2 / (right - left);
        result.m11 = 2 / (top - bottom);
        result.m22 = -2 / (far - near);
        #endif

        return result;
    }

    #pragma endregion


    #pragma endregion

    #pragma region Matrix3x3 Helper Fns

    Matrix3x3 operator+(const Matrix3x3& matrix_a, const Matrix3x3& matrix_b)
    {
        return {
          matrix_a.m00 + matrix_b.m00, matrix_a.m01 + matrix_b.m01, matrix_a.m02 + matrix_b.m02,
          matrix_a.m10 + matrix_b.m10, matrix_a.m11 + matrix_b.m11, matrix_a.m12 + matrix_b.m12,
          matrix_a.m20 + matrix_b.m20, matrix_a.m21 + matrix_b.m21, matrix_a.m22 + matrix_b.m22
        };
    }


    Matrix3x3 operator-(const Matrix3x3& matrix_a, const Matrix3x3& matrix_b)
    {
        return {
          matrix_a.m00 - matrix_b.m00, matrix_a.m01 - matrix_b.m01, matrix_a.m02 - matrix_b.m02,
          matrix_a.m10 - matrix_b.m10, matrix_a.m11 - matrix_b.m11, matrix_a.m12 - matrix_b.m12,
          matrix_a.m20 - matrix_b.m20, matrix_a.m21 - matrix_b.m21, matrix_a.m22 - matrix_b.m22
        };
    }

    Matrix3x3 operator*(const Matrix3x3& matrix_a, const Matrix3x3& matrix_b)
    {
        Matrix3x3 result;

        result.m00 = matrix_a.m00 * matrix_b.m00 + matrix_a.m10 * matrix_b.m01 + matrix_a.m20 * matrix_b.m02;
        result.m01 = matrix_a.m01 * matrix_b.m00 + matrix_a.m11 * matrix_b.m01 + matrix_a.m21 * matrix_b.m02;
        result.m02 = matrix_a.m02 * matrix_b.m00 + matrix_a.m12 * matrix_b.m01 + matrix_a.m22 * matrix_b.m02;

        result.m10 = matrix_a.m00 * matrix_b.m10 + matrix_a.m10 * matrix_b.m11 + matrix_a.m20 * matrix_b.m12;
        result.m11 = matrix_a.m01 * matrix_b.m10 + matrix_a.m11 * matrix_b.m11 + matrix_a.m21 * matrix_b.m12;
        result.m12 = matrix_a.m02 * matrix_b.m10 + matrix_a.m12 * matrix_b.m11 + matrix_a.m22 * matrix_b.m12;

        result.m20 = matrix_a.m00 * matrix_b.m20 + matrix_a.m10 * matrix_b.m21 + matrix_a.m20 * matrix_b.m22;
        result.m21 = matrix_a.m01 * matrix_b.m20 + matrix_a.m11 * matrix_b.m21 + matrix_a.m21 * matrix_b.m22;
        result.m22 = matrix_a.m02 * matrix_b.m20 + matrix_a.m12 * matrix_b.m21 + matrix_a.m22 * matrix_b.m22;

        return result;
    }

    Matrix3x3 operator*(Matrix3x3::const_value_type value, const Matrix3x3& matrix)
    {
        return {
          matrix.m00 * value, matrix.m01 * value, matrix.m02 * value,
          matrix.m10 * value, matrix.m11 * value, matrix.m12 * value,
          matrix.m20 * value, matrix.m21 * value, matrix.m22 * value
        };
    }

    Matrix3x3 operator*(const Matrix3x3& matrix, Matrix3x3::const_value_type value)
    {
        return value * matrix;
    }

    Matrix3x3 operator/(const Matrix3x3& matrix, Matrix3x3::const_value_type value)
    {
        if (value == 0) return Matrix3x3::Zero;
        Matrix3x3::const_value_type inv = 1.0f / value;
        return {
          matrix.m00 * inv, matrix.m01 * inv, matrix.m02 * inv,
          matrix.m10 * inv, matrix.m11 * inv, matrix.m12 * inv,
          matrix.m20 * inv, matrix.m21 * inv, matrix.m22 * inv
        };
    }

    std::istream& operator>>(std::istream& is, Matrix3x3& matrix)
    {
        for (Matrix3x3::size_type i = 0; i < matrix.size(); ++i)
        {
            is >> matrix.data[i];
        }
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const Matrix3x3& matrix)
    {
        for (Matrix3x3::size_type i = 0; i < matrix.size(); ++i)
        {
            os << matrix.data[i];
            if (i + 1 != matrix.size())
            {
                os << " ";
            }
        }
        return os;
    }

    #pragma endregion

}