#pragma once

#include <functional>
#include <memory>
#include <Sprite.h>

#include "BaseTransition.h"

namespace IIEngine
{
	/// <summary>
	/// フェードトランジション
	/// フェードインのみ or フェードアウト→シーン切り替え→フェードイン
	/// </summary>
	class FadeTransition : public IIEngine::BaseTransition
	{
	public:

		enum class Mode { FadeInOnly, Full };

		FadeTransition(Mode mode = Mode::Full);

		/// <summary>
		/// シーン変更時に呼び出されるコールバック関数を登録
		/// </summary>
		/// <param name="onSceneChange">シーン切り替え時に呼び出されるコールバック関数</param>
		void Start(std::function<void()> onSceneChange)override;

		//  更新
		void Update() override;

		//  描画
		void Draw() override;

	public: // ゲッター

		// トランジションが終了したか
		bool IsFinished() const override;

	private:

		std::unique_ptr<Sprite> fadeSprite_ = nullptr;

		Mode mode_;

		enum class State { FadeOut, Hold, FadeIn, Done };
		State state_ = State::FadeOut;
		float alpha_ = 0.0f;
		float speed_ = 0.02f;
		std::function<void()> onSceneChange_;

	};

}