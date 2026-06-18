#pragma once

#include "GuardianBehaviorState.h"

/// <summary>
/// ガーディアンのジャンプ
/// </summary>
class GuardianBehaviorJump : public GuardianBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorJump(Guardian* _pGuardian);
	
	// デストラクタ
	~GuardianBehaviorJump() = default;
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;
	
	// モーションのカウント等をリセット
	void ResetMotion() override;


private:

	// モーションのカウント
	uint32_t moveCount_ = 30;

};

