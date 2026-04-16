#include "Goal.h"

using namespace IIEngine;

void Goal::Initialize()
{
	// 基底クラス初期化
	StaticObject::Initialize("goal.obj", "Goal");

	position_ = goalInitialPos_;
	scale_ = goalInitialScale_;
	rotation_ = { tiltAngleRad_, 0.0f, 0.0f };
	SyncObjectTransform();
	// ライト設定
	object_->SetLighting(true);

	bobTimeSec_ = 0.0f;

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

	// 見た目の演出 回転 + 上下
	const float dt = TimeManager::Instance().GetDeltaTime();

	// y軸回転
	rotation_.y += rotateSpeedRadPerSec_ * dt;

	// 上下
	bobTimeSec_ += dt;
	position_.y = goalInitialPos_.y + std::sinf(bobTimeSec_ * bobFreqRadPerSec_) * bobAmplitude_;

	// 斜め固定
	rotation_.x = tiltAngleRad_;

	SyncObjectTransform();

	if (isCleared_)
	{
		IIEngine::ParticleEmitter::Emit("goal", position_, clearedParticleCount_);
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
