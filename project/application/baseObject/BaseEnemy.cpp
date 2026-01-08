#include "BaseEnemy.h"

#include "TimeManager.h"
#include "CameraManager.h"
#include "MyMath.h"

void BaseEnemy::Initialize(const std::string& _modelFileName, const std::string& _objectName, float _initialHP)
{
	// キャラクター基底クラスの初期化を呼び出し
	Character::Initialize(_modelFileName, _objectName, _initialHP);
    
    moveVelocity_ = { 0.1f, 0.0f, 0.1f };
    moveSpeed_ = 0.05f;

}

void BaseEnemy::Update()
{
    
	Character::Update();
}

void BaseEnemy::Move() 
{
    // デルタタイム取得
    const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();

    // プレイヤー方向ベクトル計算
    toPlayer_ = playerPosition_ - position_;
    moveVelocity_ = Normalize(toPlayer_);  // 基本ではプレイヤーに向かう

    // Y軸回転を進行方向に合わせる
    rotation_.y = std::atan2(moveVelocity_.x, moveVelocity_.z);

    // デフォルトの位置更新
    position_ += moveVelocity_ * moveSpeed_ * dt * kDefaultFrameRate;

}

Vector3 BaseEnemy::InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate)
{
    return Normalize(Slerp(currentVelocity, targetDirection, interpolationRate));
}
