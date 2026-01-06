#include "BaseEnemy.h"

#include "TimeManager.h"
#include "CameraManager.h"
#include "MyMath.h"

void BaseEnemy::Initialize(const std::string& objectName, const Vector3& scale, float initialHP)
{
    objectName_ = objectName;
    scale_ = scale;
    hp_ = initialHP;

    isDead_ = false;
    moveVelocity_ = { 0.1f, 0.0f, 0.1f };
    moveSpeed_ = 0.05f;

    // 最大HP保存
    maxHP_ = hp_;
}

void BaseEnemy::Update()
{

}

void BaseEnemy::Move() 
{
    // デルタタイム取得
    const float dt = TimeManager::Instance().GetDeltaTime();

    // プレイヤー方向ベクトル計算
    toPlayer_ = playerPosition_ - position_;
    moveVelocity_ = Normalize(toPlayer_);  // 基本ではプレイヤーに向かう

    // Y軸回転を進行方向に合わせる
    rotation_.y = std::atan2(moveVelocity_.x, moveVelocity_.z);

    // デフォルトの位置更新
    position_ += moveVelocity_ * moveSpeed_ * dt * kDefaultFrameRate;

}

void BaseEnemy::UpdateHPBar()
{
    auto camera = CameraManager::GetInstance().GetActiveCamera();

    if (!camera or sprites_.empty())
    {
        return;
    }

    // 目標HPを計算
    float targetRatio = 1.0f;
    if (maxHP_ > 0.0f) targetRatio = std::clamp(hp_ / maxHP_, 0.0f, 1.0f);

    // 補間
    const float dt = TimeManager::Instance().GetDeltaTime();
    // 補間係数
    float t = std::clamp(hpLerpSpeed_ * dt, 0.0f, 1.0f);
    hpRatio_ += (targetRatio - hpRatio_) * t;

    // スクリーン位置計算
    Vector3 worldCenter = position_ + hpBarOffset_;
    Vector2 screenCenter = camera->WorldToScreen(worldCenter);

    // 左寄せに合わせて左端位置を決める
    float baseWidth = hpBarSize_.x;
    Vector2 leftPos = screenCenter;
    leftPos.x = screenCenter.x - (baseWidth * 0.5f);

    // 幅を決める
    Vector2 newSize = hpBarSize_;
    newSize.x = baseWidth * hpRatio_;

    // 反映
    sprites_[0]->SetPosition(leftPos);
    sprites_[0]->SetSize(newSize);
    sprites_[0]->Update();
}

Vector3 BaseEnemy::InterpolateMovement(const Vector3& currentVelocity, const Vector3& targetDirection, float interpolationRate)
{
    return Normalize(Slerp(currentVelocity, targetDirection, interpolationRate));
}
