#pragma once

#include "GuardianBehaviorState.h"

/// <summary>
/// ガーディアンのヒットリアクション
/// </summary>
class GuardianBehaviorHitReact : public GuardianBehaviorState
{
public:
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorHitReact(Guardian* _pGuardian);
	
	// デストラクタ
	~GuardianBehaviorHitReact() = default;
	
	// 初期化
	void Initialize() override;
	
	// 更新
	void Update() override;

	// モーションのカウント等をリセット
	void ResetMotion() override;

private:

	// モーションのカウント
	uint32_t hitReactCount_ = 0;

};