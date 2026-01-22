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
Quaternion Normalize(const Quaternion& quaternion);

/// <summary>
///逆Quaternionを返す
/// </summary>
/// <param name="quaternion">対象Quaternion</param>
/// <returns>逆Quaternion</returns>
Quaternion Inverse(const Quaternion& quaternion);