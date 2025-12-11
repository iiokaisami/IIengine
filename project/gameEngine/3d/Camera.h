#pragma once

#include "MyMath.h"

#include <random>

/// <summary>
/// カメラクラス
/// カメラの位置、向き、射影変換行列を管理
/// </summary>
class Camera
{
public:

	Camera();
	
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// シェイク開始
	/// </summary>
	/// <param name="duration">シェイクの持続時間</param>
	/// <param name="magnitude">シェイクの強さ</param>
	void StartShake(float duration, float magnitude);

	/// <summary>
	/// シェイク更新
	/// </summary>
	/// <param name="deltaTime">前フレームからの経過時間</param>
	void UpdateShake(float deltaTime);

	Vector2 WorldToScreen(const Vector3& worldPos) const;

public: // セッター

	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }
	
	/// <summary>
	/// 位置を設定
	/// </summary>
	/// <param name="translate">位置</param>
	void SetPosition(Vector3 translate) { transform_.translate = translate; }
	
	/// <summary>
	/// 視野角を設定
	/// </summary>
	/// <param name="fovY">視野角</param>
	void SetFovY(float fovY) { fovY_ = fovY; }
	
	/// <summary>
	/// アスペクト比を設定
	/// </summary>
	/// <param name="aspectRatio">アスペクト比</param>
	void SetAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio; }
	
	/// <summary>
	/// ニアクリップ距離を設定
	/// </summary>
	/// <param name="nearClip">ニアクリップ距離</param>
	void SetNearClip(float nearClip) { nearClip_ = nearClip; }

	/// <summary>
	/// ファークリップ距離を設定
	/// </summary>
	/// <param name="farClip">ファークリップ距離</param>
	void SetFarClip(float farClip) { farClip_ = farClip; }

public: // ゲッター

	// 各種行列、情報の取得
	const Vector3& GetRotate() const { return transform_.rotate; }

	// 位置の取得
	const Vector3& GetPosition() const { return transform_.translate; }

	// ワールド行列の取得
	const Matrix4x4& GetWorldMatrix() const { return worldMatrix_; }
	
	// ビュー行列の取得
	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	
	// 透視投影行列の取得
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	
	// ビュープロジェクション行列の取得
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

	// シェイク中かどうか取得
	bool IsShaking() const { return isShaking_; }

private:

	struct Transform
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

private:

	Transform transform_{};
	Matrix4x4 worldMatrix_{};
	Matrix4x4 viewMatrix_{};

	Matrix4x4 projectionMatrix_{};
	float fovY_;			// 水平方向視野角
	float aspectRatio_;     // アスペクト比
	float nearClip_;		// ニアクリップ距離
	float farClip_;		    // ファークリップ距離

	Matrix4x4 viewProjectionMatrix_{};

	// シェイクのための変数
	bool isShaking_ = false;
	float shakeDuration_ = 0.0f;
	float shakeMagnitude_ = 0.0f;
	float shakeTimeElapsed_ = 0.0f;
	std::default_random_engine randomEngine_;
	std::uniform_real_distribution<float> randomDistribution_;
};