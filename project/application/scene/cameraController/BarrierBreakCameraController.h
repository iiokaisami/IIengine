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
	/// <param name="targetPos">プレイヤー位置を返す</param>
	/// <param name="returnPos">ゴール/バリア中心位置を返す</param>
	/// <param name="getFollowCamPos">プレイヤーを追従するカメラの位置を返す</param>
	/// <param name="isBarrierBroken">バリア破壊完了を返す</param>
    BarrierBreakCameraController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, std::function<Vector3()> returnPos, std::function<Vector3()> getFollowCamPos, std::function<bool()> isBarrierBroken);

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

	// バリアに到達したかどうか
	bool IsArrivedGoal() const { return arrivedGoalSignaled_; }

public: // セッター

	// コールバック
    // ゴール到達
	void SetOnArriveGoal(std::function<void()> cb) { onArriveGoal_ = std::move(cb); }
    // 演出完了
    void SetOnFinish(std::function<void()> cb) { onFinish_ = std::move(cb); }

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
    std::function<Vector3()> getPlayerPos_ = nullptr;
    std::function<Vector3()> getGoalPos_ = nullptr;
    std::function<Vector3()> getFollowCamPos_ = nullptr;
    std::function<bool()> isBarrierBroken_;

	// コントローラー有効フラグ
    bool active_ = false;
    
    // フェーズ管理
    Phase phase_ = Phase::None;

    // タイマー
    float timer_ = 0.0f;
    float waitTime_ = 1.5f;

    // 演出時間
    float toGoalDuration_ = 0.6f;      // Player→Goalへ寄る時間
    float returnDuration_ = 0.6f;      // Goal→Playerへ戻る時間

    // カメラのオフセット
    Vector3 goalExtraOffset_ = { 0.0f, 5.0f, -8.0f };

	// 補間開始時の位置
    Vector3 phaseFromPos_{};
    Vector3 phaseToPos_{};

	// 到達位置を保持
    Vector3 arrivedGoalPos_{};

	// 回転の補間用
    Vector3 startRot_{};
    Vector3 goalTargetRot_ = { 0.4f,0.0f,0.0f };
    Vector3 initialRot_{};
	// 戻るときはさらに上から見下ろす感じにする
    Vector3 returnStartRot_{};
    Vector3 returnTargetRot_ = { 2.0f, 0.0f, 0.0f };

    bool arrivedGoalSignaled_ = false;
    std::function<void()> onArriveGoal_ = nullptr;
    std::function<void()> onFinish_ = nullptr;

};