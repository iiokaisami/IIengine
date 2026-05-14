#include "BulletManager.h"

void BulletManager::AddBullet(std::unique_ptr<BaseBullet> _bullet)
{
    bullets_.emplace_back(std::move(_bullet));
}

void BulletManager::UpdateAll()
{
    for (auto& bullet : bullets_) { bullet->Update(); }
    // isDead等で消す処理もここ
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](const std::unique_ptr<BaseBullet>& b) { return b->IsDead(); }),
        bullets_.end());
}

void BulletManager::DrawAll()
{
    for (auto& bullet : bullets_) { bullet->Draw(); }
}
