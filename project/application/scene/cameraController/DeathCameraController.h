#pragma once

#include <memory>
#include <functional>
#include <algorithm>
#include <cmath>
#include <corecrt_math_defines.h>

#include <Vector3.h>

#include "ICameraController.h"
#include "../../../gameEngine/3d/Camera.h"

// 前方宣言
class Camera;

/// <summary>
/// デスカメラコントローラー
/// </summary>
class DeathCameraController : public ICameraController
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="targetPos">追尾対象の位置を返す関数</param>
	/// <param name="duration">デスカメラの持続時間</param>
	DeathCameraController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, float duration = 3.0f, float rotations = 2.0f, float endRadius = 12.0f, float endHeight = 12.0f);

	// デストラクタ
	~DeathCameraController() override = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	void Update(float dt) override;

	// 開始
	void Start() override;

	// 停止
	void Stop() override;

public: // セッター

	// 演出完了時のコールバック
	void SetOnFinish(std::function<void()> callback) { onFinish_ = std::move(callback); }

public: // ゲッター

	// コントローラーが有効かどうか
	bool IsActive() const override { return active_; }

private:

	// カメラ
	std::shared_ptr<IIEngine::Camera> camera_;
	// 追尾対象の位置を返す関数
	std::function<Vector3()> getPlayerPos_;

	// start のときに計算して保持する
	float startAngle_ = 0.0f;
	float startRadius_ = 0.0f;
	float startHeight_ = 0.0f;

	// 終了時のターゲット
	float endRadius_ = 12.0f;
	float endHeight_ = 12.0f;

	// 演出時間関連
	float duration_ = 3.0f;
	float timer_ = 0.0f;
	float rotations_ = 2.0f;

	// 有効フラグ
	bool active_ = false;

	// 演出完了時のコールバック
	std::function<void()> onFinish_ = nullptr;

	// 1回転 = 2π ラジアン
	float kTau_ = 2.0f * static_cast<float>(M_PI);;


};

