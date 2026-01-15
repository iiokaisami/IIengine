#pragma once

#include "Vector2.h"

class Matrix4x4;

/// <summary>
/// 3th Dimension Vector
/// 3次元ベクトル
/// </summary>
class Vector3 final {
public:
    float x;
    float y;
    float z;

    inline Vector3() : x(), y(), z() {};

    inline Vector3(const float& _x, const float& _y, const float& _z)
    {
        x = _x;
        y = _y;
        z = _z;
        return;
    }

    inline Vector3(const float& _x, const float& _y)
    {
        x = _x;
        y = _y;
        z = 0.0f;
        return;
    }

    inline Vector3(const float& _x)
    {
        x = _x;
        y = 0.0f;
        z = 0.0f;
        return;
    }

    inline Vector3(const Vector2& _vec2, float _f)
    {
        x = _vec2.x;
        y = _vec2.y;
        z = _f;
        return;
    }

    inline Vector3(const Vector2& _vec2)
    {
        x = _vec2.x;
        y = _vec2.y;
        z = 0.0f;
        return;
    }

    inline Vector2 xy() const { return Vector2(x, y); }

    /// ===========
    /// calculation
    /// ===========

	/// <summary>
	/// 内積
	/// </summary>
	/// <param name="_v">ベクトル</param>
	/// <returns>内積</returns>
    float Dot(const Vector3& _v) const;
    
	/// <summary>
	/// 外積
	/// </summary>
	/// <param name="_v">ベクトル</param>
	/// <returns>外積</returns>
    Vector3 Cross(const Vector3& _v) const;
    
	/// <summary>
	/// 原点からの距離を求める
	/// </summary>
	/// <returns>距離</returns>
    float Length() const;
    
	/// <summary>
	/// Length関数の平方根なし
	/// </summary>
	/// <returns>距離の二乗</returns>
    float LengthWithoutRoot() const;
    
	/// <summary>
	/// 正規化済みの値を返す
	/// </summary>
	/// <returns>正規化ベクトル</returns>
    Vector3 Normalize() const;
    
	/// <summary>
	/// thisから引数のベクトルまでの距離
	/// </summary>
	/// <param name="_destination">行き先</param>
	/// <returns>距離</returns>
    float Distance(const Vector3& _destination) const;
    
	/// <summary>
	/// 軸aに投影した値を返す
	/// </summary>
	/// <param name="_a">軸</param>
	/// <returns>投影値</returns>
    float Projection(const Vector3& _a) const;
    
	/// <summary>
	/// 線形補間
	/// </summary>
	/// <param name="_begin">初期値</param>
	/// <param name="_end">終了値</param>
	/// <param name="_t">0.0~1.0</param>
    void Lerp(const Vector3& _begin, const Vector3& _end, float _t);
    
	/// <summary>
	/// 原点からの角度を求める
	/// </summary>
	/// <param name="_azimuth">方位角(ラジアン)</param>
	/// <param name="_elevation">仰角(ラジアン)</param>
	/// <param name="_origin">原点</param>
	/// <returns>角度(ラジアン)</returns>
    void Theta(float& _azimuth, float& _elevation, const Vector3& _origin = { 0.0f, 0.0f }) const;

    /// ==========
    /// minus sign
    /// ==========

    Vector3 operator-() const;

    /// =======
    /// Vector3
    /// =======

    Vector3& operator+=(const Vector3& _rv);
    Vector3& operator-=(const Vector3& _rv);
    Vector3& operator*=(const Vector3& _rv);

    Vector3 operator+(const Vector3& _v) const;
    Vector3 operator-(const Vector3& _v) const;
    Vector3 operator*(const Vector3& _v) const;

    /// =====
    /// float
    /// =====

    Vector3 operator+(float) = delete;
    Vector3 operator-(float) = delete;
    Vector3 operator*(float _f) const;
    Vector3 operator/(float _f) const;
    Vector3& operator+=(float) = delete;
    Vector3& operator-=(float) = delete;
    Vector3& operator*=(float _f);
    Vector3& operator/=(float _f);

    /// =======
    /// Vector2
    /// =======

    Vector3& operator+=(const Vector2& _rv);
    Vector3& operator-=(const Vector2& _rv);
    Vector3& operator*=(const Vector2& _rv);

    Vector3 operator+(const Vector2& _v) const;
    Vector3 operator-(const Vector2& _v) const;
    Vector3 operator*(const Vector2& _v) const;
};

Vector3 operator*(const float _f, const Vector3& _v);
Vector3 operator/(const float _f, const Vector3& _v);

namespace FMath
{
    /// <summary>
	/// ベクトルの加算
	/// </summary>
	/// <param name="_v1">ベクトル1</param>
	/// <param name="_v2">ベクトル2</param>
	/// <returns>加算結果ベクトル</returns>
    Vector3 Add(const Vector3& _v1, const Vector3& _v2);
	/// <summary>
	/// ベクトルの減算
	/// </summary>
	/// <param name="_v1">ベクトル1</param>
	/// <param name="_v2">ベクトル2</param>
	/// <returns>減算結果ベクトル</returns>
    Vector3 Subtract(const Vector3& _v1, const Vector3& _v2);
	/// <summary>
	/// ベクトルのスカラー倍
	/// </summary>
	/// <param name="_scalar">スカラー値</param>
	/// <param name="_v">ベクトル</param>
	/// <returns>スカラー倍結果ベクトル</returns>
    Vector3 Multiply(float _scalar, const Vector3& _v);
	/// <summary>
	/// ベクトルの内積
	/// </summary>
	/// <param name="_v1">ベクトル1</param>
	/// <param name="_v2">ベクトル2</param>
	/// <returns>内積</returns>
    float Dot(const Vector3& _v1, const Vector3& _v2);
	/// <summary>
	/// ベクトルの外積
	/// </summary>
	/// <param name="_v1">ベクトル1</param>
	/// <param name="_v2">ベクトル2</param>
	/// <returns>外積ベクトル</returns>
    float Length(const Vector3& _v);
	/// <summary>
	/// ベクトルの正規化
	/// </summary>
	/// <param name="_v">ベクトル</param>
	/// <returns>正規化ベクトル</returns>
    Vector3 Normalize(const Vector3& _v);
	/// <summary>
	/// ベクトルの変換
	/// </summary>
	/// <param name="_vector">ベクトル</param>
	/// <param name="_matrix">変換行列</param>
	/// <returns>変換後ベクトル</returns>
    Vector3 Transform(const Vector3& _vector, const Matrix4x4& _matrix);
}