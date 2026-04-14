#pragma once

#include <functional>
#include <memory>

#include <Vector3.h>
#include "ICameraController.h"
#include "../../../gameEngine/3d/Camera.h"

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
	/// <param name="targetVelocity">追尾対象の速度を返す</param>
	/// <param name="positionLerp">位置の補間値(0.0f~1.0f)</param>
	/// <param name="rotationLerp">回転の補間値(0.0f~1.0f)</param>
	/// </summary>
	FollowController(std::shared_ptr<IIEngine::Camera> camera, std::function<Vector3()> targetPos, std::function<Vector3()> targetVelocity, float positionLerp = 0.8f, float rotationLerp = 0.25f, const Vector3& targetRot = Vector3{ 1.2f, 0.0f, 0.0f }, const Vector3& offset = Vector3{ 0.0f, 45.0f, -35.0f });

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

	// 状態を追尾対象にリセット
	void ResetStateToTarget();

public: // ゲッター

	// コントローラーが有効かどうか
	bool IsActive() const override { return active_; }

public: // セッター

	/// <summary>
	/// オフセットを動的に変更するためのセッター
	/// </summary>
	/// <param name="offset">新しいオフセット</param>
	void SetOffset(const Vector3& offset) { offset_ = offset; }

	/// <summary>
	/// 必要であれば回転目標のセッターも追加
	/// </summary>
	/// <param name="targetRot">新しい回転目標</param>
	void SetTargetRot(const Vector3& targetRot) { targetRot_ = targetRot; }

private:

	// カメラ
	std::shared_ptr<IIEngine::Camera> camera_ = nullptr;
	// 追尾対象の位置を返す関数
	std::function<Vector3()> targetPos_ = nullptr;
	// 追尾対象の速度を返す関数
	std::function<Vector3()> targetVelocity_;
	// 位置の補間値
	float positionLerp_ = 0.8f;
	// 回転の補間値
	float rotationLerp_ = 0.25f;
	// 有効フラグ
	bool active_ = true;

	/*const Vector3 offset_ = { 0.0f, 78.0f, -17.0f };
	Vector3 targetRot_ = { 2.0f, 0.0f, 0.0f };*/

	Vector3 offset_ = { 0.0f, 45.0f, -35.0f };
	Vector3 targetRot_ = { 0.4f, 0.0f, 0.0f };


	// 追尾対象の速度の平滑化済み値
	Vector3 smoothedVelocity_ = { 0.0f, 0.0f, 0.0f };
	// プレイヤー最後停止位置
	Vector3 lastStopPosition_ = { 0.0f, 0.0f, 0.0f };         
	// 追尾開始までの遅延時間
	float followDelay_ = 0.5f;                                
	// 停止までの遅延時間
	float stopDelay_ = 0.3f;                                  
	// 追尾タイマー
	float followTimer_ = 0.0f;                                
	// 停止タイマー
	float stopTimer_ = 0.0f;                                  
	// 現在の追尾状態
	bool isFollowing_ = false;  
	// 補間進行度
	float blendingProgress_ = 0.0f;
	
};