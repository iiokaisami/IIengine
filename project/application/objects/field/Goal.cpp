#include "Goal.h"

using namespace IIEngine;

void Goal::Initialize()
{
	// 基底クラス初期化
	StaticObject::Initialize("goal.obj", "Goal");

	position_ = { 0.0f,0.0f,-3.5f };
	scale_ = { 1.0f,1.0f,1.0f };
	SyncObjectTransform();
	// ライト設定
	object_->SetLighting(true);

	// バリア
	pBarrier_ = std::make_unique<Barrier>();
	pBarrier_->SetPosition(position_);
	pBarrier_->Initialize();
}

void Goal::Finalize()
{
	colliderManager_->DeleteCollider(&collider_);
	pBarrier_->Finalize();
}

void Goal::Update()
{
	StaticObject::Update();

	// バリアの更新
	pBarrier_->Update();

	// バリア破壊の状態が変わったときだけ伝える
	if (isBarrierDestroyed_ && !wasBarrierDestroyed_)
	{
		pBarrier_->SetBarrierDestroyed(true);
	}
	
	// 状態を保存
	wasBarrierDestroyed_ = isBarrierDestroyed_;

	if (isCleared_)
	{
		IIEngine::ParticleEmitter::Emit("goal", position_, 2);
	}

}

void Goal::Draw()
{
	object_->Draw();
	
	pBarrier_->Draw();
}

void Goal::ImGuiDraw()
{
}

void Goal::OnCollisionTrigger(const Collider* _other)
{
	if (_other->GetColliderID() == "Player" && isBarrierDestroyed_)
	{
		// プレイヤーがゴールに到達した場合クリア
		isCleared_ = true;
	}
}
