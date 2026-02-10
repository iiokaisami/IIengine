#include "ClearScene.h"

#include "TimeManager.h"

using namespace IIEngine;

void ClearScene::Initialize()
{
	

}

void ClearScene::Finalize()
{
}

void ClearScene::Update()
{

}

void ClearScene::Draw()
{
	// 描画前処理(Object)
	IIEngine::Object3dCommon::GetInstance()->CommonDrawSetting();

	

	// 描画前処理(Sprite)
	SpriteCommon::GetInstance()->CommonDrawSetting();

}
