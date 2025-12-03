#include "EnemyManager.h"

#include "waveState/EnemyWaveStage1.h"
#include "waveState/EnemyWaveStage2.h"

#include "TimeManager.h"

void EnemyManager::Initialize()
{
	// エディタ読み込み
	auto levelData = std::make_shared<LevelData>();
	levelData = LevelDataLoader::LoadLevelData("enemySpawn");
	SetLevelData(levelData);

	// ウェーブステート
	ChangeState(std::make_unique<EnemyWaveStage1>(this));
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
}

void EnemyManager::Update()
{
	// デルタタイム取得
	const float dt = TimeManager::Instance().GetDeltaTime();
	// アップデートするフレーム数計算
	const int framesThisUpdate = std::max(1, static_cast<int>(dt * 60.0f + 0.5f));

	// ノーマルエネミーの更新
	for (auto& enemy : pNormalEnemies_)
	{
		enemy->SetPlayerPosition(playerPosition_);
		enemy->Update();
		toPlayerDistance_.push_back(enemy->GetToPlayer());
	}

	// isDeat がたったら削除
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
					std::vector<Vector3> remainingTraps = enemy->GetRemainingTimeBombPositions();

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
			waveChangeInterval_ = 120;
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

#endif // USE_IMGUI
}

void EnemyManager::TitleEnemyInit()
{
	NormalEnemyInit({ 2.0f,0.5f,-5.0f });
	TrapEnemyInit({ -2.0f,0.5f,2.0f });
	NormalEnemyInit({ -5.0f,0.5f,-2.0f });
	TrapEnemyInit({ 5.0f,0.5f,5.0f });
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

	// isDeat がたったら削除
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
	NormalEnemyInit({ -20.0f,0.5f,0.0f });
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

	// isDeat がたったら削除
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

void EnemyManager::ChangeState(std::unique_ptr<EnemyWaveState> _pState)
{
	pState_ = std::move(_pState);
	pState_->Initialize();
}
