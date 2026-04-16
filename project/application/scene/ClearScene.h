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
#include "../objects/field/Field.h"
#include "cameraController/SceneCamera.h"

/// <summary>
/// クリアシーン
/// クリア表示、エフェクト、BGM再生、シーン遷移
/// </summary>
class ClearScene : public IIEngine::BaseScene
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

	IIEngine::CameraManager& cameraManager = IIEngine::CameraManager::GetInstance();
	std::unique_ptr<SceneCamera> camera_ = nullptr;
	Vector3 cameraPosition_ = { 0.0f,12.0f,-33.0f };
	Vector3 cameraRotate_ = { 0.3f,0.0f,0.0f };

	// 2Dスプライト
	std::vector<std::unique_ptr<IIEngine::Sprite>>sprites_ = {};
	// 描画するスプライトの数
	uint32_t spriteNum_ = 5;

	Vector2 rePos_ = { 0,620 };
	Vector2 toPos_ = { 0,620 };


	// スプライト用変数
	float time_ = 0.0f;
	float uiOffset_ = 760.0f;

	Vector4 reColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 toColor_ = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 uiColor_ = { 1.0f,1.0f,1.0f,1.0f };

	// 選択中表示位置
	Vector2 reBasePos_ = { 0,620 };
	Vector2 toBasePos_ = { 0,620 };
	// 遷移
	std::unique_ptr<IIEngine::BlockRiseTransition> transition_ = nullptr;
	bool isTransitioning_ = false;

	// プレイヤー
	std::unique_ptr<Player> pPlayer_ = nullptr;

	// フィールド
	std::unique_ptr<Field> pField_ = nullptr;

	// パーティクル位置
	Vector3 particlePosition_ = { 0.0f,0.0f,-3.5f };
	Vector3 petalPosition_ = { 0.0f,12.0f,-18.0f };

	// 次のシーン選択肢
	enum class MenuOption
	{
		None,
		Retry,      // GAMEPLAY
		Title       // TITLE
	};

	// Open直後の決定入力を無視する用
	bool ignoreConfirm_ = false;     
	uint32_t selectedIndex_ = 0;     // 0: Retry, 1: Title
	MenuOption selectedOption_ = MenuOption::None;

	// スティック判定用
	float prevStickX_ = 0.0f;
	bool leftPadStick_ = false;
	bool rightPadStick_ = false;

	// 項目ごとのタイマー
	float retrySwayTimer_ = 0.0f;
	float titleSwayTimer_ = 0.0f;

	// 最後に計算したオフセットを保持
	float retrySwayOffsetX_ = 0.0f;
	float titleSwayOffsetX_ = 0.0f;
	
};

