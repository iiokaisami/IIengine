#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Vector3.h>

#include "../../gameEngine/baseScene/BaseScene.h"
#include "../../gameEngine/particle/ParticleEmitter.h"
#include "cameraController/SceneCamera.h"
#include "cameraController/ShakeController.h"
#include "cameraController/FollowController.h"

#include "../../application/objects/player/Player.h"
#include "../../application/objects/enemy/EnemyManager.h"
#include "../../application/objects/field/Field.h"

#include "../../gameEngine/transition/BlockRiseTransition.h"

/// <summary>
/// タイトルシーン
/// タイトルロゴ表示、エフェクト、BGM再生、シーン遷移
/// </summary>
class TitleScene : public IIEngine::BaseScene
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

	// 常に最前面に描きたいもの(トランジションなど)
	void DrawOverlayTop() override;

private:

	// カメラ
	IIEngine::CameraManager& cameraManager = IIEngine::CameraManager::GetInstance();
	std::unique_ptr<SceneCamera> camera_ = nullptr;
	Vector3 cameraPosition_{};
	Vector3 cameraRotate_{};
	// 持続時間
	float duration_ = 0.3f; 
	// 振動の強さ
	float amplitude_ = 0.5f;

	// 遷移
	std::unique_ptr<IIEngine::BlockRiseTransition> transition_ = nullptr;
	bool isTransitioning_ = false;

	// 2Dスプライト
	std::vector<std::unique_ptr<IIEngine::Sprite>>sprites_ = {};
	// 描画するスプライトの数
	uint32_t spriteNum_ = 2;

	// 衝突判定
	IIEngine::ColliderManager* colliderManager_ = nullptr;

	// プレイヤー
	std::unique_ptr<Player> pPlayer_ = nullptr;

	// エネミー
	std::unique_ptr<EnemyManager> pEnemyManager_ = nullptr;

	// フィールド
	std::unique_ptr<Field> pField_ = nullptr;


	// 透明チェック
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };


	bool isTerrainDraw = false;
	bool enableLighting = false;
	bool enableDirectionalLight = false;
	bool enablePointLight = false;
	bool enableSpotLight = false;
	bool enableEnvironment = false;

	// ライトの詳細設定用変数
	Vector3 directionalLightColor = { 1.0f, 1.0f, 1.0f };
	Vector3 directionalLightDirection = { 0.0f, -1.0f, 0.0f };
	float directionalLightIntensity = 1.0f;

	Vector3 pointLightColor = { 1.0f, 1.0f, 1.0f };
	Vector3 pointLightPosition = { 0.0f, 2.0f, 0.0f };
	float pointLightIntensity = 1.0f;
	float pointLightRadius = 10.0f;
	float pointLightDecay = 1.0f;

	Vector3 spotLightColor = { 1.0f, 1.0f, 1.0f };
	Vector3 spotLightPosition = { 2.0f, 1.25f, 0.0f };
	Vector3 spotLightDirection = Normalize({ 1.0f, 1.0f, 0.0f });
	float spotLightIntensity = 4.0f;
	float spotLightDistance = 7.0f;
	float spotLightDecay = 2.0f;
	float spotLightConsAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
	float spotLightCosFalloffStart = 1.0f;

	// 環境マップ
    std::string cubeMapPath_ = "";
	uint32_t cubeSrvIndex_ = 0u;
	D3D12_GPU_DESCRIPTOR_HANDLE cubeHandle_ = { 0 };
	float environmentStrength_ = 1.0f;


	// スプライト用変数
	float time_ = 0.0f;

};