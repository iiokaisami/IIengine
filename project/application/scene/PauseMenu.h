#pragma once

#include <Framework.h>

#include <vector>
#include <memory>

/// <summary>
/// ポーズメニュー
/// </summary>
class PauseMenu
{
public:

	// コンストラクタ・デストラクタ
	PauseMenu() = default;
	~PauseMenu() = default;

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// メニューを開く
	void Open();

	// メニューを閉じる
	void Close();

	enum class MenuOption
	{
		None,
		Resume,
		Restart,
		ReturnTitle
	};

	MenuOption GetSelectedOption() const;

	MenuOption ConsumeSelectedOption();

private:

	// メニューがアクティブかどうか
	bool isActive_ = false;
	bool ignoreEscape_ = false;

	// メニューオプション
	uint32_t selectedIndex_ = 0;
	std::vector<MenuOption> options_;

	// 選択されたメニューオプション
	MenuOption selectedOption_ = MenuOption::None;

	// スプライト
	std::vector<std::unique_ptr<Sprite>> menuSprites_;
	uint32_t spriteNum_ = 5;

	// アニメーション用
	float animeTimer_ = 0.0f;

	// 各項目の基準位置
	std::vector<Vector2> basePositions_;
};

