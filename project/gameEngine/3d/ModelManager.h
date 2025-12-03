#pragma once

#include <string>
#include <map>
#include <memory>

#include "Model.h"

class ModelCommon;
class DirectXCommon;

/// <summary>
/// モデル管理クラス
/// モデルの読み込み、検索を行う
/// </summary>
class ModelManager
{
#pragma region シングルトンインスタンス

private:

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator = (ModelManager&) = delete;

public:

	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();

	// 終了
	void Finalize();

#pragma region シングルトンインスタンス

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectX共通機能管理クラスへのポインタ</param>
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	void LoadModel(const std::string& filePath);

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>モデルデータ</returns>
	Model* FindModel(const std::string& filePath);

private:

	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;

	// モデル共通部
	ModelCommon* modelCommon_ = nullptr;
};

