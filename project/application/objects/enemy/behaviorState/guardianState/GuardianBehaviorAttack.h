#pragma once

#include "GuardianBehaviorState.h"

class GuardianBehaviorAttack : public GuardianBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorAttack(Guardian* _pGuardian);
	
	// デストラクタ
	~GuardianBehaviorAttack() = default;
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのカウント等をリセット
	void ResetMotion() override;

private:

	// モーションのカウント
	uint32_t attackCount_ = 0;

};

