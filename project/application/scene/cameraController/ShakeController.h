#pragma once

#include <functional>
#include <memory>

#include "ICameraController.h"

// 前方宣言
class Camera;

/// <summary>
/// カメラシェイクコントローラー
/// </summary>
class ShakeController : public ICameraController
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="consumeHit">ヒットを消費する関数(ヒットがあればtrueを返す)</param>
	/// <param name="defaultDuration">シェイク継続時間</param>
	/// <param name="defaultAmp">シェイク振幅</param>
	ShakeController(std::shared_ptr<Camera> camera, std::function<bool()> consumeHit, float defaultDuration = 0.3f, float defaultAmp = 1.0f);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	void Update(float dt) override;
	// 開始
	void Start() override { active_ = true; }
	// 停止
	void Stop() override { active_ = false; }

public: // ゲッター

	// コントローラーが有効かどうか
	bool IsActive() const override { return active_; }

private:

	// カメラ
	std::shared_ptr<Camera> camera_ = nullptr;
	// ヒットを消費する関数
	std::function<bool()> consumeHit_ = nullptr;
	// 有効フラグ
	bool active_ = true;
	// シェイク継続時間
	float duration_ = 0.3f;
	// シェイク振幅
	float amplitude_ = 1.0f;

};

