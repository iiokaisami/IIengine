#pragma once

#include "BaseScene.h"
#include <memory>
#include <string>

/// <summary>
/// シーン生成の抽象クラス
/// 継承先でCreateSceneを実装してシーンを生成する
/// </summary>
class AbstractSceneFactory
{
public:

	// 仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;

	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">生成したいシーンの名前</param>
	/// <returns>生成されたシーンのインスタンス</returns>
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) = 0;
};