#include "Camera.h"
#include "WinApp.h"

Camera::Camera()
	: transform_({ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} })
	, fovY_(0.45f)
	, aspectRatio_(float(WinApp::kClientWidth) / float(WinApp::kClientHeight))
	, nearClip_(0.1f)
	, farClip_(100.0f)
	, worldMatrix_(MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate))
	, viewMatrix_(Inverse(worldMatrix_))
	, projectionMatrix_(MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_))
	, viewProjectionMatrix_(viewMatrix_* projectionMatrix_)
{}

void Camera::Update()
{
	worldMatrix_= MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(worldMatrix_);
	projectionMatrix_ = MakePerspectiveFovMatrix(fovY_, aspectRatio_, nearClip_, farClip_);
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

void Camera::StartShake(float duration, float magnitude)
{
	isShaking_ = true;
	shakeDuration_ = duration;
	shakeMagnitude_ = magnitude;
	shakeTimeElapsed_ = 0.0f;
	randomDistribution_ = std::uniform_real_distribution<float>(-1.0f, 1.0f);
}

void Camera::UpdateShake(float deltaTime)
{
	if (!isShaking_) return;

	shakeTimeElapsed_ += deltaTime;
	if (shakeTimeElapsed_ >= shakeDuration_)
	{
		isShaking_ = false;
		return;
	}

	float offsetX = randomDistribution_(randomEngine_) * shakeMagnitude_;
	float offsetY = randomDistribution_(randomEngine_) * shakeMagnitude_;
	float offsetZ = randomDistribution_(randomEngine_) * shakeMagnitude_;

	// カメラの位置をシェイク
	Vector3 originalPosition = GetPosition();
	SetPosition(originalPosition + Vector3(offsetX, offsetY, offsetZ));
}

Vector2 Camera::WorldToScreen(const Vector3& worldPos) const
{
	// ワールド座標をクリップ座標に変換
	float x = worldPos.x;
	float y = worldPos.y;
	float z = worldPos.z;
	float w = 1.0f;

	// ビュープロジェクション行列をかける
	Vector4 clip;
	const Matrix4x4& m = viewProjectionMatrix_;
	clip.x = x * m.m[0][0] + y * m.m[1][0] + z * m.m[2][0] + w * m.m[3][0];
	clip.y = x * m.m[0][1] + y * m.m[1][1] + z * m.m[2][1] + w * m.m[3][1];
	clip.z = x * m.m[0][2] + y * m.m[1][2] + z * m.m[2][2] + w * m.m[3][2];
	clip.w = x * m.m[0][3] + y * m.m[1][3] + z * m.m[2][3] + w * m.m[3][3];

	// 透視除算
	if (fabs(clip.w) < 1e-6f) return Vector2(-10000.0f, -10000.0f);

	Vector3 ndc = { clip.x / clip.w, clip.y / clip.w, clip.z / clip.w };

	// NDCをスクリーン座標に変換d
	float sx = (ndc.x * 0.5f + 0.5f) * float(WinApp::kClientWidth);
	float sy = (-ndc.y * 0.5f + 0.5f) * float(WinApp::kClientHeight);

	return Vector2(sx, sy);
}
