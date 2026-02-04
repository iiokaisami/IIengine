#pragma once

#include "../../baseObject/StaticObject.h"
#include "../../../gameEngine/collider/ColliderManager.h"
#include "Barrier.h"

#include <Object3d.h>
#include <memory>
#include <Framework.h>

/// <summary>
/// ゴール
/// </summary>
class Goal : public StaticObject
{
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Draw() override;

	// ImGui
	void ImGuiDraw();

public: // セッター

	/// <summary>
	/// ゴール出現フラグをセット
	/// </summary>
	/// <param name="_isCleared"> クリアフラグ</param>
	void SetBarrierDestroyed(bool _isDestroyed) { isBarrierDestroyed_ = _isDestroyed; }
	
public: // ゲッター

	// クリアフラグを取得
	bool IsCleared() const { return isCleared_; }


private: // 衝突判定

	/// <summary>
	/// 当たり判定処理
	/// </summary>
	/// <param name="_other"> 衝突相手のコライダー</param>
	void OnCollisionTrigger(const IIEngine::Collider* _other);

private:

	// バリア
	std::unique_ptr<Barrier> pBarrier_ = nullptr;

	// クリアフラグ
	bool isCleared_ = false;

	// バリア破壊フラグ
	bool isBarrierDestroyed_ = false;
	bool wasBarrierDestroyed_ = false;

};