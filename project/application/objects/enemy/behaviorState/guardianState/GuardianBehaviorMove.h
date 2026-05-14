#pragma once

#include "GuardianBehaviorState.h"

/// <summary>
/// ガーディアンの移動
/// </summary>
class GuardianBehaviorMove : public GuardianBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorMove(Guardian* _pGuardian);

	// デストラクタ
	~GuardianBehaviorMove() = default;

	// 初期化
	void Initialize() override;

	// 更新
	void Update() override;

	// モーションのカウント等をリセット
	void ResetMotion() override;

private:

	// モーションのカウント
	uint32_t moveCount_ = 0;

};

