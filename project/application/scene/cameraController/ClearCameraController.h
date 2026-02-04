#pragma once

#include <memory>
#include <functional>

#include <Vector3.h>

#include "ICameraController.h"
#include "../../../gameEngine/3d/Camera.h"

// 前方宣言
class Camera;

/// <summary>
/// クリアカメラコントローラー
/// </summary>
class ClearCameraController : public ICameraController
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="targetPos">追尾対象の位置を返す関数</param>
	/// <param name="duration">クリアカメラの持続時間</param>
	ClearCameraController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, float duration = 3.0f, float rotations = 20.0f, float rotationSpeedMultiplier = 0.12f, float heightRise = 30.0f, float fadeStartThreshold = 0.4f);

	// デストラクタ
	~ClearCameraController() override = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	void Update(float dt) override;

	// 開始
	void Start() override;

	// 停止
	void Stop() override;

public: // ゲッター

    bool IsActive() const override { return active_; }

public: // セッター

    // コールバック：フェード開始と演出完了
    void SetOnFadeStart(std::function<void()> cb) { onFadeStart_ = std::move(cb); }
    void SetOnFinish(std::function<void()> cb) { onFinish_ = std::move(cb); }

private:
    std::shared_ptr<IIEngine::Camera> camera_;
    std::function<Vector3()> getPlayerPos_;

    // 開始時に計算して保持
    float startAngle_ = 0.0f;
    float startRadius_ = 0.0f;
    float startHeight_ = 0.0f;

    // 演出パラメータ
    float duration_ = 3.0f;
    float timer_ = 0.0f;
    float rotations_ = 20.0f;
    float rotationSpeedMultiplier_ = 0.12f;
    float heightRise_ = 30.0f;
    float fadeStartThreshold_ = 0.4f;

    bool active_ = false;
    bool fadeSignaled_ = false;

    std::function<void()> onFadeStart_ = nullptr;
    std::function<void()> onFinish_ = nullptr;

};

