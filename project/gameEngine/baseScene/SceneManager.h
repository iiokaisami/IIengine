#pragma once

#include <memory>
#include <string>

#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// シーンマネージャ
/// ここでシーンの更新、描画を行う
/// </summary>
class SceneManager
{
#pragma region シングルトンインスタンス
private:

	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator = (SceneManager&) = delete;

public:
	// シングルトンインスタンスの取得
	static SceneManager* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス
public:

	// 更新
	void Update();

	// 描画
	void Draw();

	/// <summary>
	/// シーン変更
	/// </summary>
	/// <param name="sceneName">次のシーン名</param>
	void ChangeScene(const std::string& sceneName);

	/// <summary>
	/// シーンファクトリーを設定
	/// </summary>
	/// <param name="sceneFactory">シーンファクトリーのポインタ</param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:

	// 今のシーン
	std::unique_ptr<BaseScene> scene_ = nullptr;

	// 次のシーン
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	// シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

