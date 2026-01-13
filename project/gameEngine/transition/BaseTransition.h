#pragma once

#include <functional>

namespace IIEngine
{

	/// <summary>
	/// シーン切り替え時のトランジション処理を管理するための基底クラス
	/// 継承して使用する
	/// </summary>
	class BaseTransition
	{
	public:

		virtual ~BaseTransition() = default;

		/// <summary>
		/// シーン変更時に呼び出されるコールバック関数を登録
		/// </summary>
		/// <param name="onSceneChange">シーンが変更されたときに実行されるコールバック関数</param>
		virtual void Start(std::function<void()> onSceneChange) = 0;

		/// <summary>
		/// 更新
		/// </summary>
		virtual void Update() = 0;

		/// <summary>
		/// 描画
		/// </summary>
		virtual void Draw() = 0;

	public: // ゲッター

		// トランジションが終了したか
		virtual bool IsFinished() const = 0;

	};

}