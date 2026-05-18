#include "BulletManager.h"

void BulletManager::AddBullet(std::unique_ptr<BaseBullet> _bullet)
{
	_bullet->Initialize();
    bullets_.emplace_back(std::move(_bullet));
}

void BulletManager::UpdateAll()
{
    for (auto& bullet : bullets_)
    {
        if (bullet) bullet->Update();
    }

    // 消滅した弾はFinalize後erase
    for (auto& bullet : bullets_) 
    {
        if (bullet && bullet->IsDead()) 
        {
            bullet->Finalize();
        }
    }

	// 消滅した弾をリストから削除
    bullets_.erase(
        std::remove_if(bullets_.begin(), bullets_.end(),
            [](const std::unique_ptr<BaseBullet>& b) { return b->IsDead(); }),
        bullets_.end());
}

void BulletManager::DrawAll()
{
    for (auto& bullet : bullets_) { bullet->Draw(); }
}
