// WLVERSE [https://wlverse.web.app]
// matrix4x4.cpp
// 
// A 4x4 matrix.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "matrix4x4.h"

namespace FlexEngine
{

  #pragma region Reflection

  FLX_REFL_REGISTER_START(Matrix4x4)
    FLX_REFL_REGISTER_PROPERTY(m00)
    FLX_REFL_REGISTER_PROPERTY(m01)
    FLX_REFL_REGISTER_PROPERTY(m02)
    FLX_REFL_REGISTER_PROPERTY(m03)
    FLX_REFL_REGISTER_PROPERTY(m10)
    FLX_REFL_REGISTER_PROPERTY(m11)
    FLX_REFL_REGISTER_PROPERTY(m12)
    FLX_REFL_REGISTER_PROPERTY(m13)
    FLX_REFL_REGISTER_PROPERTY(m20)
    FLX_REFL_REGISTER_PROPERTY(m21)
    FLX_REFL_REGISTER_PROPERTY(m22)
    FLX_REFL_REGISTER_PROPERTY(m23)
    FLX_REFL_REGISTER_PROPERTY(m30)
    FLX_REFL_REGISTER_PROPERTY(m31)
    FLX_REFL_REGISTER_PROPERTY(m32)
    FLX_REFL_REGISTER_PROPERTY(m33)
  FLX_REFL_REGISTER_END;

  #pragma endregion

  #pragma region Standard Functions

  const Matrix4x4 Matrix4x4::Zero = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
  };

  const Matrix4x4 Matrix4x4::Identity = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
  };

  std::string Matrix4x4::ToString() const
  {
    std::string str = "(";
    for (size_type i = 0; i < 16; ++i)
    {
      str += std::to_string(data[i]);
      str += ", ";
    }
    str.pop_back();
    str.pop_back();
    str += ")";
    return str;
  }
  std::string Matrix4x4::ToPrettyString() const
  {
    std::string str = "Matrix4x4\n";
    for (size_type i = 0; i < 4; ++i)
    {
      str += "(";
      for (size_type j = 0; j < 4; ++j)
      {
        str += std::to_string(data[i * 4 + j]);
        str += ", ";
      }
      str.pop_back();
      str.pop_back();
      str += ")\n";
    }
    return str;
  }
  Matrix4x4::operator std::string() const { return ToString(); }

#ifdef _DEBUG
  void Matrix4x4::Dump() const { Log::Debug(ToPrettyString()); }
#endif

  #pragma endregion

  #pragma region Constructors

  Matrix4x4::Matrix4x4(
    value_type _m00, value_type _m01, value_type _m02, value_type _m03,
    value_type _m10, value_type _m11, value_type _m12, value_type _m13,
    value_type _m20, value_type _m21, value_type _m22, value_type _m23,
    value_type _m30, value_type _m31, value_type _m32, value_type _m33
  )
  {
    m00 = _m00; m01 = _m01; m02 = _m02; m03 = _m03;
    m10 = _m10; m11 = _m11; m12 = _m12; m13 = _m13;
    m20 = _m20; m21 = _m21; m22 = _m22; m23 = _m23;
    m30 = _m30; m31 = _m31; m32 = _m32; m33 = _m33;
  }

  Matrix4x4::Matrix4x4(const Vector4& _m0, const Vector4& _m1, const Vector4& _m2, const Vector4& _m3)
  {
    m0 = _m0;
    m1 = _m1;
    m2 = _m2;
    m3 = _m3;
  }

  Matrix4x4::Matrix4x4(const Matrix4x4& other)
  {
    m00 = other.m00; m01 = other.m01; m02 = other.m02; m03 = other.m03;
    m10 = other.m10; m11 = other.m11; m12 = other.m12; m13 = other.m13;
    m20 = other.m20; m21 = other.m21; m22 = other.m22; m23 = other.m23;
    m30 = other.m30; m31 = other.m31; m32 = other.m32; m33 = other.m33;
  }

  #pragma endregion

  #pragma region Operator Overloading

  // - =
  // += -= *= /=
  // == !=
  // + - * /

  //Matrix4x4 Matrix4x4::operator-() const;

  Matrix4x4& Matrix4x4::operator=(const Matrix4x4& other)
  {
    m00 = other.m00; m01 = other.m01; m02 = other.m02; m03 = other.m03;
    m10 = other.m10; m11 = other.m11; m12 = other.m12; m13 = other.m13;
    m20 = other.m20; m21 = other.m21; m22 = other.m22; m23 = other.m23;
    m30 = other.m30; m31 = other.m31; m32 = other.m32; m33 = other.m33;
    return *this;
  }

  Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other)
  {
    return *this = *this + other;
  }

  //Matrix4x4& Matrix4x4::operator+=(const_value_type value);

  Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other)
  {
    return *this = *this - other;
  }

  //Matrix4x4& Matrix4x4::operator-=(const_value_type value);

  Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other)
  {
    return *this = *this * other;
  }

  Matrix4x4& Matrix4x4::operator*=(const_value_type& value)
  {
    return *this = *this * value;
  }

  //Matrix4x4& Matrix4x4::operator/=(const Matrix4x4& other);

  Matrix4x4& Matrix4x4::operator/=(const_value_type value)
  {
    const_value_type inv = 1.0f / value;
    return *this = *this * inv;
  }

  bool Matrix4x4::operator==(const Matrix4x4& other) const
  {
    return
      m00 == other.m00 && m01 == other.m01 && m02 == other.m02 && m03 == other.m03 &&
      m10 == other.m10 && m11 == other.m11 && m12 == other.m12 && m13 == other.m13 &&
      m20 == other.m20 && m21 == other.m21 && m22 == other.m22 && m23 == other.m23 &&
      m30 == other.m30 && m31 == other.m31 && m32 == other.m32 && m33 == other.m33
    ;
  }

  bool Matrix4x4::operator!=(const Matrix4x4& other) const
  {
    return !(*this == other);
  }

  Matrix4x4 Matrix4x4::Transpose() const
  {
    return {
      m00, m10, m20, m30,
      m01, m11, m21, m31,
      m02, m12, m22, m32,
      m03, m13, m23, m33
    };
  }

  Matrix4x4::value_type Matrix4x4::Determinant() const
  {
    return
      m00 * m11 * m22 * m33 + m00 * m12 * m23 * m31 + m00 * m13 * m21 * m32 +
      m01 * m10 * m23 * m32 + m01 * m12 * m20 * m33 + m01 * m13 * m22 * m30 +
      m02 * m10 * m21 * m33 + m02 * m11 * m23 * m30 + m02 * m13 * m20 * m31 +
      m03 * m10 * m22 * m31 + m03 * m11 * m20 * m32 + m03 * m12 * m21 * m30 -
      m00 * m11 * m23 * m32 - m00 * m12 * m21 * m33 - m00 * m13 * m22 * m31 -
      m01 * m10 * m22 * m33 - m01 * m12 * m23 * m30 - m01 * m13 * m20 * m32 -
      m02 * m10 * m23 * m31 - m02 * m11 * m20 * m33 - m02 * m13 * m21 * m30 -
      m03 * m10 * m21 * m32 - m03 * m11 * m22 * m30 - m03 * m12 * m20 * m31
    ;
  }

  // Refer to computer_inverse in glm/ext/func_matrix.inl
  // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/detail/func_matrix.inl#L388
  Matrix4x4 Matrix4x4::Inverse() const
  {
    value_type coef_00 = m22 * m33 - m32 * m23;
    value_type coef_02 = m12 * m33 - m32 * m13;
    value_type coef_03 = m12 * m23 - m22 * m13;
    
    value_type coef_04 = m21 * m33 - m31 * m23;
    value_type coef_06 = m11 * m33 - m31 * m13;
    value_type coef_07 = m11 * m23 - m21 * m13;
    
    value_type coef_08 = m21 * m32 - m31 * m22;
    value_type coef_10 = m11 * m32 - m31 * m12;
    value_type coef_11 = m11 * m22 - m21 * m12;
    
    value_type coef_12 = m20 * m33 - m30 * m23;
    value_type coef_14 = m10 * m33 - m30 * m13;
    value_type coef_15 = m10 * m23 - m20 * m13;
    
    value_type coef_16 = m20 * m32 - m30 * m22;
    value_type coef_18 = m10 * m32 - m30 * m12;
    value_type coef_19 = m10 * m22 - m20 * m12;
    
    value_type coef_20 = m20 * m31 - m30 * m21;
    value_type coef_22 = m10 * m31 - m30 * m11;
    value_type coef_23 = m10 * m21 - m20 * m11;
    
    Vector4 fac_0(coef_00, coef_00, coef_02, coef_03);
    Vector4 fac_1(coef_04, coef_04, coef_06, coef_07);
    Vector4 fac_2(coef_08, coef_08, coef_10, coef_11);
    Vector4 fac_3(coef_12, coef_12, coef_14, coef_15);
    Vector4 fac_4(coef_16, coef_16, coef_18, coef_19);
    Vector4 fac_5(coef_20, coef_20, coef_22, coef_23);
    
    Vector4 vec_0(m10, m00, m00, m00);
    Vector4 vec_1(m11, m01, m01, m01);
    Vector4 vec_2(m12, m02, m02, m02);
    Vector4 vec_3(m13, m03, m03, m03);
    
    Vector4 inv_0(vec_1 * fac_0 - vec_2 * fac_1 + vec_3 * fac_2);
    Vector4 inv_1(vec_0 * fac_0 - vec_2 * fac_3 + vec_3 * fac_4);
    Vector4 inv_2(vec_0 * fac_1 - vec_1 * fac_3 + vec_3 * fac_5);
    Vector4 inv_3(vec_0 * fac_2 - vec_1 * fac_4 + vec_2 * fac_5);
    
    Vector4 sign_A(+1, -1, +1, -1);
    Vector4 sign_B(-1, +1, -1, +1);
    Matrix4x4 inverse(inv_0 * sign_A, inv_1 * sign_B, inv_2 * sign_A, inv_3 * sign_B);
    
    Vector4 row_0(inverse(0, 0), inverse(1, 0), inverse(2, 0), inverse(3, 0));
    
    Vector4 dot_0(m0 * row_0);
    value_type dot_1 = (dot_0.x + dot_0.y) + (dot_0.z + dot_0.w);
    
    value_type one_over_determinant = 1.0f / dot_1;
    
    return inverse * one_over_determinant;
  }

  #pragma endregion

  #pragma region Passthrough Functions

  Matrix4x4::reference Matrix4x4::at(const Matrix4x4::size_type index) { return data[index]; }
  Matrix4x4::const_reference Matrix4x4::at(const Matrix4x4::size_type index) const { return data[index]; }
  Matrix4x4::reference Matrix4x4::operator[](const Matrix4x4::size_type index) { return at(index); }
  Matrix4x4::const_reference Matrix4x4::operator[](const Matrix4x4::size_type index) const { return at(index); }

  Matrix4x4::reference Matrix4x4::at(const Matrix4x4::size_type row, const Matrix4x4::size_type column) { return data[row * 4 + column]; }
  Matrix4x4::const_reference Matrix4x4::at(const Matrix4x4::size_type row, const Matrix4x4::size_type column) const { return data[row * 4 + column]; }
  Matrix4x4::reference Matrix4x4::operator()(const Matrix4x4::size_type row, const Matrix4x4::size_type column) { return at(row, column); }
  Matrix4x4::const_reference Matrix4x4::operator()(const Matrix4x4::size_type row, const Matrix4x4::size_type column) const { return at(row, column); }

  Matrix4x4::iterator                 Matrix4x4::begin() { return data; }
  Matrix4x4::const_iterator           Matrix4x4::begin() const { return data; }
  Matrix4x4::const_iterator           Matrix4x4::cbegin() const { return data; }
  Matrix4x4::iterator                 Matrix4x4::end() { return data + size(); }
  Matrix4x4::const_iterator           Matrix4x4::end() const { return data + size(); }
  Matrix4x4::const_iterator           Matrix4x4::cend() const { return data + size(); }
  //Matrix4x4::reverse_iterator         Matrix4x4::rbegin()         { return data + size() - 1; }
  //Matrix4x4::const_reverse_iterator   Matrix4x4::rbegin() const   { return data + size() - 1; }
  //Matrix4x4::const_reverse_iterator   Matrix4x4::crbegin() const  { return data + size() - 1; }
  //Matrix4x4::reverse_iterator         Matrix4x4::rend()           { return data - 1; }
  //Matrix4x4::const_reverse_iterator   Matrix4x4::rend() const     { return data - 1; }
  //Matrix4x4::const_reverse_iterator   Matrix4x4::crend() const    { return data - 1; }

  constexpr Matrix4x4::size_type Matrix4x4::size() const { return 16; }

  #pragma endregion

  #pragma region Transformation Functions


  #pragma region Common

  Matrix4x4& Matrix4x4::Translate(const Vector3& translation)
  {
    Matrix4x4 result = *this;
    m3 += m0 * translation.x + m1 * translation.y + m2 * translation.z;
    return *this;
  }

  // Refer to rotate in glm/ext/matrix_transform.inl
  // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_transform.inl#L18
  Matrix4x4& Matrix4x4::Rotate(const_value_type radians, const Vector3& rotation_axis)
  {
    const_value_type a = radians;
    const_value_type c = cos(a);
    const_value_type s = sin(a);

    Vector3 axis = Vector3::Normalize(rotation_axis);
    Vector3 temp((1.0f - c) * axis);

    Matrix4x4 rotation;

    rotation(0, 0) = c + temp[0] * axis[0];
    rotation(0, 1) = temp[0] * axis[1] + s * axis[2];
    rotation(0, 2) = temp[0] * axis[2] - s * axis[1];

    rotation(1, 0) = temp[1] * axis[0] - s * axis[2];
    rotation(1, 1) = c + temp[1] * axis[1];
    rotation(1, 2) = temp[1] * axis[2] + s * axis[0];

    rotation(2, 0) = temp[2] * axis[0] + s * axis[1];
    rotation(2, 1) = temp[2] * axis[1] - s * axis[0];
    rotation(2, 2) = c + temp[2] * axis[2];

    Matrix4x4 result;

    result.m0 = m0 * rotation(0, 0) + m1 * rotation(0, 1) + m2 * rotation(0, 2);
    result.m1 = m0 * rotation(1, 0) + m1 * rotation(1, 1) + m2 * rotation(1, 2);
    result.m2 = m0 * rotation(2, 0) + m1 * rotation(2, 1) + m2 * rotation(2, 2);
    result.m3 = m3;

    return *this = result;
  }

  Matrix4x4& Matrix4x4::RotateDeg(const_value_type degrees, const Vector3& rotation_axis)
  {
    return Rotate(radians(degrees), rotation_axis);
  }

  Matrix4x4& Matrix4x4::RotateX(const_value_type radians)
  {
    return Rotate(radians, Vector3::Right);
  }

  Matrix4x4& Matrix4x4::RotateY(const_value_type radians)
  {
    return Rotate(radians, Vector3::Up);
  }

  Matrix4x4& Matrix4x4::RotateZ(const_value_type radians)
  {
    return Rotate(radians, Vector3::Forward);
  }

  Matrix4x4& Matrix4x4::RotateXDeg(const_value_type degrees)
  {
    return RotateX(radians(degrees));
  }

  Matrix4x4& Matrix4x4::RotateYDeg(const_value_type degrees)
  {
    return RotateY(radians(degrees));
  }

  Matrix4x4& Matrix4x4::RotateZDeg(const_value_type degrees)
  {
    return RotateZ(radians(degrees));
  }

  Matrix4x4& Matrix4x4::Scale(const Vector3& scale)
  {
    m0 *= scale.x;
    m1 *= scale.y;
    m2 *= scale.z;
    return *this;
  }

  #pragma endregion

  #pragma region Static

  Matrix4x4 Matrix4x4::Translate(const Matrix4x4& matrix, const Vector3& translation)
  {
    Matrix4x4 result = matrix;
    result.Translate(translation);
    return result;
  }

  Matrix4x4 Matrix4x4::Rotate(const Matrix4x4& matrix, const_value_type radians, const Vector3& rotation_axis)
  {
    Matrix4x4 result = matrix;
    result.Rotate(radians, rotation_axis);
    return result;
  }

  Matrix4x4 Matrix4x4::RotateDeg(const Matrix4x4& matrix, const_value_type degrees, const Vector3& rotation_axis)
  {
    Matrix4x4 result = matrix;
    result.RotateDeg(degrees, rotation_axis);
    return result;
  }

  Matrix4x4 Matrix4x4::RotateX(const Matrix4x4& matrix, const_value_type radians)
  {
    Matrix4x4 result = matrix;
    result.RotateX(radians);
    return result;
  }

  Matrix4x4 Matrix4x4::RotateY(const Matrix4x4& matrix, const_value_type radians)
  {
    Matrix4x4 result = matrix;
    result.RotateY(radians);
    return result;
  }

  Matrix4x4 Matrix4x4::RotateZ(const Matrix4x4& matrix, const_value_type radians)
  {
    Matrix4x4 result = matrix;
    result.RotateZ(radians);
    return result;
  }

  Matrix4x4 Matrix4x4::RotateXDeg(const Matrix4x4& matrix, const Matrix4x4::value_type degrees)
  {
    Matrix4x4 result = matrix;
    result.RotateXDeg(degrees);
    return result;
  }

  Matrix4x4 Matrix4x4::RotateYDeg(const Matrix4x4& matrix, const Matrix4x4::value_type degrees)
  {
    Matrix4x4 result = matrix;
    result.RotateYDeg(degrees);
    return result;
  }

  Matrix4x4 Matrix4x4::RotateZDeg(const Matrix4x4& matrix, const Matrix4x4::value_type degrees)
  {
    Matrix4x4 result = matrix;
    result.RotateZDeg(degrees);
    return result;
  }

  Matrix4x4 Matrix4x4::Scale(const Matrix4x4& matrix, const Vector3& scale)
  {
    Matrix4x4 result = matrix;
    result.Scale(scale);
    return result;
  }

  #pragma endregion

  #pragma region Others

  // Uses the right-handed coordinate system
  // Refer to lookAtRH in glm/ext/matrix_transform.inl
  // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_transform.inl#L153
  Matrix4x4 Matrix4x4::LookAt(const Vector3& eye, const Vector3& center, const Vector3& up)
  {
    const Vector3 f = Vector3::Normalize((center - eye));
    const Vector3 s = Vector3::Normalize(Cross(f, up));
    const Vector3 u = Cross(s, f);

    Matrix4x4 result = Matrix4x4::Identity;

    result.m00 =  s.x;
    result.m10 =  s.y;
    result.m20 =  s.z;
    result.m01 =  u.x;
    result.m11 =  u.y;
    result.m21 =  u.z;
    result.m02 = -f.x;
    result.m12 = -f.y;
    result.m22 = -f.z;
    result.m30 = -Dot(s, eye);
    result.m31 = -Dot(u, eye);
    result.m32 =  Dot(f, eye);

    return result;
  }

  // Uses the right-handed coordinate system
  // -1 to 1 depth clipping
  // Refer to perspectiveRH_NO in glm/ext/matrix_clip_space.inl
  // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_clip_space.inl#L249
  Matrix4x4 Matrix4x4::Perspective(
    const_value_type fov, const_value_type aspect_ratio,
    const_value_type near, const_value_type far
  )
  {
    const_value_type f = tan(fov / 2);
    Matrix4x4 result = Matrix4x4::Zero;

#ifdef DEPTH_CLIP_ZERO_TO_ONE
    result.m00 = 1 / (aspect_ratio * f);
    result.m11 = 1 / f;
    result.m22 = far / (near - far);
    result.m23 = -1;
    result.m32 = -(far * near) / (far - near);
#else
    result.m00 = 1 / (aspect_ratio * f);
    result.m11 = 1 / f;
    result.m22 = - (far + near) / (far - near);
    result.m23 = -1;
    result.m32 = - (2 * far * near) / (far - near);
#endif

    return result;
  }

  // Uses the right-handed coordinate system
  // -1 to 1 depth clipping
  // Refer to orthoRH_NO in glm/ext/matrix_clip_space.inl
  // https://github.com/g-truc/glm/blob/45008b225e28eb700fa0f7d3ff69b7c1db94fadf/glm/ext/matrix_clip_space.inl#L55
  Matrix4x4 Matrix4x4::Orthographic(
    const_value_type left, const_value_type right, const_value_type bottom, const_value_type top,
    const_value_type near, const_value_type far
  )
  {
    Matrix4x4 result = Matrix4x4::Identity;

#ifdef DEPTH_CLIP_ZERO_TO_ONE
    result.m00 = 2 / (right - left);
    result.m11 = 2 / (top - bottom);
    result.m22 = 1 / (far - near);
    result.m30 = -(right + left) / (right - left);
    result.m31 = -(top + bottom) / (top - bottom);
    result.m32 = -near / (far - near);
#else
    result.m00 = 2 / (right - left);
    result.m11 = 2 / (top - bottom);
    result.m22 = - 2 / (far - near);
    result.m30 = -(right + left) / (right - left);
    result.m31 = -(top + bottom) / (top - bottom);
    result.m32 = -(far + near) / (far - near);
#endif

    return result;
  }

  #pragma endregion


  #pragma endregion

  #pragma region Matrix4x4 Helper Fns

  Matrix4x4 operator+(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b)
  {
    return {
      matrix_a.m00 + matrix_b.m00, matrix_a.m01 + matrix_b.m01, matrix_a.m02 + matrix_b.m02, matrix_a.m03 + matrix_b.m03,
      matrix_a.m10 + matrix_b.m10, matrix_a.m11 + matrix_b.m11, matrix_a.m12 + matrix_b.m12, matrix_a.m13 + matrix_b.m13,
      matrix_a.m20 + matrix_b.m20, matrix_a.m21 + matrix_b.m21, matrix_a.m22 + matrix_b.m22, matrix_a.m23 + matrix_b.m23,
      matrix_a.m30 + matrix_b.m30, matrix_a.m31 + matrix_b.m31, matrix_a.m32 + matrix_b.m32, matrix_a.m33 + matrix_b.m33
    };
  }

  //Matrix4x4 operator+(Matrix4x4::const_value_type value, const Matrix4x4& matrix);
  
  //Matrix4x4 operator+(const Matrix4x4& matrix, Matrix4x4::const_value_type value);

  Matrix4x4 operator-(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b)
  {
    return {
      matrix_a.m00 - matrix_b.m00, matrix_a.m01 - matrix_b.m01, matrix_a.m02 - matrix_b.m02, matrix_a.m03 - matrix_b.m03,
      matrix_a.m10 - matrix_b.m10, matrix_a.m11 - matrix_b.m11, matrix_a.m12 - matrix_b.m12, matrix_a.m13 - matrix_b.m13,
      matrix_a.m20 - matrix_b.m20, matrix_a.m21 - matrix_b.m21, matrix_a.m22 - matrix_b.m22, matrix_a.m23 - matrix_b.m23,
      matrix_a.m30 - matrix_b.m30, matrix_a.m31 - matrix_b.m31, matrix_a.m32 - matrix_b.m32, matrix_a.m33 - matrix_b.m33
    };
  }

  //Matrix4x4 operator-(Matrix4x4::const_value_type value, const Matrix4x4& matrix);
  
  //Matrix4x4 operator-(const Matrix4x4& matrix, Matrix4x4::const_value_type value);

  Matrix4x4 operator*(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b)
  {
    Matrix4x4 result;

    result.m00 = matrix_a.m00 * matrix_b.m00 + matrix_a.m10 * matrix_b.m01 + matrix_a.m20 * matrix_b.m02 + matrix_a.m30 * matrix_b.m03;
    result.m01 = matrix_a.m01 * matrix_b.m00 + matrix_a.m11 * matrix_b.m01 + matrix_a.m21 * matrix_b.m02 + matrix_a.m31 * matrix_b.m03;
    result.m02 = matrix_a.m02 * matrix_b.m00 + matrix_a.m12 * matrix_b.m01 + matrix_a.m22 * matrix_b.m02 + matrix_a.m32 * matrix_b.m03;
    result.m03 = matrix_a.m03 * matrix_b.m00 + matrix_a.m13 * matrix_b.m01 + matrix_a.m23 * matrix_b.m02 + matrix_a.m33 * matrix_b.m03;

    result.m10 = matrix_a.m00 * matrix_b.m10 + matrix_a.m10 * matrix_b.m11 + matrix_a.m20 * matrix_b.m12 + matrix_a.m30 * matrix_b.m13;
    result.m11 = matrix_a.m01 * matrix_b.m10 + matrix_a.m11 * matrix_b.m11 + matrix_a.m21 * matrix_b.m12 + matrix_a.m31 * matrix_b.m13;
    result.m12 = matrix_a.m02 * matrix_b.m10 + matrix_a.m12 * matrix_b.m11 + matrix_a.m22 * matrix_b.m12 + matrix_a.m32 * matrix_b.m13;
    result.m13 = matrix_a.m03 * matrix_b.m10 + matrix_a.m13 * matrix_b.m11 + matrix_a.m23 * matrix_b.m12 + matrix_a.m33 * matrix_b.m13;

    result.m20 = matrix_a.m00 * matrix_b.m20 + matrix_a.m10 * matrix_b.m21 + matrix_a.m20 * matrix_b.m22 + matrix_a.m30 * matrix_b.m23;
    result.m21 = matrix_a.m01 * matrix_b.m20 + matrix_a.m11 * matrix_b.m21 + matrix_a.m21 * matrix_b.m22 + matrix_a.m31 * matrix_b.m23;
    result.m22 = matrix_a.m02 * matrix_b.m20 + matrix_a.m12 * matrix_b.m21 + matrix_a.m22 * matrix_b.m22 + matrix_a.m32 * matrix_b.m23;
    result.m23 = matrix_a.m03 * matrix_b.m20 + matrix_a.m13 * matrix_b.m21 + matrix_a.m23 * matrix_b.m22 + matrix_a.m33 * matrix_b.m23;

    result.m30 = matrix_a.m00 * matrix_b.m30 + matrix_a.m10 * matrix_b.m31 + matrix_a.m20 * matrix_b.m32 + matrix_a.m30 * matrix_b.m33;
    result.m31 = matrix_a.m01 * matrix_b.m30 + matrix_a.m11 * matrix_b.m31 + matrix_a.m21 * matrix_b.m32 + matrix_a.m31 * matrix_b.m33;
    result.m32 = matrix_a.m02 * matrix_b.m30 + matrix_a.m12 * matrix_b.m31 + matrix_a.m22 * matrix_b.m32 + matrix_a.m32 * matrix_b.m33;
    result.m33 = matrix_a.m03 * matrix_b.m30 + matrix_a.m13 * matrix_b.m31 + matrix_a.m23 * matrix_b.m32 + matrix_a.m33 * matrix_b.m33;

    return result;
  }

  Matrix4x4 operator*(Matrix4x4::const_value_type value, const Matrix4x4& matrix)
  {
    return {
      matrix.m00 * value, matrix.m01 * value, matrix.m02 * value, matrix.m03 * value,
      matrix.m10 * value, matrix.m11 * value, matrix.m12 * value, matrix.m13 * value,
      matrix.m20 * value, matrix.m21 * value, matrix.m22 * value, matrix.m23 * value,
      matrix.m30 * value, matrix.m31 * value, matrix.m32 * value, matrix.m33 * value
    };
  }

  Matrix4x4 operator*(const Matrix4x4& matrix, Matrix4x4::const_value_type value)
  {
    return value * matrix;
  }

  Vector4 operator*(const Matrix4x4& matrix, const Vector4& vector)
  {
    return Vector4(
      matrix.m00 * vector.x + matrix.m10 * vector.y + matrix.m20 * vector.z + matrix.m30 * vector.w,
      matrix.m01 * vector.x + matrix.m11 * vector.y + matrix.m21 * vector.z + matrix.m31 * vector.w,
      matrix.m02 * vector.x + matrix.m12 * vector.y + matrix.m22 * vector.z + matrix.m32 * vector.w,
      matrix.m03 * vector.x + matrix.m13 * vector.y + matrix.m23 * vector.z + matrix.m33 * vector.w
    );
  }

  //Matrix4x4 operator/(const Matrix4x4& matrix_a, const Matrix4x4& matrix_b);
  
  //Matrix4x4 operator/(Matrix4x4::const_value_type value, const Matrix4x4& matrix);

  Matrix4x4 operator/(const Matrix4x4& matrix, Matrix4x4::const_value_type value)
  {
    if (value == 0) return Matrix4x4::Zero;
    Matrix4x4::const_value_type inv = 1.0f / value;
    return {
      matrix.m00 * inv, matrix.m01 * inv, matrix.m02 * inv, matrix.m03 * inv,
      matrix.m10 * inv, matrix.m11 * inv, matrix.m12 * inv, matrix.m13 * inv,
      matrix.m20 * inv, matrix.m21 * inv, matrix.m22 * inv, matrix.m23 * inv,
      matrix.m30 * inv, matrix.m31 * inv, matrix.m32 * inv, matrix.m33 * inv
    };
  }

  std::istream& operator>>(std::istream& is, Matrix4x4& matrix)
  {
    for (Matrix4x4::size_type i = 0; i < matrix.size(); ++i)
    {
      is >> matrix.data[i];
    }
    return is;
  }

  std::ostream& operator<<(std::ostream& os, const Matrix4x4& matrix)
  {
    for (Matrix4x4::size_type i = 0; i < matrix.size(); ++i)
    {
      os << matrix.data[i];
      if (i < matrix.size() - 1) os << " ";
    }
    return os;
  }

  #pragma endregion

}