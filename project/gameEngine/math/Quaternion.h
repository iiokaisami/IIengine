#pragma once

#include "MyMath.h"

/// <summary>
/// 四元数
/// </summary>
struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

/// <summary>
/// Quaternionの積
/// </summary>
/// <param name="lhs">左辺Quaternion</param>
/// <param name="rhs">右辺Quaternion</param>
/// <returns>積Quaternion</returns>
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

/// <summary>
/// 単位Quaternionの作成を返す
/// </summary>
/// <returns>単位Quaternion</returns>
Quaternion IdentityQuaternion();

/// <summary>
/// 共役Quaternionを返す
/// </summary>
/// <param name="quaternion">対象Quaternion</param>
/// <returns>共役Quaternion</returns>
Quaternion Conjugate(const Quaternion& quaternion);

/// <summary>
/// Quaternionの normを返す
/// </summary>
/// <param name="quaternion">対象Quaternion</param>
/// <returns>norm値</returns>
float Norm(const Quaternion& quaternion);

/// <summary>
/// 正規化したQuaternionを返す
/// </summary>
/// <param name="quaternion">対象Quaternion</param>
/// <returns>正規化Quaternion</returns>
Quaternion NormalizeQuaternion(const Quaternion& quaternion);

/// <summary>
///逆Quaternionを返す
/// </summary>
/// <param name="quaternion">対象Quaternion</param>
/// <returns>逆Quaternion</returns>
Quaternion Inverse(const Quaternion& quaternion);

/// <summary>
/// 軸と角度からQuaternionを作成して返す
/// </summary>
/// <param name="axis">回転軸ベクトル 正規化されていることが前提</param>
/// <param name="angle">回転角度 ラジアン</param>
/// <returns>Quaternion</returns>
Quaternion MakeAxisAngle(const Vector3& axis, float angle);

/// <summary>
/// Quaternionをオイラー角に変換して返す
/// </summary>
/// <param name="q">対象Quaternion</param>
/// <returns>オイラー角ベクトル (x: roll, y: pitch, z: yaw)</returns>
Vector3 QuaternionToEuler(const Quaternion& q);