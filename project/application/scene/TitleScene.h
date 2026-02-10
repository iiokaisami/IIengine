#pragma once

#include <Framework.h>
#include <vector>
#include <memory>
#include <MyMath.h>
#include <Object3d.h>
#include <Vector3.h>

#include "../../gameEngine/baseScene/BaseScene.h"
#include "cameraController/SceneCamera.h"

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

private:

	// カメラ
	IIEngine::CameraManager& cameraManager = IIEngine::CameraManager::GetInstance();
	std::unique_ptr<SceneCamera> camera_ = nullptr;
	Vector3 cameraPosition_{};
	Vector3 cameraRotate_{};

	// 2Dスプライト
	std::vector<std::unique_ptr<IIEngine::Sprite>>sprites_ = {};
	// 描画するスプライトの数
	uint32_t spriteNum_ = 3;

	// 透明チェック
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

};