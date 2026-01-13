#pragma once

#include "BaseTransition.h"

#include <memory>
#include <functional>

namespace IIEngine
{

    /// <summary>
    /// トランジション管理クラス
    /// 他のシーンへ切り替える際にトランジションを管理する
    /// </summary>
    class TransitionManager
    {

    public:

        /// <summary>
        /// 開始(シーン切り替え時に呼び出す)
        /// </summary>
        /// <param name="transition">トランジション処理を行うBaseTransition派生オブジェクト</param>
        /// <param name="onSceneChange">シーン切り替え時に呼び出されるコールバック関数</param>
        void Start(std::unique_ptr<BaseTransition> transition, std::function<void()> onSceneChange);

        // 更新
        void Update();

        // 描画
        void Draw();

    public: // ゲッター

        // トランジション中か
        bool IsRunning() const;

    private:

        // 現在のトランジション
        std::unique_ptr<BaseTransition> transition_;

    };

}