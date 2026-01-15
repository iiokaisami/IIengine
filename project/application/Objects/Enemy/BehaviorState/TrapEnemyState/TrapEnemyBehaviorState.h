#pragma once

#include <sstream>
#include <Vector3.h>

// 前方宣言
class TrapEnemy;

/// <summary>
/// 罠型敵の行動ステート基底クラス
/// 行動ステートは以下の5つ
/// ・出現(Spawn)
/// ・移動(Move)
/// ・罠設置(SetTrap)
/// ・被弾(HitReact)
/// ・死亡(Dead)
/// </summary>
class TrapEnemyBehaviorState
{
public:
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_name">ステート名</param>
	/// <param name="_trapEnemy">罠型敵のポインタ</param>
	TrapEnemyBehaviorState(const std::string& _name, TrapEnemy* _trapEnemy) : stateName_(_name), pTrapEnemy_(_trapEnemy) {};
	
	virtual ~TrapEnemyBehaviorState();
	
	// 初期化
	virtual void Initialize() = 0;
	
	// 更新
	virtual void Update() = 0;
	
	// モーションのリセット
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

	// モーションのカウント
	void MotionCount(Motion& _motion);

	// 敵のトランスフォームを motion_.transformにセット
	void TransformUpdate(TrapEnemy* _pEnemy);

	std::string stateName_;
	TrapEnemy* pTrapEnemy_ = nullptr;

	Motion motion_;

};

