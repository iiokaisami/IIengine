#pragma once

#include <Framework.h>

#include "../../gameEngine/baseScene/BaseScene.h"
#include "../../application/objects/player/Player.h"
#include "../../application/objects/enemy/EnemyManager.h"
#include "../../application/objects/field/Field.h"
#include "../../application/objects/field/Wall.h"
#include "../../application/objects/field/Goal.h"
#include "../../gameEngine/collider/ColliderManager.h"
#include "../../gameEngine/level_editor/LevelDataLoader.h"

#include "../../gameEngine/transition/BlockRiseTransition.h"
#include "../../gameEngine/transition/FadeTransition.h"

#include "cameraController/SceneCamera.h"
#include "cameraController/ShakeController.h"
#include "cameraController/FollowController.h"
#include "cameraController/StartCameraController.h"
#include "cameraController/DeathCameraController.h"
#include "cameraController/ClearCameraController.h"

/// <summary>
///	ゲームプレイシーン
/// プレイヤー操作、敵出現、フィールド、ゴール、衝突判定、シーン遷移
/// </summary>
class GamePlayScene : public BaseScene
{
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

private:

	// このシーンで出す全てのImGui
	void AllImGui();

private:

	// カメラ
	CameraManager& cameraManager = CameraManager::GetInstance();
	std::shared_ptr<SceneCamera> camera_ = nullptr;
    
    uint32_t activeIndex = 0;
	Vector3 cameraRotate_ = { -0.9f,0.0f,0.0f};
	Vector3 cameraPosition_ = { 0.0f,-20.0f,0.0f };

	// 2Dスプライト
	std::vector<std::unique_ptr<Sprite>>sprites_ = {};
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	// 描画するスプライトの数
	uint32_t spriteNum_ = 1;

	// 遷移
	std::unique_ptr<IIEngine::BlockRiseTransition> blockTransition_ = nullptr;
	std::unique_ptr<IIEngine::FadeTransition> fadeTransition_ = nullptr;
	bool isTransitioning_ = false;

	// 衝突判定
	ColliderManager* colliderManager_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> pPlayer_ = nullptr;

	// エネミー
	std::unique_ptr<EnemyManager> pEnemyManager_ = nullptr;

	// フィールド
	std::unique_ptr<Field> pField_ = nullptr;

	// 壁
	std::vector<std::unique_ptr<Wall>> pWalls_;

	// ゴール
	std::unique_ptr<Goal> pGoal_ = nullptr;
	
	// カメラが静止時に追従する基準位置
	Vector3 cameraRestCenter_; 
	// カメラが現在静止モードかどうか
	bool cameraIsResting_ = true;

	// レベルデータローダー
	std::unique_ptr<IIEngine::LevelData> levelData_ = nullptr;

	// スタートカメラ演出変数
	bool isStartCamera_ = false;
	float cameraStartTimer_ = 0.0f;
	float cameraStartDuration_ = 1.5f;
	Vector3 camStart_ = { 0.16f, 5.0f, 8.0f };
	Vector3 camControl1_ = { 0.16f, 5.0f, -8.0f };
	Vector3 camControl2_ = { 0.16f, 50.0f, -20.0f };
	Vector3 camEnd_ = { 0.16f, 78.5f, -19.8f };
	Vector3 camStartRot_ = { 0.0f, 0.0f, 0.0f };
	Vector3 camEndRot_ = { 1.4f, 0.0f, 0.0f };

	// 死亡カメラ演出変数
	bool isDeathCamera_ = false;
	bool isDeadCameraPlayer_ = false;
	float deathCameraTimer_ = 0.0f;
	float deathCameraDuration_ = 3.0f;
	float deathCameraRotations_ = 2.0f;
	float deathStartAngle_ = 0.0f;
	float deathStartRadius_ = 0.0f;
	float deathEndRadius_ = 12.0f;
	float deathStartHeight_ = 0.0f;
	float deathEndHeight_ = 12.0f;
	float deathTargetAngleOffset_ = 0.0f;

	// クリア時カメラ演出変数
	bool isClearMoment_ = false;
	bool isClearCamera_ = false;
	bool isClearFadeStart_ = false;
	float clearCameraTimer_ = 0.0f;
	float clearCameraDuration_ = 3.0f;
	float clearCameraRotations_ = 20.0f;
	float clearStartAngle_ = 0.0f;
	float clearStartRadius_ = 20.0f;
	float clearEndRadius_ = 5.0f;
	float clearStartHeight_ = 10.0f;
	float clearEndHeight_ = 3.0f;
	float clearTargetAngleOffset_ = 0.0f;

};