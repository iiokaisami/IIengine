#pragma once

#include "GuardianBehaviorState.h"

class GuardianBehaviorDead : public GuardianBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorDead(Guardian* _pGuardian);
	
	// デストラクタ
	~GuardianBehaviorDead() = default;
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのカウント等をリセット
	void ResetMotion() override;

private:

	// モーションのカウント
	uint32_t deadCount_ = 0;

};