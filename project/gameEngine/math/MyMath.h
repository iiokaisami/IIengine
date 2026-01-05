#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cmath>

#include "Matrix4x4.h"
#include "Vector3.h"

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};

struct Matrix3x3
{
	float m[3][3];
};



/// <summary>
/// 内積
/// </summary>
/// <param name="v1">vector1</param>
/// <param name="v2">vector2</param>
/// <returns>内積</returns>
float Dot(const Vector3& v1, const Vector3& v2);
/// <summary>
/// 長さ
/// </summary>
/// <param name="v">vector</param>
/// <returns>長さ</returns>
float Length(const Vector3& v);
/// <summary>
/// 正規化
/// </summary>
/// <param name="v">vector</param>
/// <returns>正規化ベクトル</returns>
Vector3 Normalize(const Vector3& v);
/// <summary>
/// 外積
/// </summary>
/// <param name="v1">vector1</param>
/// <param name="v2">vector2</param>
/// <returns>外積ベクトル</returns>
Vector3 Cross(const Vector3& v1, const Vector3& v2);
/// <summary>
/// ベクトル変換
/// </summary>
/// <param name="v">vector</param>
/// <param name="m">matrix</param>
/// <returns>変換後ベクトル</returns>
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);

// -----行列-----
/// <summary>
/// 行列の乗算
/// </summary>
/// <param name="m1">matrix1</param>
/// <param name="m2">matrix2</param>
/// <returns>乗算結果行列</returns>
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 平行移動行列
/// </summary>
/// <param name="translate">移動量</param>
/// <returns>平行移動行列</returns>
Matrix4x4 MakeTranslateMatrix(const Vector3& translate);
/// <summary>
/// 拡大縮小行列
/// </summary>
/// <param name="scale">拡大率</param>
/// <returns>拡大縮小行列</returns>
Matrix4x4 MakeScaleMatrix(const Vector3& scale);

/// <summary>
/// X回転行列
/// </summary>
/// <param name="radian">ラジアン</param>
/// <returns>X回転行列</returns>
Matrix4x4 MakeRotateXMatrix(float radian);
/// <summary>
/// Y回転行列
/// </summary>
/// /// <param name="radian">ラジアン</param>
/// <returns>Y回転行列</returns>
Matrix4x4 MakeRotateYMatrix(float radian);
/// <summary>
/// Z回転行列
/// </summary>
/// <param name="radian">ラジアン</param>
/// <returns>Z回転行列</returns>
Matrix4x4 MakeRotateZMatrix(float radian);
/// <summary>
/// 任意軸回転行列
/// </summary>
/// <param name="axis">軸</param>
/// <param name="angle">角度</param>
/// <returns>任意軸回転行列</returns>
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle);
/// <summary>
/// ベクトルからベクトルへの変換行列
/// </summary>
/// <param name="from">始まりのベクター</param>
/// <param name="to">終わりのベクター</param>
/// <returns>変換行列</returns>
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);
/// <summary>
/// アフィン変換行列
/// </summary>
/// <param name="scale">スケール</param>
/// <param name="rotate">回転</param>
/// <param name="translate">移動</param>
/// <returns>アフィン変換行列</returns>
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

/// <summary>
/// 逆行列
/// </summary>
/// <param name="m">matrix</param>
/// <returns>逆行列</returns>
Matrix4x4 Inverse(const Matrix4x4& m);
/// <summary>
/// 転置行列
/// </summary>
/// <param name="m">matrix</param>
/// <returns>転置行列</returns>
Matrix4x4 Transpose(const Matrix4x4& m);
/// <summary>
/// 逆転置行列
/// </summary>
/// <param name="m">matrix</param>
/// <returns>逆転置行列</returns>
Matrix4x4 InverseTranspose(const Matrix4x4& m);
/// <summary>
/// 単位行列の作成
/// </summary>
/// <returns>単位行列</returns>
Matrix4x4 MakeIdentity4x4();

/// <summary>
/// ベクトルと行列の乗算
/// </summary>
/// <param name="vec">vector</param>
/// <param name="mat">matrix</param>
/// <returns></returns>
Vector3 Multiply(const Vector3& vec, const Matrix4x4& mat);

// -----座標系-----
/// <summary>
/// 透視投影行列
/// </summary>
/// <param name="fovY">フォブ角度</param>
/// <param name="aspectRatio">アスペクト比</param>
/// <param name="nearClip">ニアクリップ</param>
/// <param name="farClip">ファークリップ</param>
/// <returns>透視投影行列</returns>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
/// <summary>
/// 正射影行列
/// </summary>
/// <param name="left">左</param>
/// <param name="top">上</param>
/// <param name="right">右</param>
/// <param name="bottom">下</param>
/// <param name="nearClip">ニアクリップ</param>
/// <param name="farClip">ファークリップ</param>
/// <returns>正射影行列</returns>
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
/// <summary>
/// ビューポート変換行列
/// </summary>
/// <param name="left">左</param>
/// <param name="top">上</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="minDepth">最小深度</param>
/// <param name="maxDepth">最大深度</param>
/// <returns>ビューポート変換行列</returns>
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
/// <summary>
/// ベクトルの変換
/// </summary>
/// <param name="vector">ベクトル</param>
/// <param name="matrix">行列</param>
/// <returns>変換後ベクトル</returns>
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

// -----補間-----
/// <summary>
/// 線形補間
/// </summary>
/// <param name="v1">始まりの値</param>
/// <param name="v2">終わりの値</param>
/// <param name="t">補間係数(0.0~1.0)</param>
/// <returns>補間後の値</returns>
float Lerp(float v1, float v2, float t);
/// <summary>
/// ベクトルの線形補間
/// </summary>
/// <param name="v1">始まりのベクトル</param>
/// <param name="v2">終わりのベクトル</param>
/// <param name="t">補間係数(0.0~1.0)</param>
/// <returns>補間後のベクトル</returns>
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
/// <summary>
/// 球面線形補間
/// </summary>
/// <param name="v1">始まりのベクトル</param>
/// <param name="v2">終わりのベクトル</param>
/// <param name="t">補間係数(0.0~1.0)</param>
/// <returns>補間後のベクトル</returns>
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);
/// <summary>
/// 角度の線形補間
/// </summary>
/// <param name="a">始まりの角度</param>
/// <param name="b">終わりの角度</param>
/// <param name="t">補間係数(0.0~1.0)</param>
/// <returns>補間後の角度</returns>
float LerpAngle(float a, float b, float t);

/// <summary>
/// 3次ベジェ曲線
/// </summary>
/// <param name="p0">始点</param>
/// <param name="p1">制御点1</param>
/// <param name="p2">制御点2</param>
/// <param name="p3">終点</param>
/// <param name="t">補間係数(0.0~1.0)</param>
/// <returns>補間後のベクトル</returns>
Vector3 Bezier3(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);

namespace MyMath 
{
	inline float cot(float a) { return cos(a) / sin(a); }
}