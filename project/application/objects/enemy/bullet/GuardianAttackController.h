#pragma once

#include <vector>
#include <memory>

#include "bulletPattern/BulletPattern.h"
#include "bulletPattern/BulletManager.h"

class GuardianAttackController
{
public:

    /// <summary>
	/// 弾パターンを追加する
    /// </summary>
	/// <param name="_pattern">追加する弾パターンのユニークポインタ</param>
    void AddPattern(std::unique_ptr<BulletPattern> _pattern)
    {
        patterns_.emplace_back(std::move(_pattern));
    }

    /// <summary>
	/// 現在のパターンで弾を発射する
    /// </summary>
	/// <param name="_origin">弾の発射位置</param>
	/// <param name="_manager">弾の管理クラス</param>
	/// <param name="_time">現在の時間</param>
    void FireCurrentPattern(const Vector3& _origin, BulletManager& _manager, float _time) 
    {
        if (!patterns_.empty()) 
        {
            patterns_[currentPatternIndex_]->Fire(_origin, _manager, _time);
        }
    }

    /// <summary>
	/// 現在のパターンを切り替える
    /// </summary>
	/// <param name="_idx">切り替えるパターンのインデックス</param>
    void SetPatternIndex(size_t _idx) 
    {
        if (_idx < patterns_.size()) currentPatternIndex_ = _idx;
    }


    /// <summary>
	/// パターンの数を取得
    /// </summary>
    /// <returns>パターンの数</returns>
    size_t PatternCount() const { return patterns_.size(); }

private:

    std::vector<std::unique_ptr<BulletPattern>> patterns_;
    size_t currentPatternIndex_ = 0;


};

