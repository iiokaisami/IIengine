#include "EnemyWaveState.h"

#include "../EnemyManager.h"
#include <TimeManager.h>
#include "postEffect/PostEffectManager.h" // 追加: PostEffectManager の正しいインクルード

#include <cassert>

using namespace IIEngine;

EnemyWaveState::~EnemyWaveState()
{
}

void EnemyWaveState::LoadTextureWave1()
{
    for (uint32_t i = 0; i < spriteIndex_; i++)
    {
        auto sprite = std::make_unique<IIEngine::Sprite>();

        if (i == 0)
        {

            sprite->Initialize("wave.png", spritePos_, {1.0f,1.0f,1.0f,1.0f}, {0,0});
        }
        if (i == 1)
        {
            sprite->Initialize("1wave.png", spritePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
        }

        waveStartSprites_.push_back(std::move(sprite));
    }
}

void EnemyWaveState::LoadTextureWave2()
{
    for (uint32_t i = 0; i < spriteIndex_; i++)
    {
        auto sprite = std::make_unique<IIEngine::Sprite>();

        if (i == 0)
        {

            sprite->Initialize("wave.png", spritePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
        }
        if (i == 1)
        {
            sprite->Initialize("2wave.png", spritePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
        }

        waveStartSprites_.push_back(std::move(sprite));
    }
}

void EnemyWaveState::LoadTextureWave3()
{
    for (uint32_t i = 0; i < spriteIndex_; i++)
    {
        auto sprite = std::make_unique<IIEngine::Sprite>();
        
        if (i == 0)
        {
            sprite->Initialize("lastWave.png", spritePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
        }
        
        if (i == 1)
        {
            sprite->Initialize("3wave.png", spritePos_, { 1.0f,1.0f,1.0f,1.0f }, { 0,0 });
        }

        waveStartSprites_.push_back(std::move(sprite));
	}
}

void EnemyWaveState::UpdateTexture()
{
    // タイマーに応じて x を線形補間で -1000 まで移動させる
    if (showWaveStartSprite_)
    {
        // 表示開始時の初期値をキャプチャ
        if (!waveStartSpriteInit_)
        {
            waveStartSpriteInitialX_ = spritePos_.x;
            waveStartSpriteInitialTimer_ = waveStartSpriteTimer_;
            waveStartSpriteInit_ = true;
        }

        // 補間
        if (waveStartSpriteInitialTimer_ > 0)
        {
            float progress = 1.0f - static_cast<float>(waveStartSpriteTimer_) / static_cast<float>(waveStartSpriteInitialTimer_);
            if (progress < 0.0f) progress = 0.0f;
            if (progress > 1.0f) progress = 1.0f;

            const float targetX = -1000.0f;
            spritePos_.x = waveStartSpriteInitialX_ + (targetX - waveStartSpriteInitialX_) * progress;
        }

        // タイマーが0になったらスプライトを非表示にして初期化フラグをリセット
        if (waveStartSpriteTimer_ <= 0)
        {
            showWaveStartSprite_ = false;
            waveStartSpriteInit_ = false;
        }

        // タイマーを減らす
        waveStartSpriteTimer_--;

        for (auto& sprite : waveStartSprites_)
        {
            sprite->SetPosition(spritePos_);
            sprite->Update();
        }
    }
}

void EnemyWaveState::EffectIntro()
{
    waveIntroTimer_ = 0.0f;
    isWaveIntro_ = true;
    isPulseTriggered_ = false;

    IIEngine::PostEffectManager::GetInstance()->SetActiveEffect("Scanline", true);
    IIEngine::PostEffectManager::GetInstance()->SetActiveEffect("ChromaticPulse", true);

}

void EnemyWaveState::EffectUpdate()
{
    float dt = TimeManager::Instance().GetDeltaTime();

    if (isWaveIntro_)
    {
        waveIntroTimer_ += dt;

        float t = waveIntroTimer_ / 3.0f; // 0〜1
        t = std::clamp(t, 0.0f, 1.0f);

        auto* scanline = PostEffectManager::GetInstance()->GetPassAs<ScanlinePass>("Scanline");

        auto* pulse = PostEffectManager::GetInstance()->GetPassAs<ChromaticPulsePass>("ChromaticPulse");

        // 最初は静か
        if (t < 0.5f)
        {
            scanline->SetIntensity(t * 0.4f);
        }
        // 後半で溜め
        else if (t < 0.75f)
        {
            float local = (t - 0.5f) / 0.25f;
            scanline->SetIntensity(0.4f + local * 0.6f);
        }
        // ラスト0.15秒でPulse発動
        else
        {
            if (!isPulseTriggered_)
            {
                pulse->TriggerPulse();
                isPulseTriggered_ = true;
            }
        }

        if (waveIntroTimer_ >= 3.0f)
        {
            isWaveIntro_ = false;
            isPulseTriggered_ = false;

            PostEffectManager::GetInstance()->SetActiveEffect("Scanline", false);
			PostEffectManager::GetInstance()->SetActiveEffect("ChromaticPulse", false);
        }
    }
}


void EnemyWaveState::LoadCSV(const std::string& csvPath)
{
	// CSVファイルの読み込み
	//ファイルを開く
	std::ifstream file(csvPath);
	assert(file.is_open());

	//ファイルの内容を文字列ストリームにコピー
	enemyPopCommands_ << file.rdbuf();

	//ファイルを閉じる
	file.close();
}

void EnemyWaveState::UpdateEnemyPopCommands(EnemyManager* _pEnemyManager)
{
	enemyWaitingTimer_++; // 毎フレームカウント

    for (auto& enemy : levelData_->enemies)
    {
        if (!enemy.isSpawned && enemy.waveNum == currentWave_ && enemyWaitingTimer_ >= enemy.spawnDelay)
        {
            // 敵を出現
            _pEnemyManager->NormalEnemyInit(enemy.position);
            enemy.isSpawned = true;

            // タイマーをリセットして、次の敵の delayカウント開始
            enemyWaitingTimer_ = 0;

            // 1フレームに複数出さないように break
            break;
        }
    }

    for (auto& trapEnemy : levelData_->trapEnemies)
    {
        if (!trapEnemy.isSpawned && trapEnemy.waveNum == currentWave_ && enemyWaitingTimer_ >= trapEnemy.spawnDelay)
        {
            // 敵を出現
            _pEnemyManager->TrapEnemyInit(trapEnemy.position);
            trapEnemy.isSpawned = true;
           
            // タイマーをリセットして、次の敵の delayカウント開始
            enemyWaitingTimer_ = 0;
            
            // 1フレームに複数出さないように break
            break;
        }
	}
}

void EnemyWaveState::UpdateCSV(EnemyManager* _pEnemyManager)
{
    //待機処理
    if (isEnemyWaiting_) 
    {
        if (debugCount_ > 0)
        {
            debugCount_--;
        }

    	if (debugCount_ == 0)
    	{
    		isEnemyWaiting_ = false;
    	}
    	return;
    }

    std::string line;

    while (getline(enemyPopCommands_, line)) 
    {
    	std::istringstream line_stream(line);
    	std::string word;

    	getline(line_stream, word, ','); // コマンド取得

    	// コメント行をスキップ
    	if (word.find("//") == 0)
    	{
    		continue;
    	}

    	// POSITONコマンド（NormalEnemy or TrapEnemy）
    	if (word == "POSITION")
    	{

    		// 敵タイプ（例: "Normal", "Trap"）
    		std::string enemyType;
    		getline(line_stream, enemyType, ',');

    		// X, Y, Z 座標を取得
    		getline(line_stream, word, ',');
    		float x = static_cast<float>(atof(word.c_str()));

    		getline(line_stream, word, ',');
    		float y = static_cast<float>(atof(word.c_str()));

    		getline(line_stream, word, ',');
    		float z = static_cast<float>(atof(word.c_str()));

    		if (enemyType == "Normal")
    		{
    			_pEnemyManager->NormalEnemyInit({ x, y, z });
    		} 
    		else if (enemyType == "Trap") 
    		{
    			_pEnemyManager->TrapEnemyInit({ x, y, z });
    		}
    	}
    	// WAITコマンド
    	else if (word == "WAIT")
    	{
    		getline(line_stream, word, ',');
    		int waitTime = atoi(word.c_str());

    		isEnemyWaiting_ = true;
            debugCount_ = waitTime;
    		break;
    	}
    }
}
