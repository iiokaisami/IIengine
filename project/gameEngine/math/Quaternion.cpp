#include "Quaternion.h"

#include <numbers>

Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion result{};
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
	return result;
}

Quaternion IdentityQuaternion()
{
	return Quaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
}

Quaternion Conjugate(const Quaternion& quaternion)
{
	return Quaternion{ -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

float Norm(const Quaternion& quaternion)
{
	return std::sqrt(quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w);
}

Quaternion NormalizeQuaternion(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	return Quaternion{ quaternion.x / norm, quaternion.y / norm, quaternion.z / norm, quaternion.w / norm };
}

Quaternion Inverse(const Quaternion& quaternion)
{
	float norm = Norm(quaternion);
	Quaternion conjugate = Conjugate(quaternion);
	return Quaternion{ conjugate.x / (norm * norm), conjugate.y / (norm * norm), conjugate.z / (norm * norm), conjugate.w / (norm * norm) };
}

Quaternion MakeAxisAngle(const Vector3& axis, float angle)
{
	float half = angle * 0.5f;
	float s = std::sin(half);

	return NormalizeQuaternion(Quaternion{
		axis.x * s,
		axis.y * s,
		axis.z * s,
		std::cos(half)
		});
}

Vector3 QuaternionToEuler(const Quaternion& q)
{
	Vector3 euler{};

	// roll (X軸)
	float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
	euler.x = std::atan2(sinr_cosp, cosr_cosp);

	// pitch (Y軸)
	float sinp = 2.0f * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		euler.y = std::copysign(std::numbers::pi_v<float> / 2.0f, sinp);
	else
		euler.y = std::asin(sinp);

	// yaw (Z軸)
	float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
	euler.z = std::atan2(siny_cosp, cosy_cosp);

	return euler;
}
