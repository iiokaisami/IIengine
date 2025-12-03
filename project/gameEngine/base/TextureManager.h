#pragma once

#include <unordered_map>

#include "DirectXCommon.h"
#include "SrvManager.h"

/// <summary>
/// テクスチャ管理クラス
/// テクスチャの読み込み、SRVの生成、SRVハンドルの取得を行う
/// </summary>
class TextureManager
{
private:

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = default;
	TextureManager& operator=(TextureManager&) = default;

public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static TextureManager* GetInstance();
	// 終了
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectX共通クラス</param>
	/// <param name="srvManager">SRV管理クラス</param>
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);

	/// <summary>
	/// テクスチャファイルの読み込み
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <param name="forceCubeMap">強制的にキューブマップとして読み込むか</param>
	void LoadTexture(const std::string& filePath, bool forceCubeMap = false);

public: // ゲッター

	/// <summary>
	/// メタデータを取得
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>メタデータ</returns>
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

	/// <summary>
	/// SRVインデックスの取得
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>SRVインデックス</returns>
	uint32_t GetTextureIndexByFilePath(const std::string& filePath);

	/// <summary>
	/// テクスチャ番号からGPUハンドルを所得
	/// </summary>
	/// <param name="filePath">ファイルパス</param>
	/// <returns>GPUデスクリプタハンドル</returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

	/// <summary>
	/// SRV管理クラスを取得
	/// </summary>
	/// <returns>SRV管理クラス</returns>
	SrvManager* GetSrvManager() const { return srvManager_; }


private: // 構造体

	// テクスチャ1枚分のデータ
	struct TextureData
	{
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediate;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

private:

	// テクスチャデータ
	std::unordered_map<std::string, TextureData> textureDatas;

	DirectXCommon* dxCommon_ = nullptr;

	SrvManager* srvManager_ = nullptr;

	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;
};