#include "GameOverScene.h"

#include <Ease.h>

#include "TimeManager.h"

using namespace IIEngine;

void GameOverScene::Initialize()
{

}

void GameOverScene::Finalize()
{

}

void GameOverScene::Update()
{
	

}

void GameOverScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	
	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

}