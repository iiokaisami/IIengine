#pragma once

#include <vector>
#include <memory>

#include "../../../../baseObject/BaseBullet.h"

/// <summary>
/// 弾の管理クラス
/// </summary>
class BulletManager
{
public:

    /// <summary>
	/// 弾の追加
    /// </summary>
	/// <param name="_bullet">追加する弾のユニークポインタ</param>
    void AddBullet(std::unique_ptr<BaseBullet> _bullet);

    /// <summary>
	/// 全ての弾の更新と消去処理
    /// </summary>
    void UpdateAll();

    /// <summary>
	/// 全ての弾の描画
    /// </summary>
    void DrawAll();

private:

	// 弾のリスト
    std::vector<std::unique_ptr<BaseBullet>> bullets_;

};

