#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Vector3.h>

#include "../../gameEngine/baseScene/BaseScene.h"
#include "../../gameEngine/transition/BlockRiseTransition.h"
#include "../../application/objects/player/Player.h"
#include "../../application/objects/enemy/EnemyManager.h"
#include "../../gameEngine/collider/ColliderManager.h"

/// <summary>
/// ゲームオーバーシーン
/// ゲームオーバー表示、エフェクト、BGM再生、シーン遷移
/// </summary>
class GameOverScene : public BaseScene
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

	// カメラシェイク
	void CameraShake();

private:

	CameraManager& cameraManager = CameraManager::GetInstance();
	std::shared_ptr<Camera> camera_ = std::make_shared<Camera>();
	Vector3 cameraPosition_{};
	Vector3 cameraRotate_{};

	// 2Dスプライト
	std::vector<std::unique_ptr<Sprite>>sprites_ = {};
	// 描画するスプライトの数
	uint32_t spriteNum_ = 3;

	// 遷移
	std::unique_ptr<BlockRiseTransition> transition_ = nullptr;
	bool isTransitioning_ = false;

	// 衝突判定
	ColliderManager* colliderManager_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> pPlayer_ = nullptr;

	// エネミー
	std::unique_ptr<EnemyManager> pEnemyManager_ = nullptr;

};

