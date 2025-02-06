// WLVERSE [https://wlverse.web.app]
// UnitTests.cpp
//
// Extremely trivial and basic unit tests for testing library functionality like
// FlexMath and FlexID. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include <FlexEngine.h>
using namespace FlexEngine;

#pragma warning(disable: 4189) // local variable is initialized but not referenced


#pragma region Helper Functions

#pragma warning(push)
#pragma warning(disable: 4505) // unreferenced function with internal linkage has been removed


// Special functions for comparing Vector# and glm::vec#
#pragma region AreEqualVector

static void AreEqualVector(const Vector2& expected, const Vector2& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
}
static void AreEqualVector(const Vector3& expected, const Vector3& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
  Assert::AreEqual(expected.z, actual.z);
}
static void AreEqualVector(const Vector4& expected, const Vector4& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
  Assert::AreEqual(expected.z, actual.z);
  Assert::AreEqual(expected.w, actual.w);
}
static void AreEqualVector(const glm::vec2& expected, const Vector2& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
}
static void AreEqualVector(const glm::vec3& expected, const Vector3& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
  Assert::AreEqual(expected.z, actual.z);
}
static void AreEqualVector(const glm::vec4& expected, const Vector4& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
  Assert::AreEqual(expected.z, actual.z);
  Assert::AreEqual(expected.w, actual.w);
}

#pragma endregion

#pragma region AreEqualVector (with float tolerance)

static void AreEqualVector(const Vector2& expected, const Vector2& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
}
static void AreEqualVector(const Vector3& expected, const Vector3& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
  Assert::AreEqual(expected.z, actual.z, tolerance);
}
static void AreEqualVector(const Vector4& expected, const Vector4& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
  Assert::AreEqual(expected.z, actual.z, tolerance);
  Assert::AreEqual(expected.w, actual.w, tolerance);
}
static void AreEqualVector(const glm::vec2& expected, const Vector2& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
}
static void AreEqualVector(const glm::vec3& expected, const Vector3& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
  Assert::AreEqual(expected.z, actual.z, tolerance);
}
static void AreEqualVector(const glm::vec4& expected, const Vector4& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
  Assert::AreEqual(expected.z, actual.z, tolerance);
  Assert::AreEqual(expected.w, actual.w, tolerance);
}

#pragma endregion


// Special functions for comparing Quaternion and glm::quat
#pragma region AreEqualQuaternion

static void AreEqualQuaternion(const Quaternion& expected, const Quaternion& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
  Assert::AreEqual(expected.z, actual.z);
  Assert::AreEqual(expected.w, actual.w);
}
static void AreEqualQuaternion(const glm::quat& expected, const Quaternion& actual)
{
  Assert::AreEqual(expected.x, actual.x);
  Assert::AreEqual(expected.y, actual.y);
  Assert::AreEqual(expected.z, actual.z);
  Assert::AreEqual(expected.w, actual.w);
}

#pragma endregion

#pragma region AreEqualQuaternion (with float tolerance)

static void AreEqualQuaternion(const Quaternion& expected, const Quaternion& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
  Assert::AreEqual(expected.z, actual.z, tolerance);
  Assert::AreEqual(expected.w, actual.w, tolerance);
}
static void AreEqualQuaternion(const glm::quat& expected, const Quaternion& actual, const float tolerance)
{
  Assert::AreEqual(expected.x, actual.x, tolerance);
  Assert::AreEqual(expected.y, actual.y, tolerance);
  Assert::AreEqual(expected.z, actual.z, tolerance);
  Assert::AreEqual(expected.w, actual.w, tolerance);
}

#pragma endregion


// Special functions for comparing Matrix4x4 and glm::mat4
#pragma region AreEqualMatrix

static void AreEqualMatrix(const Matrix4x4& expected, const Matrix4x4& actual)
{
  for (int i = 0; i < 16; i++)
  {
    Assert::AreEqual(expected.data[i], actual.data[i]);
  }
}

static void AreEqualMatrix(const glm::mat4& expected, const Matrix4x4& actual)
{
  for (int i = 0; i < 16; i++)
  {
    Assert::AreEqual(expected[i / 4][i % 4], actual.data[i]);
  }
}

#pragma endregion

#pragma region AreEqualMatrix (with float tolerance)

static void AreEqualMatrix(const Matrix4x4& expected, const Matrix4x4& actual, const float tolerance)
{
  for (int i = 0; i < 16; i++)
  {
    Assert::AreEqual(expected[i], actual[i], tolerance);
  }
}

static void AreEqualMatrix(const glm::mat4& expected, const Matrix4x4& actual, const float tolerance)
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      Assert::AreEqual(expected[i][j], actual(i, j), tolerance);
    }
  }
}

#pragma endregion


#pragma warning(pop)

#pragma endregion


namespace T_FlexMath
{

  #pragma region Vector2

  namespace T_Vector2
  {

    TEST_CLASS(T_Constructors)
    {
    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(DefaultConstructor)
      {
        Vector2 a;
        Assert::AreEqual(0.0f, a.x);
        Assert::AreEqual(0.0f, a.y);
      }

      TEST_METHOD(ValueConstructor)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
      }

      TEST_METHOD(CopyConstructor)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Vector2 b = Vector2(a);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
      }

    };

    TEST_CLASS(T_Accessors)
    {
      Vector2 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(XY)
      {
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
      }

      TEST_METHOD(RG)
      {
        Assert::AreEqual(1.0f, a.r);
        Assert::AreEqual(2.0f, a.g);
      }

      TEST_METHOD(ST)
      {
        Assert::AreEqual(1.0f, a.s);
        Assert::AreEqual(2.0f, a.t);
      }

      TEST_METHOD(FirstSecond)
      {
        Assert::AreEqual(1.0f, a.first);
        Assert::AreEqual(2.0f, a.second);
      }

      TEST_METHOD(Array)
      {
        Assert::AreEqual(1.0f, a[0]);
        Assert::AreEqual(2.0f, a[1]);
      }

      TEST_METHOD(DataArray)
      {
        Assert::AreEqual(1.0f, a.data[0]);
        Assert::AreEqual(2.0f, a.data[1]);
      }

    };

    TEST_CLASS(T_ConversionOperators)
    {
      Vector2 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(ToVector4)
      {
        AreEqualVector(Vector4(1.0f, 2.0f, 0.0f, 0.0f), (Vector4)a);
      }

      TEST_METHOD(ToVector3)
      {
        AreEqualVector(Vector3(1.0f, 2.0f, 0.0f), (Vector3)a);
      }

      TEST_METHOD(ToVector1)
      {
        Assert::AreEqual(1.0f, (Vector1)a);
      }

      TEST_METHOD(ToBool)
      {
        Assert::IsTrue((bool)a);
      }

      TEST_METHOD(ToBool_EmptyVector)
      {
        Assert::IsFalse((bool)Vector4(0, 0, 0, 0));
      }

    };

    //TEST_CLASS(T_Swizzling)

    TEST_CLASS(T_UnaryOperators)
    {
      Vector2 a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f };
        b = { 4.0f, 5.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Negation)
      {
        AreEqualVector(Vector2(-a.x, -a.y), -a);
      }

      TEST_METHOD(Comparison_Equality)
      {
        Assert::IsTrue(a == Vector2(1.0f, 2.0f));
        Assert::IsFalse(a == Vector2(4.0f, 5.0f));
      }

      TEST_METHOD(Comparison_Inequality)
      {
        Assert::IsFalse(a != Vector2(1.0f, 2.0f));
        Assert::IsTrue(a != Vector2(4.0f, 5.0f));
      }

      TEST_METHOD(Addition_Vector)
      {
        a += b;
        AreEqualVector(Vector2(5.0f, 7.0f), a);
      }

      TEST_METHOD(Addition_Value)
      {
        a += 4.0f;
        AreEqualVector(Vector2(5.0f, 6.0f), a);
      }

      TEST_METHOD(Subtraction_Vector)
      {
        a -= b;
        AreEqualVector(Vector2(-3.0f, -3.0f), a);
      }

      TEST_METHOD(Subtraction_Value)
      {
        a -= 4.0f;
        AreEqualVector(Vector2(-3.0f, -2.0f), a);
      }

      TEST_METHOD(Multiplication_Vector)
      {
        a *= b;
        AreEqualVector(Vector2(4.0f, 10.0f), a);
      }

      TEST_METHOD(Multiplication_Value)
      {
        a *= 4.0f;
        AreEqualVector(Vector2(4.0f, 8.0f), a);
      }

      TEST_METHOD(Division_Vec)
      {
        a /= b;
        AreEqualVector(Vector2(1.0f / 4.0f, 2.0f / 5.0f), a);
      }

      TEST_METHOD(Division_Value)
      {
        a /= 4.0f;
        AreEqualVector(Vector2(1.0f / 4.0f, 2.0f / 4.0f), a);
      }

      TEST_METHOD(Division_ValueZero)
      {
        a /= 0.0f;
        AreEqualVector(Vector2(1.0f, 2.0f), a);
      }

    };

    TEST_CLASS(T_BinaryOperators)
    {
      Vector2 a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f };
        b = { 4.0f, 5.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Addition_VectorToVector)
      {
        AreEqualVector(Vector2(5.0f, 7.0f), a + b);
      }

      TEST_METHOD(Addition_VectorToValue)
      {
        AreEqualVector(Vector2(5.0f, 6.0f), a + 4.0f);
      }

      TEST_METHOD(Addition_ValueToVector)
      {
        AreEqualVector(Vector2(5.0f, 6.0f), 4.0f + a);
      }

      TEST_METHOD(Subtraction_VectorToVector)
      {
        AreEqualVector(Vector2(-3.0f, -3.0f), a - b);
      }

      TEST_METHOD(Subtraction_VectorToValue)
      {
        AreEqualVector(Vector2(-3.0f, -2.0f), a - 4.0f);
      }

      TEST_METHOD(Subtraction_ValueToVector)
      {
        AreEqualVector(Vector2(3.0f, 2.0f), 4.0f - a);
      }

      TEST_METHOD(Multiplication_VectorToValue)
      {
        AreEqualVector(Vector2(4.0f, 8.0f), a * 4.0f);
      }

      TEST_METHOD(Multiplication_ValueToVector)
      {
        AreEqualVector(Vector2(4.0f, 8.0f), 4.0f * a);
      }

      TEST_METHOD(Division_VectorToValue)
      {
        AreEqualVector(Vector2(1.0f / 4.0f, 2.0f / 4.0f), a / 4.0f);
      }

      TEST_METHOD(Division_ValueToVector)
      {
        AreEqualVector(Vector2(4.0f / 1.0f, 4.0f / 2.0f), 4.0f / a);
      }

    };

    TEST_CLASS(T_Functions)
    {
      Vector2 a, b;
      glm::vec2 glm_a, glm_b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f };
        b = { 4.0f, 5.0f };
        glm_a = { 1.0f, 2.0f };
        glm_b = { 4.0f, 5.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      //TEST_METHOD(Rotate)

      TEST_METHOD(Magnitude_LengthParity)
      {
        Assert::IsTrue(a.Magnitude() == a.Length());
      }

      TEST_METHOD(Magnitude_LengthSqrParity)
      {
        Assert::IsTrue(a.Length() == sqrt(a.LengthSqr()));
      }

      TEST_METHOD(Magnitude_Calculation)
      {
        Assert::AreEqual(glm::length(glm_a), a.Length());
      }

      TEST_METHOD(Normalize_StaticParity)
      {
        Vector2 my_normalize_static = Vector2::Normalize(a);
        Vector2 my_normalize = a.Normalize();
        Assert::IsTrue(my_normalize == my_normalize_static);
      }

      TEST_METHOD(Normalize_LengthCheck)
      {
        Assert::AreEqual(1.0f, Vector2::Normalize(a).Length(), EPSILONf);
      }

      TEST_METHOD(Normalize_Calculation)
      {
        AreEqualVector(glm::normalize(glm_a), Vector2::Normalize(a));
      }

      TEST_METHOD(DotProduct)
      {
        Assert::AreEqual(glm::dot(glm_a, glm_b), Dot(a, b));
      }

      TEST_METHOD(LinearInterpolation)
      {
        AreEqualVector(glm::mix(glm_a, glm_b, 0.5f), Lerp(a, b, 0.5f));
      }

    };

    TEST_CLASS(T_ConversionOverloads)
    {
      Vector2 a;
      glm::vec2 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f };
        glm_a = { 1.0f, 2.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(DegreeToRadians)
      {
        AreEqualVector(glm::radians(glm_a), radians(a));
      }

      TEST_METHOD(RadiansToDegrees)
      {
        AreEqualVector(glm::degrees(glm_a), degrees(a));
      }

    };

  }

  #pragma endregion

  #pragma region Vector3

  namespace T_Vector3
  {

    TEST_CLASS(T_Constructors)
    {
    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(DefaultConstructor)
      {
        Vector3 a;
        Assert::AreEqual(0.0f, a.x);
        Assert::AreEqual(0.0f, a.y);
        Assert::AreEqual(0.0f, a.z);
      }

      TEST_METHOD(ValueConstructor)
      {
        Vector3 a = Vector3(1.0f, 2.0f, 3.0f);
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
        Assert::AreEqual(3.0f, a.z);
      }

      TEST_METHOD(CopyConstructor)
      {
        Vector3 a = Vector3(1.0f, 2.0f, 3.0f);
        Vector3 b = Vector3(a);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
        Assert::AreEqual(3.0f, b.z);
      }

      TEST_METHOD(Vector2Constructor_Left)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Vector3 b = Vector3(a, 3.0f);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
        Assert::AreEqual(3.0f, b.z);
      }

      TEST_METHOD(Vector2Constructor_Right)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Vector3 b = Vector3(3.0f, a);
        Assert::AreEqual(3.0f, b.x);
        Assert::AreEqual(1.0f, b.y);
        Assert::AreEqual(2.0f, b.z);
      }

    };

    TEST_CLASS(T_Accessors)
    {
      Vector3 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }
      
      TEST_METHOD(XYZ)
      {
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
        Assert::AreEqual(3.0f, a.z);
      }

      TEST_METHOD(RGB)
      {
        Assert::AreEqual(1.0f, a.r);
        Assert::AreEqual(2.0f, a.g);
        Assert::AreEqual(3.0f, a.b);
      }

      TEST_METHOD(STP)
      {
        Assert::AreEqual(1.0f, a.s);
        Assert::AreEqual(2.0f, a.t);
        Assert::AreEqual(3.0f, a.p);
      }

      TEST_METHOD(FirstSecondThird)
      {
        Assert::AreEqual(1.0f, a.first);
        Assert::AreEqual(2.0f, a.second);
        Assert::AreEqual(3.0f, a.third);
      }

      TEST_METHOD(Array)
      {
        Assert::AreEqual(1.0f, a[0]);
        Assert::AreEqual(2.0f, a[1]);
        Assert::AreEqual(3.0f, a[2]);
      }

      TEST_METHOD(DataArray)
      {
        Assert::AreEqual(1.0f, a.data[0]);
        Assert::AreEqual(2.0f, a.data[1]);
        Assert::AreEqual(3.0f, a.data[2]);
      }

    };

    TEST_CLASS(T_ConversionOperators)
    {
      Vector3 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(ToVector4)
      {
        AreEqualVector(Vector4(1.0f, 2.0f, 3.0f, 0.0f), (Vector4)a);
      }

      TEST_METHOD(ToVector2)
      {
        AreEqualVector(Vector2(1.0f, 2.0f), (Vector2)a);
      }

      TEST_METHOD(ToVector1)
      {
        Assert::AreEqual(1.0f, (Vector1)a);
      }

      TEST_METHOD(ToBool)
      {
        Assert::IsTrue((bool)a);
      }

      TEST_METHOD(ToBool_EmptyVector)
      {
        Assert::IsFalse((bool)Vector3(0, 0, 0));
      }

    };

    TEST_CLASS(T_Swizzling)
    {
      Vector3 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Swizzling_Vector4)
      {
        // Since swizzling is not very important.
        // I'm not going to natively support swizzling up.
        // Do it this way instead.
        AreEqualVector(Vector4(a.x, a.x, a.x, a.x), Vector4(a.Swizzle("xxx"), a.x));
        AreEqualVector(Vector4(1.0f, a.z, a.y, a.x), Vector4(1.0f, a.Swizzle("zyx")));
      }

      TEST_METHOD(Swizzling_Vector3_Same)
      {
        AreEqualVector(a, a.Swizzle());
        AreEqualVector(a, a.Swizzle("xyz"));
      }

      TEST_METHOD(Swizzling_Vector3)
      {
        AreEqualVector(Vector3(a.x, a.x, a.x), a.Swizzle("xxx"));
        AreEqualVector(Vector3(a.x, a.x, a.y), a.Swizzle("xxy"));
        AreEqualVector(Vector3(a.x, a.x, a.z), a.Swizzle("xxz"));
        AreEqualVector(Vector3(a.x, a.y, a.x), a.Swizzle("xyx"));
        AreEqualVector(Vector3(a.x, a.y, a.y), a.Swizzle("xyy"));
        AreEqualVector(Vector3(a.x, a.y, a.z), a.Swizzle("xyz"));
        AreEqualVector(Vector3(a.x, a.z, a.x), a.Swizzle("xzx"));
        AreEqualVector(Vector3(a.x, a.z, a.y), a.Swizzle("xzy"));
        AreEqualVector(Vector3(a.x, a.z, a.z), a.Swizzle("xzz"));

        AreEqualVector(Vector3(a.y, a.x, a.x), a.Swizzle("yxx"));
        AreEqualVector(Vector3(a.y, a.x, a.y), a.Swizzle("yxy"));
        AreEqualVector(Vector3(a.y, a.x, a.z), a.Swizzle("yxz"));
        AreEqualVector(Vector3(a.y, a.y, a.x), a.Swizzle("yyx"));
        AreEqualVector(Vector3(a.y, a.y, a.y), a.Swizzle("yyy"));
        AreEqualVector(Vector3(a.y, a.y, a.z), a.Swizzle("yyz"));
        AreEqualVector(Vector3(a.y, a.z, a.x), a.Swizzle("yzx"));
        AreEqualVector(Vector3(a.y, a.z, a.y), a.Swizzle("yzy"));
        AreEqualVector(Vector3(a.y, a.z, a.z), a.Swizzle("yzz"));

        AreEqualVector(Vector3(a.z, a.x, a.x), a.Swizzle("zxx"));
        AreEqualVector(Vector3(a.z, a.x, a.y), a.Swizzle("zxy"));
        AreEqualVector(Vector3(a.z, a.x, a.z), a.Swizzle("zxz"));
        AreEqualVector(Vector3(a.z, a.y, a.x), a.Swizzle("zyx"));
        AreEqualVector(Vector3(a.z, a.y, a.y), a.Swizzle("zyy"));
        AreEqualVector(Vector3(a.z, a.y, a.z), a.Swizzle("zyz"));
        AreEqualVector(Vector3(a.z, a.z, a.x), a.Swizzle("zzx"));
        AreEqualVector(Vector3(a.z, a.z, a.y), a.Swizzle("zzy"));
        AreEqualVector(Vector3(a.z, a.z, a.z), a.Swizzle("zzz"));
      }

      TEST_METHOD(Swizzling_Vector3_2)
      {
        AreEqualVector(Vector3(a.x, a.x), a.Swizzle("xx"));
        AreEqualVector(Vector3(a.x, a.y), a.Swizzle("xy"));
        AreEqualVector(Vector3(a.x, a.z), a.Swizzle("xz"));
        AreEqualVector(Vector3(a.y, a.x), a.Swizzle("yx"));
        AreEqualVector(Vector3(a.y, a.y), a.Swizzle("yy"));
        AreEqualVector(Vector3(a.y, a.z), a.Swizzle("yz"));
        AreEqualVector(Vector3(a.z, a.x), a.Swizzle("zx"));
        AreEqualVector(Vector3(a.z, a.y), a.Swizzle("zy"));
        AreEqualVector(Vector3(a.z, a.z), a.Swizzle("zz"));
      }

      TEST_METHOD(Swizzling_Vector3_1)
      {
        AreEqualVector(Vector3(a.x), a.Swizzle("x"));
        AreEqualVector(Vector3(a.y), a.Swizzle("y"));
        AreEqualVector(Vector3(a.z), a.Swizzle("z"));
      }

      TEST_METHOD(Swizzling_Vector2)
      {
        AreEqualVector(Vector2(a.x, a.x), (Vector2)a.Swizzle("xx"));
        AreEqualVector(Vector2(a.x, a.y), (Vector2)a.Swizzle("xy"));
        AreEqualVector(Vector2(a.x, a.z), (Vector2)a.Swizzle("xz"));
        AreEqualVector(Vector2(a.y, a.x), (Vector2)a.Swizzle("yx"));
        AreEqualVector(Vector2(a.y, a.y), (Vector2)a.Swizzle("yy"));
        AreEqualVector(Vector2(a.y, a.z), (Vector2)a.Swizzle("yz"));
        AreEqualVector(Vector2(a.z, a.x), (Vector2)a.Swizzle("zx"));
        AreEqualVector(Vector2(a.z, a.y), (Vector2)a.Swizzle("zy"));
        AreEqualVector(Vector2(a.z, a.z), (Vector2)a.Swizzle("zz"));
      }

      TEST_METHOD(Swizzling_Vector2_1)
      {
        AreEqualVector(Vector2(a.x), (Vector2)a.Swizzle("x"));
        AreEqualVector(Vector2(a.y), (Vector2)a.Swizzle("y"));
        AreEqualVector(Vector2(a.z), (Vector2)a.Swizzle("z"));
      }

      TEST_METHOD(Swizzling_Vector1)
      {
        Assert::AreEqual(Vector1(a.x), (Vector1)a.Swizzle("x"));
        Assert::AreEqual(Vector1(a.y), (Vector1)a.Swizzle("y"));
        Assert::AreEqual(Vector1(a.z), (Vector1)a.Swizzle("z"));
      }

    };

    TEST_CLASS(T_UnaryOperators)
    {
      Vector3 a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f };
        b = { 4.0f, 5.0f, 6.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Negation)
      {
        AreEqualVector(Vector3(-a.x, -a.y, -a.z), -a);
      }

      TEST_METHOD(Comparison_Equality)
      {
        Assert::IsTrue(a == Vector3(1.0f, 2.0f, 3.0f));
        Assert::IsFalse(a == Vector3(4.0f, 5.0f, 6.0f));
      }

      TEST_METHOD(Comparison_Inequality)
      {
        Assert::IsFalse(a != Vector3(1.0f, 2.0f, 3.0f));
        Assert::IsTrue(a != Vector3(4.0f, 5.0f, 6.0f));
      }

      TEST_METHOD(Addition_Vector)
      {
        a += b;
        AreEqualVector(Vector3(5.0f, 7.0f, 9.0f), a);
      }

      TEST_METHOD(Addition_Value)
      {
        a += 4.0f;
        AreEqualVector(Vector3(5.0f, 6.0f, 7.0f), a);
      }

      TEST_METHOD(Subtraction_Vector)
      {
        a -= b;
        AreEqualVector(Vector3(-3.0f, -3.0f, -3.0f), a);
      }

      TEST_METHOD(Subtraction_Value)
      {
        a -= 4.0f;
        AreEqualVector(Vector3(-3.0f, -2.0f, -1.0f), a);
      }

      TEST_METHOD(Multiplication_Vector)
      {
        a *= b;
        AreEqualVector(Vector3(4.0f, 10.0f, 18.0f), a);
      }

      TEST_METHOD(Multiplication_Value)
      {
        a *= 4.0f;
        AreEqualVector(Vector3(4.0f, 8.0f, 12.0f), a);
      }

      TEST_METHOD(Division_Vec)
      {
        a /= b;
        AreEqualVector(Vector3(1.0f / 4.0f, 2.0f / 5.0f, 3.0f / 6.0f), a);
      }

      TEST_METHOD(Division_Value)
      {
        a /= 4.0f;
        AreEqualVector(Vector3(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f), a);
      }

      TEST_METHOD(Division_ValueZero)
      {
        a /= 0.0f;
        AreEqualVector(Vector3(1.0f, 2.0f, 3.0f), a);
      }

    };

    TEST_CLASS(T_BinaryOperators)
    {
      Vector3 a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f };
        b = { 4.0f, 5.0f, 6.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Addition_VectorToVector)
      {
        AreEqualVector(Vector3(5.0f, 7.0f, 9.0f), a + b);
      }

      TEST_METHOD(Addition_VectorToValue)
      {
        AreEqualVector(Vector3(5.0f, 6.0f, 7.0f), a + 4.0f);
      }

      TEST_METHOD(Addition_ValueToVector)
      {
        AreEqualVector(Vector3(5.0f, 6.0f, 7.0f), 4.0f + a);
      }

      TEST_METHOD(Subtraction_VectorToVector)
      {
        AreEqualVector(Vector3(-3.0f, -3.0f, -3.0f), a - b);
      }

      TEST_METHOD(Subtraction_VectorToValue)
      {
        AreEqualVector(Vector3(-3.0f, -2.0f, -1.0f), a - 4.0f);
      }

      TEST_METHOD(Subtraction_ValueToVector)
      {
        AreEqualVector(Vector3(3.0f, 2.0f, 1.0f), 4.0f - a);
      }

      TEST_METHOD(Multiplication_VectorToValue)
      {
        AreEqualVector(Vector3(4.0f, 8.0f, 12.0f), a * 4.0f);
      }

      TEST_METHOD(Multiplication_ValueToVector)
      {
        AreEqualVector(Vector3(4.0f, 8.0f, 12.0f), 4.0f * a);
      }

      TEST_METHOD(Division_VectorToValue)
      {
        AreEqualVector(Vector3(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f), a / 4.0f);
      }

      TEST_METHOD(Division_ValueToVector)
      {
        AreEqualVector(Vector3(4.0f / 1.0f, 4.0f / 2.0f, 4.0f / 3.0f), 4.0f / a);
      }

    };

    TEST_CLASS(T_Functions)
    {
      Vector3 a, b;
      glm::vec3 glm_a, glm_b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f };
        b = { 4.0f, 5.0f, 6.0f };
        glm_a = { 1.0f, 2.0f, 3.0f };
        glm_b = { 4.0f, 5.0f, 6.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Magnitude_LengthParity)
      {
        Assert::IsTrue(a.Magnitude() == a.Length());
      }

      TEST_METHOD(Magnitude_LengthSqrParity)
      {
        Assert::IsTrue(a.Length() == sqrt(a.LengthSqr()));
      }

      TEST_METHOD(Magnitude_Calculation)
      {
        Assert::AreEqual(glm::length(glm_a), a.Length());
      }

      TEST_METHOD(Normalize_StaticParity)
      {
        Vector3 my_normalize_static = Vector3::Normalize(a);
        Vector3 my_normalize = a.Normalize();
        Assert::IsTrue(my_normalize == my_normalize_static);
      }

      TEST_METHOD(Normalize_LengthCheck)
      {
        Assert::AreEqual(1.0f, Vector3::Normalize(a).Length(), EPSILONf);
      }

      TEST_METHOD(Normalize_Calculation)
      {
        AreEqualVector(glm::normalize(glm_a), Vector3::Normalize(a));
      }

      TEST_METHOD(DotProduct)
      {
        Assert::AreEqual(glm::dot(glm_a, glm_b), Dot(a, b));
      }

      TEST_METHOD(CrossProduct)
      {
        AreEqualVector(glm::cross(glm_a, glm_b), Cross(a, b));
      }

      TEST_METHOD(LinearInterpolation)
      {
        AreEqualVector(glm::mix(glm_a, glm_b, 0.5f), Lerp(a, b, 0.5f));
      }

    };

    TEST_CLASS(T_ConversionOverloads)
    {
      Vector3 a;
      glm::vec3 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f };
        glm_a = { 1.0f, 2.0f, 3.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(DegreeToRadians)
      {
        AreEqualVector(glm::radians(glm_a), radians(a));
      }

      TEST_METHOD(RadiansToDegrees)
      {
        AreEqualVector(glm::degrees(glm_a), degrees(a));
      }

    };

  }

  #pragma endregion

  #pragma region Vector4

  namespace T_Vector4
  {

    TEST_CLASS(T_Constructors)
    {
    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(DefaultConstructor)
      {
        Vector4 a;
        Assert::AreEqual(0.0f, a.x);
        Assert::AreEqual(0.0f, a.y);
        Assert::AreEqual(0.0f, a.z);
        Assert::AreEqual(0.0f, a.w);
      }

      TEST_METHOD(ValueConstructor)
      {
        Vector4 a = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
        Assert::AreEqual(3.0f, a.z);
        Assert::AreEqual(4.0f, a.w);
      }

      TEST_METHOD(CopyConstructor)
      {
        Vector4 a = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
        Vector4 b = Vector4(a);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
        Assert::AreEqual(3.0f, b.z);
        Assert::AreEqual(4.0f, b.w);
      }

      TEST_METHOD(Vector3Constructor_Left)
      {
        Vector3 a = Vector3(1.0f, 2.0f, 3.0f);
        Vector4 b = Vector4(a, 4.0f);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
        Assert::AreEqual(3.0f, b.z);
        Assert::AreEqual(4.0f, b.w);
      }

      TEST_METHOD(Vector3Constructor_Right)
      {
        Vector3 a = Vector3(1.0f, 2.0f, 3.0f);
        Vector4 b = Vector4(4.0f, a);
        Assert::AreEqual(4.0f, b.x);
        Assert::AreEqual(1.0f, b.y);
        Assert::AreEqual(2.0f, b.z);
        Assert::AreEqual(3.0f, b.w);
      }

      TEST_METHOD(Vector2Constructor_Left)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Vector4 b = Vector4(a, 3.0f, 4.0f);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
        Assert::AreEqual(3.0f, b.z);
        Assert::AreEqual(4.0f, b.w);
      }

      TEST_METHOD(Vector2Constructor_Middle)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Vector4 b = Vector4(3.0f, a, 4.0f);
        Assert::AreEqual(3.0f, b.x);
        Assert::AreEqual(1.0f, b.y);
        Assert::AreEqual(2.0f, b.z);
        Assert::AreEqual(4.0f, b.w);
      }

      TEST_METHOD(Vector2Constructor_Right)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Vector4 b = Vector4(3.0f, 4.0f, a);
        Assert::AreEqual(3.0f, b.x);
        Assert::AreEqual(4.0f, b.y);
        Assert::AreEqual(1.0f, b.z);
        Assert::AreEqual(2.0f, b.w);
      }

      TEST_METHOD(Vector2Constructor_Both)
      {
        Vector2 a = Vector2(1.0f, 2.0f);
        Vector2 b = Vector2(3.0f, 4.0f);
        Vector4 c = Vector4(a, b);
        Assert::AreEqual(1.0f, c.x);
        Assert::AreEqual(2.0f, c.y);
        Assert::AreEqual(3.0f, c.z);
        Assert::AreEqual(4.0f, c.w);
      }

    };

    TEST_CLASS(T_Accessors)
    {
      Vector4 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(XYZW)
      {
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
        Assert::AreEqual(3.0f, a.z);
        Assert::AreEqual(4.0f, a.w);
      }

      TEST_METHOD(RGBA)
      {
        Assert::AreEqual(1.0f, a.r);
        Assert::AreEqual(2.0f, a.g);
        Assert::AreEqual(3.0f, a.b);
        Assert::AreEqual(4.0f, a.a);
      }

      TEST_METHOD(STPQ)
      {
        Assert::AreEqual(1.0f, a.s);
        Assert::AreEqual(2.0f, a.t);
        Assert::AreEqual(3.0f, a.p);
        Assert::AreEqual(4.0f, a.q);
      }

      TEST_METHOD(FirstSecondThirdFourth)
      {
        Assert::AreEqual(1.0f, a.first);
        Assert::AreEqual(2.0f, a.second);
        Assert::AreEqual(3.0f, a.third);
        Assert::AreEqual(4.0f, a.fourth);
      }

      TEST_METHOD(Array)
      {
        Assert::AreEqual(1.0f, a[0]);
        Assert::AreEqual(2.0f, a[1]);
        Assert::AreEqual(3.0f, a[2]);
        Assert::AreEqual(4.0f, a[3]);
      }

      TEST_METHOD(DataArray)
      {
        Assert::AreEqual(1.0f, a.data[0]);
        Assert::AreEqual(2.0f, a.data[1]);
        Assert::AreEqual(3.0f, a.data[2]);
        Assert::AreEqual(4.0f, a.data[3]);
      }

    };

    TEST_CLASS(T_ConversionOperators)
    {
      Vector4 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(ToVector3)
      {
        AreEqualVector(Vector3(1.0f, 2.0f, 3.0f), (Vector3)a);
      }

      TEST_METHOD(ToVector2)
      {
        AreEqualVector(Vector2(1.0f, 2.0f), (Vector2)a);
      }

      TEST_METHOD(ToVector1)
      {
        Assert::AreEqual(1.0f, (Vector1)a);
      }

      TEST_METHOD(ToBool)
      {
        Assert::IsTrue((bool)a);
      }

      TEST_METHOD(ToBool_EmptyVector)
      {
        Assert::IsFalse((bool)Vector4(0, 0, 0, 0));
      }

    };

    TEST_CLASS(T_Swizzling)
    {
      Vector4 a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Swizzling_Vector4_Same)
      {
        AreEqualVector(a, a.Swizzle());
        AreEqualVector(a, a.Swizzle("xyzw"));
      }

      TEST_METHOD(Swizzling_Vector4)
      {
        AreEqualVector(Vector4(a.x, a.x, a.x, a.x), a.Swizzle("xxxx"));
        AreEqualVector(Vector4(a.x, a.x, a.y, a.x), a.Swizzle("xxyx"));
        AreEqualVector(Vector4(a.x, a.x, a.z, a.x), a.Swizzle("xxzx"));
        AreEqualVector(Vector4(a.x, a.y, a.x, a.x), a.Swizzle("xyxx"));
        AreEqualVector(Vector4(a.x, a.y, a.y, a.x), a.Swizzle("xyyx"));
        AreEqualVector(Vector4(a.x, a.y, a.z, a.x), a.Swizzle("xyzx"));
        AreEqualVector(Vector4(a.x, a.z, a.x, a.x), a.Swizzle("xzxx"));
        AreEqualVector(Vector4(a.x, a.z, a.y, a.x), a.Swizzle("xzyx"));
        AreEqualVector(Vector4(a.x, a.z, a.z, a.x), a.Swizzle("xzzx"));
        AreEqualVector(Vector4(a.y, a.x, a.x, a.x), a.Swizzle("yxxx"));
        AreEqualVector(Vector4(a.y, a.x, a.y, a.x), a.Swizzle("yxyx"));
        AreEqualVector(Vector4(a.y, a.x, a.z, a.x), a.Swizzle("yxzx"));
        AreEqualVector(Vector4(a.y, a.y, a.x, a.x), a.Swizzle("yyxx"));
        AreEqualVector(Vector4(a.y, a.y, a.y, a.x), a.Swizzle("yyyx"));
        AreEqualVector(Vector4(a.y, a.y, a.z, a.x), a.Swizzle("yyzx"));
        AreEqualVector(Vector4(a.y, a.z, a.x, a.x), a.Swizzle("yzxx"));
        AreEqualVector(Vector4(a.y, a.z, a.y, a.x), a.Swizzle("yzyx"));
        AreEqualVector(Vector4(a.y, a.z, a.z, a.x), a.Swizzle("yzzx"));
        AreEqualVector(Vector4(a.z, a.x, a.x, a.x), a.Swizzle("zxxx"));
        AreEqualVector(Vector4(a.z, a.x, a.y, a.x), a.Swizzle("zxyx"));
        AreEqualVector(Vector4(a.z, a.x, a.z, a.x), a.Swizzle("zxzx"));
        AreEqualVector(Vector4(a.z, a.y, a.x, a.x), a.Swizzle("zyxx"));
        AreEqualVector(Vector4(a.z, a.y, a.y, a.x), a.Swizzle("zyyx"));
        AreEqualVector(Vector4(a.z, a.y, a.z, a.x), a.Swizzle("zyzx"));
        AreEqualVector(Vector4(a.z, a.z, a.x, a.x), a.Swizzle("zzxx"));
        AreEqualVector(Vector4(a.z, a.z, a.y, a.x), a.Swizzle("zzyx"));
        AreEqualVector(Vector4(a.z, a.z, a.z, a.x), a.Swizzle("zzzx"));

        AreEqualVector(Vector4(a.x, a.x, a.x, a.y), a.Swizzle("xxxy"));
        AreEqualVector(Vector4(a.x, a.x, a.y, a.y), a.Swizzle("xxyy"));
        AreEqualVector(Vector4(a.x, a.x, a.z, a.y), a.Swizzle("xxzy"));
        AreEqualVector(Vector4(a.x, a.y, a.x, a.y), a.Swizzle("xyxy"));
        AreEqualVector(Vector4(a.x, a.y, a.y, a.y), a.Swizzle("xyyy"));
        AreEqualVector(Vector4(a.x, a.y, a.z, a.y), a.Swizzle("xyzy"));
        AreEqualVector(Vector4(a.x, a.z, a.x, a.y), a.Swizzle("xzxy"));
        AreEqualVector(Vector4(a.x, a.z, a.y, a.y), a.Swizzle("xzyy"));
        AreEqualVector(Vector4(a.x, a.z, a.z, a.y), a.Swizzle("xzzy"));
        AreEqualVector(Vector4(a.y, a.x, a.x, a.y), a.Swizzle("yxxy"));
        AreEqualVector(Vector4(a.y, a.x, a.y, a.y), a.Swizzle("yxyy"));
        AreEqualVector(Vector4(a.y, a.x, a.z, a.y), a.Swizzle("yxzy"));
        AreEqualVector(Vector4(a.y, a.y, a.x, a.y), a.Swizzle("yyxy"));
        AreEqualVector(Vector4(a.y, a.y, a.y, a.y), a.Swizzle("yyyy"));
        AreEqualVector(Vector4(a.y, a.y, a.z, a.y), a.Swizzle("yyzy"));
        AreEqualVector(Vector4(a.y, a.z, a.x, a.y), a.Swizzle("yzxy"));
        AreEqualVector(Vector4(a.y, a.z, a.y, a.y), a.Swizzle("yzyy"));
        AreEqualVector(Vector4(a.y, a.z, a.z, a.y), a.Swizzle("yzzy"));
        AreEqualVector(Vector4(a.z, a.x, a.x, a.y), a.Swizzle("zxxy"));
        AreEqualVector(Vector4(a.z, a.x, a.y, a.y), a.Swizzle("zxyy"));
        AreEqualVector(Vector4(a.z, a.x, a.z, a.y), a.Swizzle("zxzy"));
        AreEqualVector(Vector4(a.z, a.y, a.x, a.y), a.Swizzle("zyxy"));
        AreEqualVector(Vector4(a.z, a.y, a.y, a.y), a.Swizzle("zyyy"));
        AreEqualVector(Vector4(a.z, a.y, a.z, a.y), a.Swizzle("zyzy"));
        AreEqualVector(Vector4(a.z, a.z, a.x, a.y), a.Swizzle("zzxy"));
        AreEqualVector(Vector4(a.z, a.z, a.y, a.y), a.Swizzle("zzyy"));
        AreEqualVector(Vector4(a.z, a.z, a.z, a.y), a.Swizzle("zzzy"));

        AreEqualVector(Vector4(a.x, a.x, a.x, a.z), a.Swizzle("xxxz"));
        AreEqualVector(Vector4(a.x, a.x, a.y, a.z), a.Swizzle("xxyz"));
        AreEqualVector(Vector4(a.x, a.x, a.z, a.z), a.Swizzle("xxzz"));
        AreEqualVector(Vector4(a.x, a.y, a.x, a.z), a.Swizzle("xyxz"));
        AreEqualVector(Vector4(a.x, a.y, a.y, a.z), a.Swizzle("xyyz"));
        AreEqualVector(Vector4(a.x, a.y, a.z, a.z), a.Swizzle("xyzz"));
        AreEqualVector(Vector4(a.x, a.z, a.x, a.z), a.Swizzle("xzxz"));
        AreEqualVector(Vector4(a.x, a.z, a.y, a.z), a.Swizzle("xzyz"));
        AreEqualVector(Vector4(a.x, a.z, a.z, a.z), a.Swizzle("xzzz"));
        AreEqualVector(Vector4(a.y, a.x, a.x, a.z), a.Swizzle("yxxz"));
        AreEqualVector(Vector4(a.y, a.x, a.y, a.z), a.Swizzle("yxyz"));
        AreEqualVector(Vector4(a.y, a.x, a.z, a.z), a.Swizzle("yxzz"));
        AreEqualVector(Vector4(a.y, a.y, a.x, a.z), a.Swizzle("yyxz"));
        AreEqualVector(Vector4(a.y, a.y, a.y, a.z), a.Swizzle("yyyz"));
        AreEqualVector(Vector4(a.y, a.y, a.z, a.z), a.Swizzle("yyzz"));
        AreEqualVector(Vector4(a.y, a.z, a.x, a.z), a.Swizzle("yzxz"));
        AreEqualVector(Vector4(a.y, a.z, a.y, a.z), a.Swizzle("yzyz"));
        AreEqualVector(Vector4(a.y, a.z, a.z, a.z), a.Swizzle("yzzz"));
        AreEqualVector(Vector4(a.z, a.x, a.x, a.z), a.Swizzle("zxxz"));
        AreEqualVector(Vector4(a.z, a.x, a.y, a.z), a.Swizzle("zxyz"));
        AreEqualVector(Vector4(a.z, a.x, a.z, a.z), a.Swizzle("zxzz"));
        AreEqualVector(Vector4(a.z, a.y, a.x, a.z), a.Swizzle("zyxz"));
        AreEqualVector(Vector4(a.z, a.y, a.y, a.z), a.Swizzle("zyyz"));
        AreEqualVector(Vector4(a.z, a.y, a.z, a.z), a.Swizzle("zyzz"));
        AreEqualVector(Vector4(a.z, a.z, a.x, a.z), a.Swizzle("zzxz"));
        AreEqualVector(Vector4(a.z, a.z, a.y, a.z), a.Swizzle("zzyz"));
        AreEqualVector(Vector4(a.z, a.z, a.z, a.z), a.Swizzle("zzzz"));

        AreEqualVector(Vector4(a.x, a.x, a.x, a.w), a.Swizzle("xxxw"));
        AreEqualVector(Vector4(a.x, a.x, a.y, a.w), a.Swizzle("xxyw"));
        AreEqualVector(Vector4(a.x, a.x, a.z, a.w), a.Swizzle("xxzw"));
        AreEqualVector(Vector4(a.x, a.y, a.x, a.w), a.Swizzle("xyxw"));
        AreEqualVector(Vector4(a.x, a.y, a.y, a.w), a.Swizzle("xyyw"));
        AreEqualVector(Vector4(a.x, a.y, a.z, a.w), a.Swizzle("xyzw"));
        AreEqualVector(Vector4(a.x, a.z, a.x, a.w), a.Swizzle("xzxw"));
        AreEqualVector(Vector4(a.x, a.z, a.y, a.w), a.Swizzle("xzyw"));
        AreEqualVector(Vector4(a.x, a.z, a.z, a.w), a.Swizzle("xzzw"));
        AreEqualVector(Vector4(a.y, a.x, a.x, a.w), a.Swizzle("yxxw"));
        AreEqualVector(Vector4(a.y, a.x, a.y, a.w), a.Swizzle("yxyw"));
        AreEqualVector(Vector4(a.y, a.x, a.z, a.w), a.Swizzle("yxzw"));
        AreEqualVector(Vector4(a.y, a.y, a.x, a.w), a.Swizzle("yyxw"));
        AreEqualVector(Vector4(a.y, a.y, a.y, a.w), a.Swizzle("yyyw"));
        AreEqualVector(Vector4(a.y, a.y, a.z, a.w), a.Swizzle("yyzw"));
        AreEqualVector(Vector4(a.y, a.z, a.x, a.w), a.Swizzle("yzxw"));
        AreEqualVector(Vector4(a.y, a.z, a.y, a.w), a.Swizzle("yzyw"));
        AreEqualVector(Vector4(a.y, a.z, a.z, a.w), a.Swizzle("yzzw"));
        AreEqualVector(Vector4(a.z, a.x, a.x, a.w), a.Swizzle("zxxw"));
        AreEqualVector(Vector4(a.z, a.x, a.y, a.w), a.Swizzle("zxyw"));
        AreEqualVector(Vector4(a.z, a.x, a.z, a.w), a.Swizzle("zxzw"));
        AreEqualVector(Vector4(a.z, a.y, a.x, a.w), a.Swizzle("zyxw"));
        AreEqualVector(Vector4(a.z, a.y, a.y, a.w), a.Swizzle("zyyw"));
        AreEqualVector(Vector4(a.z, a.y, a.z, a.w), a.Swizzle("zyzw"));
        AreEqualVector(Vector4(a.z, a.z, a.x, a.w), a.Swizzle("zzxw"));
        AreEqualVector(Vector4(a.z, a.z, a.y, a.w), a.Swizzle("zzyw"));
        AreEqualVector(Vector4(a.z, a.z, a.z, a.w), a.Swizzle("zzzw"));
      }

      TEST_METHOD(Swizzling_Vector3)
      {
        AreEqualVector(Vector3(a.x, a.x, a.x), (Vector3)a.Swizzle("xxx"));
        AreEqualVector(Vector3(a.x, a.x, a.y), (Vector3)a.Swizzle("xxy"));
        AreEqualVector(Vector3(a.x, a.x, a.z), (Vector3)a.Swizzle("xxz"));
        AreEqualVector(Vector3(a.x, a.x, a.w), (Vector3)a.Swizzle("xxw"));
        AreEqualVector(Vector3(a.x, a.y, a.x), (Vector3)a.Swizzle("xyx"));
        AreEqualVector(Vector3(a.x, a.y, a.y), (Vector3)a.Swizzle("xyy"));
        AreEqualVector(Vector3(a.x, a.y, a.z), (Vector3)a.Swizzle("xyz"));
        AreEqualVector(Vector3(a.x, a.y, a.w), (Vector3)a.Swizzle("xyw"));
        AreEqualVector(Vector3(a.x, a.z, a.x), (Vector3)a.Swizzle("xzx"));
        AreEqualVector(Vector3(a.x, a.z, a.y), (Vector3)a.Swizzle("xzy"));
        AreEqualVector(Vector3(a.x, a.z, a.z), (Vector3)a.Swizzle("xzz"));
        AreEqualVector(Vector3(a.x, a.z, a.w), (Vector3)a.Swizzle("xzw"));
        AreEqualVector(Vector3(a.x, a.w, a.x), (Vector3)a.Swizzle("xwx"));
        AreEqualVector(Vector3(a.x, a.w, a.y), (Vector3)a.Swizzle("xwy"));
        AreEqualVector(Vector3(a.x, a.w, a.z), (Vector3)a.Swizzle("xwz"));
        AreEqualVector(Vector3(a.x, a.w, a.w), (Vector3)a.Swizzle("xww"));

        AreEqualVector(Vector3(a.y, a.x, a.x), (Vector3)a.Swizzle("yxx"));
        AreEqualVector(Vector3(a.y, a.x, a.y), (Vector3)a.Swizzle("yxy"));
        AreEqualVector(Vector3(a.y, a.x, a.z), (Vector3)a.Swizzle("yxz"));
        AreEqualVector(Vector3(a.y, a.x, a.w), (Vector3)a.Swizzle("yxw"));
        AreEqualVector(Vector3(a.y, a.y, a.x), (Vector3)a.Swizzle("yyx"));
        AreEqualVector(Vector3(a.y, a.y, a.y), (Vector3)a.Swizzle("yyy"));
        AreEqualVector(Vector3(a.y, a.y, a.z), (Vector3)a.Swizzle("yyz"));
        AreEqualVector(Vector3(a.y, a.y, a.w), (Vector3)a.Swizzle("yyw"));
        AreEqualVector(Vector3(a.y, a.z, a.x), (Vector3)a.Swizzle("yzx"));
        AreEqualVector(Vector3(a.y, a.z, a.y), (Vector3)a.Swizzle("yzy"));
        AreEqualVector(Vector3(a.y, a.z, a.z), (Vector3)a.Swizzle("yzz"));
        AreEqualVector(Vector3(a.y, a.z, a.w), (Vector3)a.Swizzle("yzw"));
        AreEqualVector(Vector3(a.y, a.w, a.x), (Vector3)a.Swizzle("ywx"));
        AreEqualVector(Vector3(a.y, a.w, a.y), (Vector3)a.Swizzle("ywy"));
        AreEqualVector(Vector3(a.y, a.w, a.z), (Vector3)a.Swizzle("ywz"));
        AreEqualVector(Vector3(a.y, a.w, a.w), (Vector3)a.Swizzle("yww"));

        AreEqualVector(Vector3(a.z, a.x, a.x), (Vector3)a.Swizzle("zxx"));
        AreEqualVector(Vector3(a.z, a.x, a.y), (Vector3)a.Swizzle("zxy"));
        AreEqualVector(Vector3(a.z, a.x, a.z), (Vector3)a.Swizzle("zxz"));
        AreEqualVector(Vector3(a.z, a.x, a.w), (Vector3)a.Swizzle("zxw"));
        AreEqualVector(Vector3(a.z, a.y, a.x), (Vector3)a.Swizzle("zyx"));
        AreEqualVector(Vector3(a.z, a.y, a.y), (Vector3)a.Swizzle("zyy"));
        AreEqualVector(Vector3(a.z, a.y, a.z), (Vector3)a.Swizzle("zyz"));
        AreEqualVector(Vector3(a.z, a.y, a.w), (Vector3)a.Swizzle("zyw"));
        AreEqualVector(Vector3(a.z, a.z, a.x), (Vector3)a.Swizzle("zzx"));
        AreEqualVector(Vector3(a.z, a.z, a.y), (Vector3)a.Swizzle("zzy"));
        AreEqualVector(Vector3(a.z, a.z, a.z), (Vector3)a.Swizzle("zzz"));
        AreEqualVector(Vector3(a.z, a.z, a.w), (Vector3)a.Swizzle("zzw"));
        AreEqualVector(Vector3(a.z, a.w, a.x), (Vector3)a.Swizzle("zwx"));
        AreEqualVector(Vector3(a.z, a.w, a.y), (Vector3)a.Swizzle("zwy"));
        AreEqualVector(Vector3(a.z, a.w, a.z), (Vector3)a.Swizzle("zwz"));
        AreEqualVector(Vector3(a.z, a.w, a.w), (Vector3)a.Swizzle("zww"));

        AreEqualVector(Vector3(a.w, a.x, a.x), (Vector3)a.Swizzle("wxx"));
        AreEqualVector(Vector3(a.w, a.x, a.y), (Vector3)a.Swizzle("wxy"));
        AreEqualVector(Vector3(a.w, a.x, a.z), (Vector3)a.Swizzle("wxz"));
        AreEqualVector(Vector3(a.w, a.x, a.w), (Vector3)a.Swizzle("wxw"));
        AreEqualVector(Vector3(a.w, a.y, a.x), (Vector3)a.Swizzle("wyx"));
        AreEqualVector(Vector3(a.w, a.y, a.y), (Vector3)a.Swizzle("wyy"));
        AreEqualVector(Vector3(a.w, a.y, a.z), (Vector3)a.Swizzle("wyz"));
        AreEqualVector(Vector3(a.w, a.y, a.w), (Vector3)a.Swizzle("wyw"));
        AreEqualVector(Vector3(a.w, a.z, a.x), (Vector3)a.Swizzle("wzx"));
        AreEqualVector(Vector3(a.w, a.z, a.y), (Vector3)a.Swizzle("wzy"));
        AreEqualVector(Vector3(a.w, a.z, a.z), (Vector3)a.Swizzle("wzz"));
        AreEqualVector(Vector3(a.w, a.z, a.w), (Vector3)a.Swizzle("wzw"));
        AreEqualVector(Vector3(a.w, a.w, a.x), (Vector3)a.Swizzle("wwx"));
        AreEqualVector(Vector3(a.w, a.w, a.y), (Vector3)a.Swizzle("wwy"));
        AreEqualVector(Vector3(a.w, a.w, a.z), (Vector3)a.Swizzle("wwz"));
        AreEqualVector(Vector3(a.w, a.w, a.w), (Vector3)a.Swizzle("www"));
      }

      TEST_METHOD(Swizzling_Vector3_2)
      {
        AreEqualVector(Vector3(a.x, a.x), (Vector3)a.Swizzle("xx"));
        AreEqualVector(Vector3(a.x, a.y), (Vector3)a.Swizzle("xy"));
        AreEqualVector(Vector3(a.x, a.z), (Vector3)a.Swizzle("xz"));
        AreEqualVector(Vector3(a.x, a.w), (Vector3)a.Swizzle("xw"));
        AreEqualVector(Vector3(a.y, a.x), (Vector3)a.Swizzle("yx"));
        AreEqualVector(Vector3(a.y, a.y), (Vector3)a.Swizzle("yy"));
        AreEqualVector(Vector3(a.y, a.z), (Vector3)a.Swizzle("yz"));
        AreEqualVector(Vector3(a.y, a.w), (Vector3)a.Swizzle("yw"));
        AreEqualVector(Vector3(a.z, a.x), (Vector3)a.Swizzle("zx"));
        AreEqualVector(Vector3(a.z, a.y), (Vector3)a.Swizzle("zy"));
        AreEqualVector(Vector3(a.z, a.z), (Vector3)a.Swizzle("zz"));
        AreEqualVector(Vector3(a.z, a.w), (Vector3)a.Swizzle("zw"));
        AreEqualVector(Vector3(a.w, a.x), (Vector3)a.Swizzle("wx"));
        AreEqualVector(Vector3(a.w, a.y), (Vector3)a.Swizzle("wy"));
        AreEqualVector(Vector3(a.w, a.z), (Vector3)a.Swizzle("wz"));
        AreEqualVector(Vector3(a.w, a.w), (Vector3)a.Swizzle("ww"));
      }

      TEST_METHOD(Swizzling_Vector3_1)
      {
        AreEqualVector(Vector3(a.x), (Vector3)a.Swizzle("x"));
        AreEqualVector(Vector3(a.y), (Vector3)a.Swizzle("y"));
        AreEqualVector(Vector3(a.z), (Vector3)a.Swizzle("z"));
        AreEqualVector(Vector3(a.w), (Vector3)a.Swizzle("w"));
      }

      TEST_METHOD(Swizzling_Vector2)
      {
        AreEqualVector(Vector2(a.x, a.x), (Vector2)a.Swizzle("xx"));
        AreEqualVector(Vector2(a.x, a.y), (Vector2)a.Swizzle("xy"));
        AreEqualVector(Vector2(a.x, a.z), (Vector2)a.Swizzle("xz"));
        AreEqualVector(Vector2(a.x, a.w), (Vector2)a.Swizzle("xw"));
        AreEqualVector(Vector2(a.y, a.x), (Vector2)a.Swizzle("yx"));
        AreEqualVector(Vector2(a.y, a.y), (Vector2)a.Swizzle("yy"));
        AreEqualVector(Vector2(a.y, a.z), (Vector2)a.Swizzle("yz"));
        AreEqualVector(Vector2(a.y, a.w), (Vector2)a.Swizzle("yw"));
        AreEqualVector(Vector2(a.z, a.x), (Vector2)a.Swizzle("zx"));
        AreEqualVector(Vector2(a.z, a.y), (Vector2)a.Swizzle("zy"));
        AreEqualVector(Vector2(a.z, a.z), (Vector2)a.Swizzle("zz"));
        AreEqualVector(Vector2(a.z, a.w), (Vector2)a.Swizzle("zw"));
        AreEqualVector(Vector2(a.w, a.x), (Vector2)a.Swizzle("wx"));
        AreEqualVector(Vector2(a.w, a.y), (Vector2)a.Swizzle("wy"));
        AreEqualVector(Vector2(a.w, a.z), (Vector2)a.Swizzle("wz"));
        AreEqualVector(Vector2(a.w, a.w), (Vector2)a.Swizzle("ww"));
      }

      TEST_METHOD(Swizzling_Vector2_1)
      {
        AreEqualVector(Vector2(a.x), (Vector2)a.Swizzle("x"));
        AreEqualVector(Vector2(a.y), (Vector2)a.Swizzle("y"));
        AreEqualVector(Vector2(a.z), (Vector2)a.Swizzle("z"));
        AreEqualVector(Vector2(a.w), (Vector2)a.Swizzle("w"));
      }

      TEST_METHOD(Swizzling_Vector1)
      {
        Assert::AreEqual(Vector1(a.x), (Vector1)a.Swizzle("x"));
        Assert::AreEqual(Vector1(a.y), (Vector1)a.Swizzle("y"));
        Assert::AreEqual(Vector1(a.z), (Vector1)a.Swizzle("z"));
        Assert::AreEqual(Vector1(a.w), (Vector1)a.Swizzle("w"));
      }

    };

    TEST_CLASS(T_UnaryOperators)
    {
      Vector4 a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
        b = { 5.0f, 6.0f, 7.0f, 8.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Negation)
      {
        AreEqualVector(Vector4(-a.x, -a.y, -a.z, -a.w), -a);
      }

      TEST_METHOD(Comparison_Equality)
      {
        Assert::IsTrue(a == Vector4(1.0f, 2.0f, 3.0f, 4.0f));
        Assert::IsFalse(a == Vector4(5.0f, 6.0f, 7.0f, 8.0f));
      }

      TEST_METHOD(Comparison_Inequality)
      {
        Assert::IsFalse(a != Vector4(1.0f, 2.0f, 3.0f, 4.0f));
        Assert::IsTrue(a != Vector4(5.0f, 6.0f, 7.0f, 8.0f));
      }

      TEST_METHOD(Addition_Vector)
      {
        a += b;
        AreEqualVector(Vector4(6.0f, 8.0f, 10.0f, 12.0f), a);
      }

      TEST_METHOD(Addition_Value)
      {
        a += 4.0f;
        AreEqualVector(Vector4(5.0f, 6.0f, 7.0f, 8.0f), a);
      }

      TEST_METHOD(Subtraction_Vector)
      {
        a -= b;
        AreEqualVector(Vector4(-4.0f, -4.0f, -4.0f, -4.0f), a);
      }

      TEST_METHOD(Subtraction_Value)
      {
        a -= 4.0f;
        AreEqualVector(Vector4(-3.0f, -2.0f, -1.0f, 0.0f), a);
      }

      TEST_METHOD(Multiplication_Vector)
      {
        a *= b;
        AreEqualVector(Vector4(5.0f, 12.0f, 21.0f, 32.0f), a);
      }

      TEST_METHOD(Multiplication_Value)
      {
        a *= 4.0f;
        AreEqualVector(Vector4(4.0f, 8.0f, 12.0f, 16.0f), a);
      }

      TEST_METHOD(Division_Vec)
      {
        a /= b;
        AreEqualVector(Vector4(1.0f / 5.0f, 2.0f / 6.0f, 3.0f / 7.0f, 4.0f / 8.0f), a);
      }

      TEST_METHOD(Division_Value)
      {
        a /= 4.0f;
        AreEqualVector(Vector4(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 4.0f / 4.0f), a);
      }

      TEST_METHOD(Division_ValueZero)
      {
        a /= 0.0f;
        AreEqualVector(Vector4(1.0f, 2.0f, 3.0f, 4.0f), a);
      }

    };

    TEST_CLASS(T_BinaryOperators)
    {
      Vector4 a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
        b = { 5.0f, 6.0f, 7.0f, 8.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Addition_VectorToVector)
      {
        AreEqualVector(Vector4(6.0f, 8.0f, 10.0f, 12.0f), a + b);
      }

      TEST_METHOD(Addition_VectorToValue)
      {
        AreEqualVector(Vector4(5.0f, 6.0f, 7.0f, 8.0f), a + 4.0f);
      }

      TEST_METHOD(Addition_ValueToVector)
      {
        AreEqualVector(Vector4(5.0f, 6.0f, 7.0f, 8.0f), 4.0f + a);
      }

      TEST_METHOD(Subtraction_VectorToVector)
      {
        AreEqualVector(Vector4(-4.0f, -4.0f, -4.0f, -4.0f), a - b);
      }

      TEST_METHOD(Subtraction_VectorToValue)
      {
        AreEqualVector(Vector4(-3.0f, -2.0f, -1.0f, 0.0f), a - 4.0f);
      }

      TEST_METHOD(Subtraction_ValueToVector)
      {
        AreEqualVector(Vector4(3.0f, 2.0f, 1.0f, 0.0f), 4.0f - a);
      }

      TEST_METHOD(Multiplication_VectorToValue)
      {
        AreEqualVector(Vector4(4.0f, 8.0f, 12.0f, 16.0f), a * 4.0f);
      }

      TEST_METHOD(Multiplication_ValueToVector)
      {
        AreEqualVector(Vector4(4.0f, 8.0f, 12.0f, 16.0f), 4.0f * a);
      }

      TEST_METHOD(Division_VectorToValue)
      {
        AreEqualVector(Vector4(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 4.0f / 4.0f), a / 4.0f);
      }

      TEST_METHOD(Division_ValueToVector)
      {
        AreEqualVector(Vector4(4.0f / 1.0f, 4.0f / 2.0f, 4.0f / 3.0f, 4.0f / 4.0f), 4.0f / a);
      }

    };

    TEST_CLASS(T_Functions)
    {
      Vector4 a, b;
      glm::vec4 glm_a, glm_b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
        b = { 5.0f, 6.0f, 7.0f, 8.0f };
        glm_a = { 1.0f, 2.0f, 3.0f, 4.0f };
        glm_b = { 5.0f, 6.0f, 7.0f, 8.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Magnitude_LengthParity)
      {
        Assert::IsTrue(a.Magnitude() == a.Length());
      }

      TEST_METHOD(Magnitude_LengthSqrParity)
      {
        Assert::IsTrue(a.Length() == sqrt(a.LengthSqr()));
      }

      TEST_METHOD(Magnitude_Calculation)
      {
        Assert::AreEqual(glm::length(glm_a), a.Length());
      }

      TEST_METHOD(Normalize_StaticParity)
      {
        Vector4 my_normalize_static = Vector4::Normalize(a);
        Vector4 my_normalize = a.Normalize();
        Assert::IsTrue(my_normalize == my_normalize_static);
      }

      TEST_METHOD(Normalize_LengthCheck)
      {
        Assert::AreEqual(1.0f, Vector4::Normalize(a).Length(), EPSILONf);
      }

      TEST_METHOD(Normalize_Calculation)
      {
        AreEqualVector(glm::normalize(glm_a), Vector4::Normalize(a), EPSILONf);
      }

      TEST_METHOD(LinearInterpolation)
      {
        AreEqualVector(glm::mix(glm_a, glm_b, 0.5f), Lerp(a, b, 0.5f));
      }

    };

    TEST_CLASS(T_ConversionOverloads)
    {
      Vector4 a;
      glm::vec4 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
        glm_a = { 1.0f, 2.0f, 3.0f, 4.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(DegreeToRadians)
      {
        AreEqualVector(glm::radians(glm_a), radians(a));
      }

      TEST_METHOD(RadiansToDegrees)
      {
        AreEqualVector(glm::degrees(glm_a), degrees(a));
      }

    };

  }

  #pragma endregion

  #pragma region Matrix4x4

  namespace T_Matrix4x4
  {

    TEST_CLASS(T_Accessors)
    {
      Matrix4x4 a;
      glm::mat4 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
        glm_a = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Individual)
      {
        Assert::AreEqual(1.0f, a.m00);
        Assert::AreEqual(2.0f, a.m01);
        Assert::AreEqual(3.0f, a.m02);
        Assert::AreEqual(4.0f, a.m03);
        Assert::AreEqual(5.0f, a.m10);
        Assert::AreEqual(6.0f, a.m11);
        Assert::AreEqual(7.0f, a.m12);
        Assert::AreEqual(8.0f, a.m13);
        Assert::AreEqual(9.0f, a.m20);
        Assert::AreEqual(10.0f, a.m21);
        Assert::AreEqual(11.0f, a.m22);
        Assert::AreEqual(12.0f, a.m23);
        Assert::AreEqual(13.0f, a.m30);
        Assert::AreEqual(14.0f, a.m31);
        Assert::AreEqual(15.0f, a.m32);
        Assert::AreEqual(16.0f, a.m33);
      }

      TEST_METHOD(Array)
      {
        Assert::AreEqual(1.0f, a.data[0]);
        Assert::AreEqual(2.0f, a.data[1]);
        Assert::AreEqual(3.0f, a.data[2]);
        Assert::AreEqual(4.0f, a.data[3]);
        Assert::AreEqual(5.0f, a.data[4]);
        Assert::AreEqual(6.0f, a.data[5]);
        Assert::AreEqual(7.0f, a.data[6]);
        Assert::AreEqual(8.0f, a.data[7]);
        Assert::AreEqual(9.0f, a.data[8]);
        Assert::AreEqual(10.0f, a.data[9]);
        Assert::AreEqual(11.0f, a.data[10]);
        Assert::AreEqual(12.0f, a.data[11]);
        Assert::AreEqual(13.0f, a.data[12]);
        Assert::AreEqual(14.0f, a.data[13]);
        Assert::AreEqual(15.0f, a.data[14]);
        Assert::AreEqual(16.0f, a.data[15]);
      }

      TEST_METHOD(Function)
      {
        Assert::AreEqual(1.0f, a(0, 0));
        Assert::AreEqual(2.0f, a(0, 1));
        Assert::AreEqual(3.0f, a(0, 2));
        Assert::AreEqual(4.0f, a(0, 3));
        Assert::AreEqual(5.0f, a(1, 0));
        Assert::AreEqual(6.0f, a(1, 1));
        Assert::AreEqual(7.0f, a(1, 2));
        Assert::AreEqual(8.0f, a(1, 3));
        Assert::AreEqual(9.0f, a(2, 0));
        Assert::AreEqual(10.0f, a(2, 1));
        Assert::AreEqual(11.0f, a(2, 2));
        Assert::AreEqual(12.0f, a(2, 3));
        Assert::AreEqual(13.0f, a(3, 0));
        Assert::AreEqual(14.0f, a(3, 1));
        Assert::AreEqual(15.0f, a(3, 2));
        Assert::AreEqual(16.0f, a(3, 3));
      }

      TEST_METHOD(MemberParity)
      {
        Assert::AreEqual(a.m00, a(0, 0));
        Assert::AreEqual(a.m01, a(0, 1));
        Assert::AreEqual(a.m02, a(0, 2));
        Assert::AreEqual(a.m03, a(0, 3));
        Assert::AreEqual(a.m10, a(1, 0));
        Assert::AreEqual(a.m11, a(1, 1));
        Assert::AreEqual(a.m12, a(1, 2));
        Assert::AreEqual(a.m13, a(1, 3));
        Assert::AreEqual(a.m20, a(2, 0));
        Assert::AreEqual(a.m21, a(2, 1));
        Assert::AreEqual(a.m22, a(2, 2));
        Assert::AreEqual(a.m23, a(2, 3));
        Assert::AreEqual(a.m30, a(3, 0));
        Assert::AreEqual(a.m31, a(3, 1));
        Assert::AreEqual(a.m32, a(3, 2));
        Assert::AreEqual(a.m33, a(3, 3));
      }

      TEST_METHOD(ParityWithGLM)
      {
        for (int i = 0; i < 4; i++)
        {
          for (int j = 0; j < 4; j++)
          {
            Assert::AreEqual(glm_a[i][j], a(i, j));
          }
        }
      }

    };

    TEST_CLASS(T_UnaryOperators)
    {
      Matrix4x4 a, b, c;
      glm::mat4 glm_a, glm_b, glm_c;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
        b = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
        c = {
           2.0f,  4.0f,  6.0f,  8.0f,
          18.0f, 20.0f, 22.0f, 24.0f,
          10.0f, 12.0f, 14.0f, 16.0f,
          26.0f, 28.0f, 30.0f, 32.0f
        };
        glm_a = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
        glm_b = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
        glm_c = {
           2.0f,  4.0f,  6.0f,  8.0f,
          18.0f, 20.0f, 22.0f, 24.0f,
          10.0f, 12.0f, 14.0f, 16.0f,
          26.0f, 28.0f, 30.0f, 32.0f
        };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Comparison_Equality)
      {
        Assert::IsTrue(a == b);
        Assert::IsFalse(a == c);
      }

      TEST_METHOD(Comparison_Inequality)
      {
        Assert::IsFalse(a != b);
        Assert::IsTrue(a != c);
      }

      TEST_METHOD(Addition_Matrix)
      {
        a += c;
        glm_a += glm_c;
        AreEqualMatrix(glm_a, a);
      }

      TEST_METHOD(Subtraction_Matrix)
      {
        a -= c;
        glm_a -= glm_c;
        AreEqualMatrix(glm_a, a);
      }

      TEST_METHOD(Multiplication_Matrix)
      {
        a *= c;
        glm_a *= glm_c;
        AreEqualMatrix(glm_a, a);
      }

      TEST_METHOD(Multiplication_Value)
      {
        a *= 2.0f;
        glm_a *= 2.0f;
        AreEqualMatrix(glm_a, a);
      }

      TEST_METHOD(Division_Value)
      {
        a /= 2.0f;
        glm_a /= 2.0f;
        AreEqualMatrix(glm_a, a);
      }

    };

    TEST_CLASS(T_BinaryOperators)
    {
      Matrix4x4 a, b;
      glm::mat4 glm_a, glm_b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
        b = {
           2.0f,  4.0f,  6.0f,  8.0f,
          18.0f, 20.0f, 22.0f, 24.0f,
          10.0f, 12.0f, 14.0f, 16.0f,
          26.0f, 28.0f, 30.0f, 32.0f
        };
        glm_a = {
           1.0f,  2.0f,  3.0f,  4.0f,
           5.0f,  6.0f,  7.0f,  8.0f,
           9.0f, 10.0f, 11.0f, 12.0f,
          13.0f, 14.0f, 15.0f, 16.0f
        };
        glm_b = {
           2.0f,  4.0f,  6.0f,  8.0f,
          18.0f, 20.0f, 22.0f, 24.0f,
          10.0f, 12.0f, 14.0f, 16.0f,
          26.0f, 28.0f, 30.0f, 32.0f
        };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Addition_MatrixToMatrix)
      {
        AreEqualMatrix(glm_a + glm_b, a + b);
      }

      TEST_METHOD(Subtraction_MatrixToMatrix)
      {
        AreEqualMatrix(glm_a - glm_b, a - b);
      }

      TEST_METHOD(Multiplication_MatrixToMatrix)
      {
        AreEqualMatrix(glm_a * glm_b, a * b);
      }

      TEST_METHOD(Multiplication_MatrixToValue)
      {
        AreEqualMatrix(glm_a * 2.0f, a * 2.0f);
      }

      TEST_METHOD(Multiplication_ValueToMatrix)
      {
        AreEqualMatrix(2.0f * glm_a, 2.0f * a);
      }

      TEST_METHOD(Division_MatrixToValue)
      {
        AreEqualMatrix(glm_a / 2.0f, a / 2.0f);
      }

    };

    TEST_CLASS(T_Functions)
    {
      Matrix4x4 a;
      glm::mat4 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = {
          5.0f, 6.0f, 6.0f, 8.0f,
          2.0f, 2.0f, 2.0f, 8.0f,
          6.0f, 6.0f, 2.0f, 8.0f,
          2.0f, 3.0f, 6.0f, 7.0f
        };
        glm_a = {
          5.0f, 6.0f, 6.0f, 8.0f,
          2.0f, 2.0f, 2.0f, 8.0f,
          6.0f, 6.0f, 2.0f, 8.0f,
          2.0f, 3.0f, 6.0f, 7.0f
        };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Transpose)
      {
        AreEqualMatrix(glm::transpose(glm_a), a.Transpose());
      }

      TEST_METHOD(Determinant)
      {
        Assert::AreEqual(glm::determinant(glm_a), a.Determinant());
      }

      TEST_METHOD(Inverse)
      {
        AreEqualMatrix(glm::inverse(glm_a), a.Inverse());
      }

    };

    TEST_CLASS(T_Transformations_StaticParity)
    {
      Matrix4x4 a;
      glm::mat4 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = Matrix4x4::Identity;
        glm_a = glm::mat4(1.0f);
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Static_Translate)
      {
        Matrix4x4 static_translate = Matrix4x4::Translate(a, Vector3(1.0f, 2.0f, 3.0f));
        Assert::IsTrue(a.Translate(Vector3(1.0f, 2.0f, 3.0f)) == static_translate);
      }

      TEST_METHOD(Static_Rotate)
      {
        Matrix4x4 static_rotate = Matrix4x4::Rotate(a, radians(45.0f), Vector3(0.3f, 0.5f, 0.7f));
        Assert::IsTrue(a.Rotate(radians(45.0f), Vector3(0.3f, 0.5f, 0.7f)) == static_rotate);
      }

      TEST_METHOD(Static_RotateDeg)
      {
        Matrix4x4 static_rotate_deg = Matrix4x4::RotateDeg(a, 45.0f, Vector3(0.3f, 0.5f, 0.7f));
        Assert::IsTrue(a.RotateDeg(45.0f, Vector3(0.3f, 0.5f, 0.7f)) == static_rotate_deg);
      }

      TEST_METHOD(Static_RotateX)
      {
        Matrix4x4 static_rotate_x = Matrix4x4::RotateX(a, radians(45.0f));
        Assert::IsTrue(a.RotateX(radians(45.0f)) == static_rotate_x);
      }

      TEST_METHOD(Static_RotateY)
      {
        Matrix4x4 static_rotate_y = Matrix4x4::RotateY(a, radians(45.0f));
        Assert::IsTrue(a.RotateY(radians(45.0f)) == static_rotate_y);
      }

      TEST_METHOD(Static_RotateZ)
      {
        Matrix4x4 static_rotate_z = Matrix4x4::RotateZ(a, radians(45.0f));
        Assert::IsTrue(a.RotateZ(radians(45.0f)) == static_rotate_z);
      }

      TEST_METHOD(Static_RotateXDeg)
      {
        Matrix4x4 static_rotate_x_deg = Matrix4x4::RotateXDeg(a, 45.0f);
        Assert::IsTrue(a.RotateXDeg(45.0f) == static_rotate_x_deg);
      }

      TEST_METHOD(Static_RotateYDeg)
      {
        Matrix4x4 static_rotate_y_deg = Matrix4x4::RotateYDeg(a, 45.0f);
        Assert::IsTrue(a.RotateYDeg(45.0f) == static_rotate_y_deg);
      }

      TEST_METHOD(Static_RotateZDeg)
      {
        Matrix4x4 static_rotate_z_deg = Matrix4x4::RotateZDeg(a, 45.0f);
        Assert::IsTrue(a.RotateZDeg(45.0f) == static_rotate_z_deg);
      }

      TEST_METHOD(Static_Scale)
      {
        Matrix4x4 static_scale = Matrix4x4::Scale(a, Vector3(1.0f, 2.0f, 3.0f));
        Assert::IsTrue(a.Scale(Vector3(1.0f, 2.0f, 3.0f)) == static_scale);
      }

    };

    TEST_CLASS(T_Transformations_Common)
    {
      Matrix4x4 a;
      glm::mat4 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = Matrix4x4::Identity;
        glm_a = glm::mat4(1.0f);
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Common_Translate)
      {
        glm_a = glm::translate(glm_a, glm::vec3(10.0f, -20.0f, 30.0f));
        AreEqualMatrix(glm_a, a.Translate(Vector3(10.0f, -20.0f, 30.0f)));
      }

      TEST_METHOD(Common_Rotate)
      {
        glm_a = glm::rotate(glm_a, glm::radians(45.0f), glm::vec3(0.3f, 0.5f, 0.7f));
        AreEqualMatrix(glm_a, a.Rotate(radians(45.0f), Vector3(0.3f, 0.5f, 0.7f)));
      }

      TEST_METHOD(Common_RotateDeg)
      {
        glm_a = glm::rotate(glm_a, glm::radians(45.0f), glm::vec3(0.3f, 0.5f, 0.7f));
        AreEqualMatrix(glm_a, a.RotateDeg(45.0f, Vector3(0.3f, 0.5f, 0.7f)));
      }

      TEST_METHOD(Common_RotateX)
      {
        glm_a = glm::rotate(glm_a, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        AreEqualMatrix(glm_a, a.RotateX(radians(35.0f)));
      }

      TEST_METHOD(Common_RotateY)
      {
        glm_a = glm::rotate(glm_a, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        AreEqualMatrix(glm_a, a.RotateY(radians(45.0f)));
      }

      TEST_METHOD(Common_RotateZ)
      {
        glm_a = glm::rotate(glm_a, glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        AreEqualMatrix(glm_a, a.RotateZ(radians(55.0f)));
      }

      TEST_METHOD(Common_RotateXDeg)
      {
        glm_a = glm::rotate(glm_a, glm::radians(35.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        AreEqualMatrix(glm_a, a.RotateXDeg(35.0f));
      }

      TEST_METHOD(Common_RotateYDeg)
      {
        glm_a = glm::rotate(glm_a, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        AreEqualMatrix(glm_a, a.RotateYDeg(45.0f));
      }

      TEST_METHOD(Common_RotateZDeg)
      {
        glm_a = glm::rotate(glm_a, glm::radians(55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        AreEqualMatrix(glm_a, a.RotateZDeg(55.0f));
      }

      TEST_METHOD(Common_Scale)
      {
        glm_a = glm::scale(glm_a, glm::vec3(10.0f, 20.0f, -30.0f));
        AreEqualMatrix(glm_a, a.Scale(Vector3(10.0f, 20.0f, -30.0f)));
      }

    };

    TEST_CLASS(T_Transformations_Others)
    {
      Matrix4x4 a;
      glm::mat4 glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = Matrix4x4::Identity;
        glm_a = glm::mat4(1.0f);
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Others_LookAt)
      {
        glm_a = glm::lookAt(glm::vec3(1.0f, 2.0f, 3.0f), glm::vec3(4.0f, 5.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        a = Matrix4x4::LookAt(Vector3(1.0f, 2.0f, 3.0f), Vector3(4.0f, 5.0f, 6.0f), Vector3(0.0f, 1.0f, 0.0f));
        AreEqualMatrix(glm_a, a, 1e-06f);
#pragma message("Developer's Note: The LookAt test is not to EPSILONf precision but 1e-06f is close enough.")
      }

      TEST_METHOD(Others_Perspective)
      {
        glm_a = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        a = Matrix4x4::Perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        AreEqualMatrix(glm_a, a);
      }

      TEST_METHOD(Others_Orthographic)
      {
        glm_a = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        a = Matrix4x4::Orthographic(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
        AreEqualMatrix(glm_a, a);
      }

    };

  }

  #pragma endregion

  #pragma region Quaternion

  namespace T_Quaternion
  {

    TEST_CLASS(T_Constructors)
    {
    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(DefaultConstructor)
      {
        Quaternion a;
        Assert::AreEqual(0.0f, a.x);
        Assert::AreEqual(0.0f, a.y);
        Assert::AreEqual(0.0f, a.z);
        Assert::AreEqual(0.0f, a.w);
      }

      TEST_METHOD(ValueConstructor)
      {
        Quaternion a = Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
        Assert::AreEqual(3.0f, a.z);
        Assert::AreEqual(4.0f, a.w);
      }

      TEST_METHOD(CopyConstructor)
      {
        Quaternion a = Quaternion(1.0f, 2.0f, 3.0f, 4.0f);
        Quaternion b = Quaternion(a);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
        Assert::AreEqual(3.0f, b.z);
        Assert::AreEqual(4.0f, b.w);
      }

      TEST_METHOD(Vector4Constructor)
      {
        Vector4 a = Vector4(1.0f, 2.0f, 3.0f, 4.0f);
        Quaternion b = Quaternion(a);
        Assert::AreEqual(1.0f, b.x);
        Assert::AreEqual(2.0f, b.y);
        Assert::AreEqual(3.0f, b.z);
        Assert::AreEqual(4.0f, b.w);
      }

    };

    TEST_CLASS(T_Accessors)
    {
      Quaternion a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(XYZW)
      {
        Assert::AreEqual(1.0f, a.x);
        Assert::AreEqual(2.0f, a.y);
        Assert::AreEqual(3.0f, a.z);
        Assert::AreEqual(4.0f, a.w);
      }

      TEST_METHOD(Array)
      {
        Assert::AreEqual(1.0f, a[0]);
        Assert::AreEqual(2.0f, a[1]);
        Assert::AreEqual(3.0f, a[2]);
        Assert::AreEqual(4.0f, a[3]);
      }

      TEST_METHOD(DataArray)
      {
        Assert::AreEqual(1.0f, a.data[0]);
        Assert::AreEqual(2.0f, a.data[1]);
        Assert::AreEqual(3.0f, a.data[2]);
        Assert::AreEqual(4.0f, a.data[3]);
      }

    };

    TEST_CLASS(T_ConversionOperators)
    {
      Quaternion a;
      glm::quat glm_a;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        // Rotation 1 radian, axis (0.4, 0.5, 0.6)
        a = { 0.2185424f, 0.2731781f, 0.3278137f, 0.8775826f };
        glm_a = { 0.8775826f, 0.2185424f, 0.2731781f, 0.3278137f }; // wxyz
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(ToVector3)
      {
        // Converts to euler angles
        glm::vec3 glm_euler = glm::eulerAngles(glm_a);
        AreEqualVector(glm_euler, (Vector3)a);
      }

      TEST_METHOD(ToMatrix4x4)
      {
        // Convert to rotation matrix
        glm::mat4 glm_rot = glm::mat4_cast(glm_a);
        AreEqualMatrix(glm_rot, (Matrix4x4)a);
      }

      TEST_METHOD(ToBool)
      {
        Assert::IsTrue((bool)a);
      }

      TEST_METHOD(ToBool_EmptyQuaternion)
      {
        Assert::IsFalse((bool)Quaternion(0, 0, 0, 0));
      }

    };

    TEST_CLASS(T_UnaryOperators)
    {
      Quaternion a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
        b = { 5.0f, 6.0f, 7.0f, 8.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Negation)
      {
        AreEqualQuaternion(Quaternion(-a.x, -a.y, -a.z, -a.w), -a);
      }

      TEST_METHOD(Comparison_Equality)
      {
        Assert::IsTrue(a == Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
        Assert::IsFalse(a == Quaternion(5.0f, 6.0f, 7.0f, 8.0f));
      }

      TEST_METHOD(Comparison_Inequality)
      {
        Assert::IsFalse(a != Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
        Assert::IsTrue(a != Quaternion(5.0f, 6.0f, 7.0f, 8.0f));
      }

      TEST_METHOD(Addition_Quaternion)
      {
        a += b;
        AreEqualQuaternion(Quaternion(6.0f, 8.0f, 10.0f, 12.0f), a);
      }

      TEST_METHOD(Addition_Value)
      {
        a += 4.0f;
        AreEqualQuaternion(Quaternion(5.0f, 6.0f, 7.0f, 8.0f), a);
      }

      TEST_METHOD(Subtraction_Quaternion)
      {
        a -= b;
        AreEqualQuaternion(Quaternion(-4.0f, -4.0f, -4.0f, -4.0f), a);
      }

      TEST_METHOD(Subtraction_Value)
      {
        a -= 4.0f;
        AreEqualQuaternion(Quaternion(-3.0f, -2.0f, -1.0f, 0.0f), a);
      }

      TEST_METHOD(Multiplication_Quaternion)
      {
        a *= b;
        AreEqualQuaternion(Quaternion(5.0f, 12.0f, 21.0f, 32.0f), a);
      }

      TEST_METHOD(Multiplication_Value)
      {
        a *= 4.0f;
        AreEqualQuaternion(Quaternion(4.0f, 8.0f, 12.0f, 16.0f), a);
      }

      TEST_METHOD(Division_Quaternion)
      {
        a /= b;
        AreEqualQuaternion(Quaternion(1.0f / 5.0f, 2.0f / 6.0f, 3.0f / 7.0f, 4.0f / 8.0f), a);
      }

      TEST_METHOD(Division_Value)
      {
        a /= 4.0f;
        AreEqualQuaternion(Quaternion(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 4.0f / 4.0f), a);
      }

      TEST_METHOD(Division_ValueZero)
      {
        a /= 0.0f;
        AreEqualQuaternion(Quaternion(1.0f, 2.0f, 3.0f, 4.0f), a);
      }

    };

    TEST_CLASS(T_BinaryOperators)
    {
      Quaternion a, b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        a = { 1.0f, 2.0f, 3.0f, 4.0f };
        b = { 5.0f, 6.0f, 7.0f, 8.0f };
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(Addition_QuaternionToQuaternion)
      {
        AreEqualQuaternion(Quaternion(6.0f, 8.0f, 10.0f, 12.0f), a + b);
      }

      TEST_METHOD(Addition_QuaternionToValue)
      {
        AreEqualQuaternion(Quaternion(5.0f, 6.0f, 7.0f, 8.0f), a + 4.0f);
      }

      TEST_METHOD(Addition_ValueToQuaternion)
      {
        AreEqualQuaternion(Quaternion(5.0f, 6.0f, 7.0f, 8.0f), 4.0f + a);
      }

      TEST_METHOD(Subtraction_QuaternionToQuaternion)
      {
        AreEqualQuaternion(Quaternion(-4.0f, -4.0f, -4.0f, -4.0f), a - b);
      }

      TEST_METHOD(Subtraction_QuaternionToValue)
      {
        AreEqualQuaternion(Quaternion(-3.0f, -2.0f, -1.0f, 0.0f), a - 4.0f);
      }

      TEST_METHOD(Subtraction_ValueToQuaternion)
      {
        AreEqualQuaternion(Quaternion(3.0f, 2.0f, 1.0f, 0.0f), 4.0f - a);
      }

      TEST_METHOD(Multiplication_QuaternionToValue)
      {
        AreEqualQuaternion(Quaternion(4.0f, 8.0f, 12.0f, 16.0f), a * 4.0f);
      }

      TEST_METHOD(Multiplication_ValueToQuaternion)
      {
        AreEqualQuaternion(Quaternion(4.0f, 8.0f, 12.0f, 16.0f), 4.0f * a);
      }

      TEST_METHOD(Division_QuaternionToValue)
      {
        AreEqualQuaternion(Quaternion(1.0f / 4.0f, 2.0f / 4.0f, 3.0f / 4.0f, 4.0f / 4.0f), a / 4.0f);
      }

      TEST_METHOD(Division_ValueToQuaternion)
      {
        AreEqualQuaternion(Quaternion(4.0f / 1.0f, 4.0f / 2.0f, 4.0f / 3.0f, 4.0f / 4.0f), 4.0f / a);
      }

    };

    TEST_CLASS(T_Functions)
    {
      Quaternion a, b;
      glm::quat glm_a, glm_b;

    public:

      TEST_METHOD_INITIALIZE(Initialize)
      {
        // Rotation 1 radian, axis (0.4, 0.5, 0.6)
        a = { 0.2185424f, 0.2731781f, 0.3278137f, 0.8775826f };
        glm_a = { 0.8775826f, 0.2185424f, 0.2731781f, 0.3278137f }; // wxyz

        // Rotation 2 radians, axis (0.6, 0.7, 0.8)
        b = { 0.4136159f, 0.4825519f, 0.5514879f, 0.5403023f };
        glm_b = { 0.5403023f, 0.4136159f, 0.4825519f, 0.5514879f }; // wxyz
      }

      TEST_METHOD_CLEANUP(Cleanup)
      {
      }

      TEST_METHOD(EulerAngles)
      {
        // Converts to euler angles
        Vector3 a_euler = a.ToEulerAngles();
        glm::vec3 glm_euler = glm::eulerAngles(glm_a);
        AreEqualVector(glm_euler, a_euler);
      }

      TEST_METHOD(Magnitude_LengthParity)
      {
        Assert::IsTrue(a.Magnitude() == a.Length());
      }

      TEST_METHOD(Magnitude_LengthSqrParity)
      {
        Assert::IsTrue(a.Length() == sqrt(a.LengthSqr()));
      }

      TEST_METHOD(Magnitude_Calculation)
      {
        Assert::AreEqual(glm::length(glm_a), a.Length());
      }

      TEST_METHOD(Normalize_StaticParity)
      {
        Quaternion my_normalize_static = Quaternion::Normalize(a);
        Quaternion my_normalize = a.Normalize();
        Assert::IsTrue(my_normalize == my_normalize_static);
      }

      TEST_METHOD(Normalize_LengthCheck)
      {
        Assert::AreEqual(1.0f, Quaternion::Normalize(a).Length(), EPSILONf);
      }

      TEST_METHOD(Normalize_Calculation)
      {
        AreEqualQuaternion(glm::normalize(glm_a), Quaternion::Normalize(a), EPSILONf);
      }

      TEST_METHOD(LinearInterpolation)
      {
        AreEqualQuaternion(glm::mix(glm_a, glm_b, 0.5f), Lerp(a, b, 0.5f), EPSILONf);
      }

      TEST_METHOD(DotProduct)
      {
        Assert::AreEqual(glm::dot(glm_a, glm_b), Dot(a, b));
      }

      TEST_METHOD(CrossProduct)
      {
        AreEqualQuaternion(glm::cross(glm_a, glm_b), Cross(a, b), EPSILONf);
      }

    };

  }

  #pragma endregion

}

namespace T_FlexID
{

  TEST_CLASS(T_Create)
  {
    FLX_ID_SETUP;
  public:

    TEST_METHOD_INITIALIZE(Initialize)
    {
      _flx_id_next = 1;
      _flx_id_unused.clear();
    }

    TEST_METHOD_CLEANUP(Cleanup)
    {
    }

    TEST_METHOD(T_Create_None)
    {
      uint64_t id = ID::Create(ID::Flag_None, _flx_id_next, _flx_id_unused);
      // 0000   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)4294967297, id);
    }

    TEST_METHOD(T_Create_Next)
    {
      uint64_t id = ID::Create(ID::Flag_None, _flx_id_next, _flx_id_unused);
      Assert::AreEqual((size_t)2, _flx_id_next);
    }

    TEST_METHOD(T_Create_Increment)
    {
      uint64_t id1 = ID::Create(ID::Flag_None, _flx_id_next, _flx_id_unused);
      uint64_t id2 = ID::Create(ID::Flag_None, _flx_id_next, _flx_id_unused);
      // 0000   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0010
      Assert::AreEqual((uint64_t)4294967298, id2);
    }

    TEST_METHOD(T_Create_Parity)
    {
      uint64_t id1 = ID::Create(ID::Flag_None, _flx_id_next, _flx_id_unused);
      _flx_id_next--;
      uint64_t id2 = ID::Create(false, false, false, false, _flx_id_next, _flx_id_unused);
      Assert::AreEqual(id1, id2);
    }

    TEST_METHOD(T_Create_Flag1)
    {
      uint64_t id = ID::Create(true, false, false, false, _flx_id_next, _flx_id_unused);
      // 0001   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)1152921508901814273, id);
    }

    TEST_METHOD(T_Create_Flag2)
    {
      uint64_t id = ID::Create(false, true, false, false, _flx_id_next, _flx_id_unused);
      // 0010   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)2305843013508661249, id);
    }

    TEST_METHOD(T_Create_Flag3)
    {
      uint64_t id = ID::Create(false, false, true, false, _flx_id_next, _flx_id_unused);
      // 0100   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)4611686022722355201, id);
    }

    TEST_METHOD(T_Create_Flag4)
    {
      uint64_t id = ID::Create(false, false, false, true, _flx_id_next, _flx_id_unused);
      // 1000   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)9223372041149743105, id);
    }

    TEST_METHOD(T_Create_Flags)
    {
      uint64_t id;
      id = ID::Create(false, false, true, true, _flx_id_next, _flx_id_unused);
      // 1100   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)13835058059577131009, id);

      id = ID::Create(false, true, false, true, _flx_id_next, _flx_id_unused);
      // 1010   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0010
      Assert::AreEqual((uint64_t)11529215050363437058, id);

      id = ID::Create(true, false, true, false, _flx_id_next, _flx_id_unused);
      // 0101   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0011
      Assert::AreEqual((uint64_t)5764607527329202179, id);

      id = ID::Create(true, true, true, true, _flx_id_next, _flx_id_unused);
      // 1111   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0100
      Assert::AreEqual((uint64_t)17293822573397671940, id);
    }

  };
  
  TEST_CLASS(T_Get)
  {
    // 0110   0000 0000 0000 0000 0000 0000 0011   0000 0000 0000 0000 0000 0000 0000 0101
    uint64_t id = 6917529040525983749U;
  public:

    TEST_METHOD_INITIALIZE(Initialize)
    {
      // 0110   0000 0000 0000 0000 0000 0000 0011   0000 0000 0000 0000 0000 0000 0000 0101
      id = 6917529040525983749U;
    }

    TEST_METHOD_CLEANUP(Cleanup)
    {
    }

    TEST_METHOD(T_Get_ID)
    {
      Assert::AreEqual((uint32_t)5, ID::GetID(id));
    }

    TEST_METHOD(T_Get_Generation)
    {
      Assert::AreEqual((uint32_t)3, ID::GetGeneration(id));
    }

    TEST_METHOD(T_Get_Flag)
    {
      Assert::AreEqual((uint8_t)6, ID::GetFlags(id));
    }

    TEST_METHOD(T_Get_Flag_Individual)
    {
      Assert::IsFalse(ID::_FLX_ECS_ID_FLAG_ONE   (id));
      Assert::IsTrue (ID::_FLX_ECS_ID_FLAG_TWO   (id));
      Assert::IsTrue (ID::_FLX_ECS_ID_FLAG_THREE (id));
      Assert::IsFalse(ID::_FLX_ECS_ID_FLAG_FOUR  (id));
    }

    TEST_METHOD(T_Decompose)
    {
      uint32_t id_out;
      uint32_t generation_out;
      uint8_t flags_out;
      ID::Decompose(id, &id_out, &generation_out, &flags_out);

      Assert::AreEqual((uint32_t)5, id_out);
      Assert::AreEqual((uint32_t)3, generation_out);
      Assert::AreEqual((uint8_t)6, flags_out);
    }

  };

  TEST_CLASS(T_Destroy)
  {
    FLX_ID_SETUP;
  public:

    TEST_METHOD_INITIALIZE(Initialize)
    {
      _flx_id_next = 1;
      _flx_id_unused.clear();
    }

    TEST_METHOD_CLEANUP(Cleanup)
    {
    }

    TEST_METHOD(T_Destroy_None)
    {
      uint64_t id = ID::Create(ID::Flag_None, _flx_id_next, _flx_id_unused);
      ID::Destroy(id, _flx_id_unused);

      Assert::AreEqual((size_t)2, _flx_id_next);
      Assert::AreEqual((size_t)1, _flx_id_unused.size());
      // 0000   0000 0000 0000 0000 0000 0000 0001   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)4294967297, _flx_id_unused.back());
    }

    TEST_METHOD(T_Destroy_Reuse)
    {
      uint64_t id = ID::Create(2, _flx_id_next, _flx_id_unused);
      ID::Destroy(id, _flx_id_unused);
      id = ID::Create(false, false, true, false, _flx_id_next, _flx_id_unused);

      Assert::AreEqual((size_t)2, _flx_id_next);
      Assert::AreEqual((size_t)0, _flx_id_unused.size());
      // 0100   0000 0000 0000 0000 0000 0000 0010   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)4611686027017322497, id);

      ID::Destroy(id, _flx_id_unused);
      id = ID::Create(false, false, true, false, _flx_id_next, _flx_id_unused);

      Assert::AreEqual((size_t)2, _flx_id_next);
      Assert::AreEqual((size_t)0, _flx_id_unused.size());
      // 0100   0000 0000 0000 0000 0000 0000 0011   0000 0000 0000 0000 0000 0000 0000 0001
      Assert::AreEqual((uint64_t)4611686031312289793, id);
    }

  };

}