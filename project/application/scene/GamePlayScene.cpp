#include "GamePlayScene.h"

#include <ModelManager.h>
#include <Ease.h>
#include <corecrt_math_defines.h>

#include "TimeManager.h"

using namespace IIEngine;

void GamePlayScene::Initialize()
{
	

}

void GamePlayScene::Finalize()
{
	
}

void GamePlayScene::Update()
{
	
}

void GamePlayScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

	
}
