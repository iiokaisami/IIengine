#pragma once

#include "GuardianBehaviorState.h"

/// <summary>
/// ガーディアンの出現
/// </summary>
class GuardianBehaviorSpawn : public GuardianBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorSpawn(Guardian* _pGuardian);
	
	// デストラクタ
	~GuardianBehaviorSpawn() = default;
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;
	
	// モーションのカウント等をリセット
	void ResetMotion() override;

private:

	// モーションのカウント
	uint32_t spawnCount_ = 30;

};

