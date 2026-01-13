#pragma once

#include <sstream>
#include <Vector3.h>


// 前方宣言
class Corruptor;

/// <summary>
/// コラプター行動ステート基底クラス
/// 行動ステートは以下の3つ
/// ・出現
/// ・移動
/// ・自爆(死亡時も同じ行動)
/// </summary>
class CorruptorBehaviorState
{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="_name">ステート名</param>
	/// <param name="_corruptor">コラプターのポインタ</param>
	CorruptorBehaviorState(const std::string& _name, Corruptor* _corruptor) : stateName_(_name), pCorruptor_(_corruptor){}
	
	virtual ~CorruptorBehaviorState();
	
	// 初期化
	virtual void Initialize() = 0;
	
	// 更新
	virtual void Update() = 0;
	
	// モーションリセット
	virtual void ResetMotion() = 0;
	
protected:

	struct Transform
	{
		Vector3 position{};
		Vector3 rotation{};
		Vector3 scale{};
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

	// 敵のトランスフォームをmotion_.transformにセット
	void TransformUpdate(Corruptor* _pEnemy);

	std::string stateName_;
	Corruptor* pCorruptor_ = nullptr;

	Motion motion_;

};

