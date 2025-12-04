#pragma once

#include "Framework.h"
#include "SceneFactory.h"

/// <summary>
/// ゲーム固有のフレームワーク
/// ゲーム固有の初期化、終了、更新、描画を行う
/// </summary>
class MyGame : public Framework
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

	// モデル読み込み
	void LoadModel();

	// サウンド読み込み
	void LoadSound();

	// サウンド
	SoundData soundData_;
	SoundData soundData2_;

private:

	bool useExampleGroup_ = true;

	// 0 : グレースケール無効, 1: グレースケール有効
	uint32_t useGrayscale_ = 0;

	// Vignetteの強度
	float vignetteRadius_ = 0.5f;

	float time = 0.0f;
	bool isPetal_ = false;
	bool isHoming_ = false;
	bool isFlame_ = false;
	bool isExplosion_ = false;
};