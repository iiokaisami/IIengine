#pragma once

#include <memory>
#include <functional>

#include <Vector3.h>
#include "ICameraController.h"

// 前方宣言
class Camera;

/// <summary>
/// スタートカメラコントローラー
/// </summary>
class StartCameraController : public ICameraController
{
public:

    /// <summary>
	/// コンストラクタ
    /// </summary>
	/// <param name="camera">カメラ</param>
	/// <param name="cameraStart">カメラ開始位置</param>
	/// <param name="cameraControl1">カメラ制御点1</param>
	/// <param name="cameraControl2">カメラ制御点2</param>
	/// <param name="cameraEnd">カメラ終了位置</param>
	/// <param name="cameraStartRot">カメラ開始回転</param>
	/// <param name="cameraEndRot">カメラ終了回転</param>
	/// <param name="duration">持続時間</param>
    StartCameraController(std::shared_ptr<Camera> camera,const Vector3& cameraStart, const Vector3& cameraControl1, const Vector3& cameraControl2, const Vector3& cameraEnd, const Vector3& cameraStartRot, const Vector3& cameraEndRot, float duration = 1.5f);

	// デストラクタ
	~StartCameraController() override = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	void Update(float dt) override;

	// 演出を開始する
	void Start() override;

	// 演出を停止する
	void Stop() override;

public: // ゲッター

	bool IsActive() const override { return active_; }

public: // セッター

	// 演出完了時のコールバック
	void SetOnFinish(std::function<void()> cb) { onFinish_ = std::move(cb); }

private:

	// カメラ
	std::shared_ptr<Camera> camera_;

	// ベジェ曲線の制御点と位置
	Vector3 startPos_;
	Vector3 control1_;
	Vector3 control2_;
	Vector3 endPos_;

	// 回転の開始と終了
	Vector3 startRot_;
	Vector3 endRot_;

	// 演出時間関連
	float duration_ = 1.5f;
	float timer_ = 0.0f;
	bool active_ = false;

	// 演出完了コールバック
	std::function<void()> onFinish_;

};

