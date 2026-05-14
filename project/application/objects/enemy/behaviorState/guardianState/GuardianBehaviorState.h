#pragma once

#include <sstream>
#include <Vector3.h>

// 前方宣言
class Guardian;

/// <summary>
/// Guardianの行動ステート基底クラス
/// </summary>
class GuardianBehaviorState
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_state">ステート名</param>
	/// <param name="_pGuardian">ガーディアンポインタ</param>
	GuardianBehaviorState(const std::string& _state, Guardian* _pGuardian) :stateName_(_state), pGuardian_(_pGuardian) {};
	virtual ~GuardianBehaviorState();

	// 初期化
	virtual void Initialize() = 0;

	// 更新
	virtual void Update() = 0;

	// モーションのカウント等をリセット
	virtual void ResetMotion() = 0;

protected:

	struct Transform
	{
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		Vector3 rotation = { 0.0f, 0.0f, 0.0f };
		Vector3 scale = { 1.0f, 1.0f, 1.0f };
	};

	struct Motion
	{
		// モーションがアクティブかどうか
		bool isActive = false;
		// カウント
		uint32_t count = 0;
		// Maxカウント
		uint32_t maxCount = 30;
		// トランスフォーム
		Transform transform;
	};

	/// <summary>
	/// モーションのカウント
	/// </summary>
	/// <param name="_motion">モーション構造体</param>
	void MotionCount(Motion& _motion);

	/// <summary>
	/// 敵のトランスフォームを motion_.transformにセット
	/// </summary>
	/// <param name="_pEnemy">ガーディアンポインタ</param>
	void TransformUpdate(Guardian* _pEnemy);

	std::string stateName_;
	Guardian* pGuardian_ = nullptr;

	Motion motion_;


};

