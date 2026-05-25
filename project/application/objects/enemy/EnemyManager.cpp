#include "EnemyManager.h"

#include "waveState/EnemyWaveStage1.h"
#include "waveState/EnemyWaveStage2.h"

#include "TimeManager.h"

void EnemyManager::Initialize()
{
	// エディタ読み込み
	auto levelData = std::make_shared<IIEngine::LevelData>();
	levelData = IIEngine::LevelDataLoader::LoadLevelData("easyEnemySpawn");
	SetLevelData(levelData);

	// ウェーブステート
	ChangeState(std::make_unique<EnemyWaveStage1>(this));
	pState_->LoadTextureWave1();
}

void EnemyManager::Finalize()
{
	for (auto& enemy : pTrapEnemies_)
	{
		enemy->Finalize();
	}

	for (auto& enemy : pNormalEnemies_)
	{
		enemy->Finalize();
	}

	for(auto& corruptor : pCorruptors_)
	{
		corruptor->Finalize();
	}

	for (auto& guardian : pGuardians_)
	{
		guardian->Finalize();
	}

}

void EnemyManager::Update()
{
	// デルタタイム取得
	const float dt = IIEngine::TimeManager::Instance().GetDeltaTime();
	// アップデートするフレーム数計算
	const int framesThisUpdate = std::max(1, static_cast<int>(dt * updateFrameRateBase_ + 0.5f));

	// ノーマルエネミーの更新
	for (auto& enemy : pNormalEnemies_)
	{
		enemy->SetPlayerPosition(playerPosition_);
		enemy->Update();
		toPlayerDistance_.push_back(enemy->GetToPlayer());
	}

	// isDeath がたったら削除
	pNormalEnemies_.erase(
		std::remove_if(
			pNormalEnemies_.begin(),
			pNormalEnemies_.end(),
			[this](std::unique_ptr<NormalEnemy>& enemy)
			{
				if (enemy->IsDead())
				{
					enemy->Finalize();

					// 敵のカウントを減らす
					enemyCount_--;

					return true;
				}
				return false;
			}),
		pNormalEnemies_.end()
	);

	// トラップエネミーの更新
	for (auto& enemy : pTrapEnemies_)
	{
		enemy->SetPlayerPosition(playerPosition_);
		enemy->Update();
	}

	// isDead がたったら削除
	pTrapEnemies_.erase(
		std::remove_if(
			pTrapEnemies_.begin(),
			pTrapEnemies_.end(),
			[this](std::unique_ptr<TrapEnemy>& enemy)
			{
				if (enemy->IsDead())
				{
					// Finalize 前に残っている罠の位置を取得
					const auto& remainingTraps = enemy->GetRemainingTimeBombPositions();

					// 出現位置にCorruptorを生成
					for (const auto& trapPos : remainingTraps)
					{
						CorruptorInit(trapPos);
					}

					enemy->Finalize();
					// 敵のカウントを減らす
					enemyCount_--;
					return true;
				}
				return false;
			}),
		pTrapEnemies_.end()
	);

	// コラプターの更新
	for(auto& corruptor : pCorruptors_)
	{
		corruptor->SetPlayerPosition(playerPosition_);
		corruptor->Update();
	}

	// isDead がたったら削除
	pCorruptors_.erase(
		std::remove_if(
			pCorruptors_.begin(),
			pCorruptors_.end(),
			[this](std::unique_ptr<Corruptor>& corruptor)
			{
				if (corruptor->IsDead())
				{
					corruptor->Finalize();
					// 敵のカウントを減らす
					enemyCount_--;
					return true;
				}
				return false;
			}),
		pCorruptors_.end()
	);

	// ガーディアンの更新
	for (auto& guardian : pGuardians_)
	{
		guardian->SetPlayerPosition(playerPosition_);
		guardian->Update(); 
	}
	// isDead がたったら削除
	pGuardians_.erase(
		std::remove_if(
			pGuardians_.begin(),
			pGuardians_.end(),
			[this](std::unique_ptr<Guardian>& guardian)
			{
				if (guardian->IsDead())
				{
					guardian->Finalize();
					// 敵のカウントを減らす
					enemyCount_--;
					return true;
				}
				return false;
			}),
		pGuardians_.end()
	);


	// ウェーブステートの更新
	pState_->Update();
	
	// 敵が全て倒されたらウェーブチェンジ開始
	if (enemyCount_ == 0)
	{
		if (waveChangeInterval_ > 0)
		{
			isWaveChange_ = false;
			waveChangeInterval_ = std::max(0, waveChangeInterval_ - framesThisUpdate);
		} 
		else
		{
			// ステート遷移
			isWaveChange_ = true;
			waveChangeInterval_ = waveChangeIntervalResetFrames_;
		}
	}
}

void EnemyManager::Draw()
{
	for (auto& enemy : pNormalEnemies_)
	{
		enemy->Draw();
	}

	for (auto& enemy : pTrapEnemies_)
	{
		enemy->Draw();
	}

	for(auto& corruptor : pCorruptors_)
	{
		corruptor->Draw();
	}

	for(auto& guardian : pGuardians_)
	{
		guardian->Draw();
	}

}

void EnemyManager::Draw2D()
{
	for (auto& enemy : pNormalEnemies_)
	{
		enemy->Draw2D();
	}
	for (auto& enemy : pTrapEnemies_)
	{
		enemy->Draw2D();
	}
	for (auto& corruptor : pCorruptors_)
	{
		corruptor->Draw2D();
	}
	for (auto& guardian : pGuardians_)
	{
		guardian->Draw2D();
	}
	
	pState_->Draw2D();
	
}

void EnemyManager::ImGuiDraw()
{
#ifdef USE_IMGUI

	for (auto& enemy : pNormalEnemies_)
	{
		enemy->ImGuiDraw();
	}

	for (auto& enemy : pTrapEnemies_)
	{
		enemy->ImGuiDraw();
	}

	for (auto& guardian : pGuardians_)
	{
		guardian->ImGuiDraw();
	}

#endif // USE_IMGUI
}

void EnemyManager::TitleEnemyInit()
{
	NormalEnemyInit(titleEnemySpawnPos_[0]);
	TrapEnemyInit(titleEnemySpawnPos_[1]);
	NormalEnemyInit(titleEnemySpawnPos_[2]);
	TrapEnemyInit(titleEnemySpawnPos_[3]);
}

void EnemyManager::TitleEnemyUpdate()
{
	// ノーマルエネミーの更新
	for (auto& enemy : pNormalEnemies_)
	{
		enemy->SetPlayerPosition(playerPosition_);
		enemy->Update();
		toPlayerDistance_.push_back(enemy->GetToPlayer());
	}

	// isDeath がたったら削除
	pNormalEnemies_.erase(
		std::remove_if(
			pNormalEnemies_.begin(),
			pNormalEnemies_.end(),
			[this](std::unique_ptr<NormalEnemy>& enemy)
			{
				if (enemy->IsDead())
				{
					enemy->Finalize();

					// 敵のカウントを減らす
					enemyCount_--;

					return true;
				}
				return false;
			}),
		pNormalEnemies_.end()
	);

	// トラップエネミーの更新
	for (auto& enemy : pTrapEnemies_)
	{
		enemy->SetPlayerPosition(playerPosition_);
		enemy->Update();
	}

	// isDead がたったら削除
	pTrapEnemies_.erase(
		std::remove_if(
			pTrapEnemies_.begin(),
			pTrapEnemies_.end(),
			[this](std::unique_ptr<TrapEnemy>& enemy)
			{
				if (enemy->IsDead())
				{
					enemy->Finalize();
					// 敵のカウントを減らす
					enemyCount_--;
					return true;
				}
				return false;
			}),
		pTrapEnemies_.end()
	);

	// 敵が全て倒されたらもう一度出現
	if (enemyCount_ == 0)
	{
		TitleEnemyInit();
	}
}

void EnemyManager::GameOverEnemyInit()
{
	NormalEnemyInit(gameOverEnemySpawnPos_);
}

void EnemyManager::GameOverEnemyUpdate()
{
	// ノーマルエネミーの更新
	for (auto& enemy : pNormalEnemies_)
	{
		enemy->SetPlayerPosition(playerPosition_);
		enemy->Update();
		toPlayerDistance_.push_back(enemy->GetToPlayer());
	}

	// isDeath がたったら削除
	pNormalEnemies_.erase(
		std::remove_if(
			pNormalEnemies_.begin(),
			pNormalEnemies_.end(),
			[this](std::unique_ptr<NormalEnemy>& enemy)
			{
				if (enemy->IsDead())
				{
					enemy->Finalize();

					// 敵のカウントを減らす
					enemyCount_--;

					return true;
				}
				return false;
			}),
		pNormalEnemies_.end()
	);

	// 敵が全て倒されたらもう一度出現
	if (enemyCount_ == 0)
	{
		GameOverEnemyInit();
	}
}

void EnemyManager::NormalEnemyInit(const Vector3& pos)
{
	// ノーマルエネミー
	std::unique_ptr<NormalEnemy> enemy = std::make_unique<NormalEnemy>();
	enemy->SetPosition(pos);
	enemy->Initialize();
	enemy->SetPlayerPosition(playerPosition_);
	// obstacles_が空でないなら障害物をセット
	if (!obstacles_.empty())
	{
		enemy->SetObstaclePositions(obstacles_);
	}
	enemy->Update();

	// 敵を登録
	pNormalEnemies_.push_back(std::move(enemy));

	// 敵のカウントを増やす
	enemyCount_++;
}

void EnemyManager::TrapEnemyInit(const Vector3& pos)
{
	// トラップエネミー
	std::unique_ptr<TrapEnemy> trapEnemy = std::make_unique<TrapEnemy>();
	trapEnemy->SetPosition(pos);
	trapEnemy->Initialize();
	trapEnemy->SetPlayerPosition(playerPosition_);
	trapEnemy->Update();

	// 敵を登録
	pTrapEnemies_.push_back(std::move(trapEnemy));

	// 敵のカウントを増やす
	enemyCount_++;
}

void EnemyManager::CorruptorInit(const Vector3& pos)
{
	// コラプター
	std::unique_ptr<Corruptor> corruptor = std::make_unique<Corruptor>();
	corruptor->SetPosition(pos);
	corruptor->Initialize();
	corruptor->SetPlayerPosition(playerPosition_);
	corruptor->Update();

	// 敵を登録
	pCorruptors_.push_back(std::move(corruptor));
	
	// 敵のカウントを増やす
	enemyCount_++;
}

void EnemyManager::GuardianInit(const Vector3& pos)
{
	// ガーディアン
	std::unique_ptr<Guardian> guardian = std::make_unique<Guardian>();
	guardian->SetPosition(pos);
	guardian->Initialize();
	guardian->SetPlayerPosition(playerPosition_);
	guardian->Update();

	// 敵を登録
	pGuardians_.push_back(std::move(guardian));

	// 敵のカウントを増やす
	enemyCount_++;
}

void EnemyManager::ChangeState(std::unique_ptr<EnemyWaveState> _pState)
{
	pState_ = std::move(_pState);
	pState_->Initialize();
}

bool EnemyManager::GetNearestEnemyPosition(const Vector3& from, Vector3& outPos) const
{
	// 最も近い敵の位置を探す
	bool found = false;
	float bestDist2 = FLT_MAX;

	// コンテナ内の敵を検討するラムダ関数
	auto consider = [&](const auto& container)
		{
			// 各敵を検討
			for (const auto& ePtr : container)
			{
				// 敵が存在しない場合はスキップ
				if (!ePtr)
				{
					continue;
				}

				// 敵が死んでいる場合はスキップ
				if (ePtr->IsDead())
				{
					continue;
				}

				// 距離の二乗を計算
				Vector3 pos = ePtr->GetPosition();
				float dx = pos.x - from.x;
				float dy = pos.y - from.y;
				float dz = pos.z - from.z;
				float d2 = dx * dx + dy * dy + dz * dz;

				// 最小距離を更新
				if (d2 < bestDist2)
				{
					bestDist2 = d2;
					outPos = pos;
					found = true;
				}
			}
		};

	// 各コンテナを検討
	consider(pNormalEnemies_);
	consider(pTrapEnemies_);
	consider(pCorruptors_);

	return found;
}
