#pragma once

#include "AbstractSceneFactory.h"

/// <summary>
/// シーンファクトリ
/// シーンの生成を行う
/// </summary>
class SceneFactory : public AbstractSceneFactory
{
public:

	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName">生成したいシーンの名前</param>
	/// <returns>生成されたシーンのインスタンス</returns>
	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;
};