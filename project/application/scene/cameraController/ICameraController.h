#pragma once

#include <memory>

/// <summary>
/// カメラコントローラーインターフェース
/// </summary>
class ICameraController
{
public:

	// デストラクタ
	virtual ~ICameraController() = default;

	// 開始
	virtual void Start() {}

	// 停止
	virtual void Stop() {}
	
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">デルタタイム</param>
	virtual void Update(float dt) = 0;
	
	// ImGui描画
	virtual void ImGuiDraw() {}

public: // ゲッター

	// コントローラーが有効かどうか
	virtual bool IsActive() const { return true; }

};

