#pragma once

#include <functional>
#include <memory>

#include <Vector3.h>
#include "ICameraController.h"

// 前方宣言
class Camera;

/// <summary>
/// 追尾カメラコントローラー
/// </summary>
class FollowController : public ICameraController
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="targetPos">追尾対象の位置を返す関数</param>
	/// <param name="positionLerp">位置の補間値(0.0f~1.0f)</param>
	/// <param name="rotationLerp">回転の補間値(0.0f~1.0f)</param>
	/// </summary>
	FollowController(std::shared_ptr<Camera> camera, std::function<Vector3()> targetPos, float positionLerp = 0.8f, float rotationLerp = 0.25f, Vector3 targetRot = Vector3{ 1.2f, 0.0f, 0.0f });

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	void Update(float dt) override;

	// 開始
	void Start() override { active_ = true; }

	// 停止
	void Stop() override { active_ = false; }

	// カメラ位置と回転を強制セット
	void SnapToTarget();

public: // ゲッター

	// コントローラーが有効かどうか
	bool IsActive() const override { return active_; }

private:

	// カメラ
	std::shared_ptr<Camera> camera_ = nullptr;
	// 追尾対象の位置を返す関数
	std::function<Vector3()> targetPos_ = nullptr;
	// 位置の補間値
	float positionLerp_ = 0.8f;
	// 回転の補間値
	float rotationLerp_ = 0.25f;
	// 有効フラグ
	bool active_ = true;

	const Vector3 offset_ = { 0.0f, 78.0f, -17.0f };
	Vector3 targetRot_ = { 2.0f, 0.0f, 0.0f };
};

