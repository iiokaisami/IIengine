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
/// バリア破壊カメラコントローラー
/// バリアが破壊されたときのカメラ演出を管理
/// </summary>
class BarrierBreakCameraController : public ICameraController
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="camera">カメラ</param>
	///  <param name="playerPos">プレイヤー位置を返す</param>
    /// <param name="goalPos">ゴール/バリア中心位置を返す</param>
    /// <param name="isBarrierBroken">バリア破壊完了(戻るタイミング)を返す</param>
	BarrierBreakCameraController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, std::function<Vector3()> returnPos, std::function<bool()> isBarrierBroken);

	// デストラクタ
	~BarrierBreakCameraController() override = default;

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	void Update(float dt) override;

	// 開始
	void Start() override;

public: // ゲッター

	// コントローラーが有効かどうか
	bool IsActive() const override { return active_; }

public: // セッター

	// コールバック：ゴール到達と演出完了
	void SetOnArriveGoal(std::function<void()> cb) { onArriveGoal_ = std::move(cb); }
	void SetOnFinish(std::function<void()> cb) { onFinish_ = std::move(cb); }

private:

    // LookAtで回転を作る
    void LookAt(const Vector3& eye, const Vector3& target);


private:

    enum class Phase
    {
        None,
        ToGoal,          // player -> goal へ
        WaitBroken,      // goalを注視して待つ
        ReturnToPlayer,  // goal -> player へ戻る
        Finished
    };

	// カメラ
    std::shared_ptr<IIEngine::Camera> camera_;
	// 位置取得用コールバック
    std::function<Vector3()> getPlayerPos_;
    std::function<Vector3()> getGoalPos_;
    std::function<bool()> isBarrierBroken_;

	// コントローラー有効フラグ
    bool active_ = false;

    // フェーズ管理
    Phase phase_ = Phase::None;

    // タイマー
    float timer_ = 0.0f;

    // 演出時間
    float toGoalDuration_ = 0.6f;      // Player→Goalへ寄る時間
    float returnDuration_ = 0.6f;      // Goal→Playerへ戻る時間

    // 開始時点のカメラ状態
    Vector3 startCamPos_{};
    Vector3 startCamRot_{};

    // 目標構図
    Vector3 goalViewOffset_ = { 0.0f, 10.0f, -18.0f };
    Vector3 playerViewOffset_ = { 1.4f, 0.0f, 0.0f }; // Followのoffsetに寄せるなら後で合わせる

    // フェーズ開始時の補間始点・終点
    Vector3 phaseFromPos_{};
    Vector3 phaseToPos_{};

    // 到達通知/終了通知
    bool arrivedGoalSignaled_ = false;
    std::function<void()> onArriveGoal_ = nullptr;
    std::function<void()> onFinish_ = nullptr;

};