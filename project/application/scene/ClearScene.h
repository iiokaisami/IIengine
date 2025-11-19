#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Vector3.h>

#include "../../gameEngine/baseScene/BaseScene.h"
#include "../../gameEngine/transition/BlockRiseTransition.h"
#include "../../gameEngine/particle/ParticleEmitter.h"
#include "../objects/player/Player.h"

/// <summary>
/// クリアシーン
/// クリア表示、エフェクト、BGM再生、シーン遷移
/// </summary>
class ClearScene : public BaseScene
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

	CameraManager& cameraManager = CameraManager::GetInstance();
	std::shared_ptr<Camera> camera_ = std::make_shared<Camera>();
	Vector3 cameraPosition_{};
	Vector3 cameraRotate_{};

	// 2Dスプライト
	std::vector<Sprite*>sprites = {};

	// 遷移
	std::unique_ptr<BlockRiseTransition> transition_ = nullptr;
	bool isTransitioning_ = false;

	// プレイヤー
	std::unique_ptr<Player> pPlayer_ = nullptr;

	// パーティクル位置
	Vector3 particlePosition_ = { 0.0f,-3.5f,-3.5f };
	Vector3 petalPosition_ = { 0.0f,12.0f,-18.0f };

};

