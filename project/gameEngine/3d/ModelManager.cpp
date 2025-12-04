#include "ModelManager.h"

#include <filesystem>

#include "DirectXCommon.h"
#include "ModelCommon.h"

ModelManager* ModelManager::GetInstance()
{
	static ModelManager instance;
	return &instance;
}

void ModelManager::Finalize(){}

void ModelManager::Initialize(DirectXCommon* dxCommon)
{
	modelCommon_ = std::make_unique<ModelCommon>();
	modelCommon_->Initialize(dxCommon);
}

void ModelManager::LoadModel(const std::string& filePath)
{
	// ファイルパスからディレクトリとファイル名を抽出
	std::filesystem::path path(filePath);
	std::string directory = "resources/models/" + path.parent_path().string();
	std::string fileName = path.filename().string(); // ファイル名だけを抽出

	// 読み込み済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込み済みなら早期 return
		return;
	}

	// ディレクトリが存在しない場合は "resources/models/" をデフォルトに
	if (directory.empty()) {
		directory = "resources/models/";
	}

	// モデルの生成とファイル読み込み・初期化 ---
	std::unique_ptr<Model> model = std::make_unique<Model>();
	model->Initialize(modelCommon_.get(), directory, fileName);

	// モデルをmapコンテナに格納する
	models.insert(std::make_pair(fileName, std::move(model))); // 所有権を譲渡
}

Model* ModelManager::FindModel(const std::string& filePath)
{
	// 読み込み済みモデルを検索
	if (models.contains(filePath)) {
		// 読み込みモデルを戻り値としてreturn
		return models.at(filePath).get();
	}
	// ファイル名一致無し
	return nullptr;
}
