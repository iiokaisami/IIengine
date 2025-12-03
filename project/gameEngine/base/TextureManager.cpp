#include "TextureManager.h"

#include <cassert>
#include <d3d12.h>

uint32_t TextureManager::kSRVIndexTop = 1;

TextureManager* TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}

void TextureManager::Finalize(){}

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	// SRVの数と同数
	textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

void TextureManager::LoadTexture(const std::string& filePath, bool forceCubeMap)
{
	// 読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		// 読み込み済みなら早期return
		return;
	}
	// テクスチャ枚数上限チェック
	assert(srvManager_->IsAllocate());

	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = StringUtility::ConvertString(filePath);
	HRESULT hr;
	if (filePathW.ends_with(L".dds"))
	{
		// DDSファイルの読み込み
		hr = DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else
	{
		hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
		assert(SUCCEEDED(hr));
	}

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format))
	{
		// 圧縮テクスチャの場合は、ミップマップを生成しない
		mipImages = std::move(image);
	} 
	else
	{
		hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	}

	const auto& meta = mipImages.GetMetadata();
	if (filePathW.ends_with(L".dds")) 
	{
		// キューブマップとして使いたい場合のみチェック
		assert(!forceCubeMap or meta.IsCubemap()); // forceCubeMap時は必ずキューブマップ
		assert(!forceCubeMap or meta.arraySize == 6);
	}
	meta;
	forceCubeMap;

	// ★ ここでmetaの内容を出力
	const auto& meta2 = mipImages.GetMetadata();
	OutputDebugStringA(std::format(
		"meta2: IsCubemap={}, arraySize={}, mipLevels={}\n",
		meta2.IsCubemap(), meta2.arraySize, meta2.mipLevels
	).c_str());

	// テクスチャデータを追加
	// 追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas[filePath];

	textureData.metadata = mipImages.GetMetadata();
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);
	textureData.intermediate = dxCommon_->UploadTextureData(textureData.resource, mipImages);
	dxCommon_->CommandPass();

	// テクスチャデータの要素番号をSRVのインデックスをする
	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	// SRVの設定
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	OutputDebugStringA(std::format(
		"DEBUG: dimension={}, IsCubemap={}, arraySize={}\n",
		static_cast<int>(textureData.metadata.dimension),
		textureData.metadata.IsCubemap(),
		textureData.metadata.arraySize
	).c_str());

	if (textureData.metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE2D && textureData.metadata.IsCubemap() && textureData.metadata.arraySize == 6)
	{
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0; // 最も詳細なミップレベル
		srvDesc.TextureCube.MipLevels = UINT(textureData.metadata.mipLevels);
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f; // 最小LODクランプ値
	}
	else
	{
		assert(textureData.metadata.IsCubemap() == false && "2DテクスチャなのにIsCubemap==true");
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	}

	// ★ ここでSRV設定内容を出力
	OutputDebugStringA(std::format(
		"SRV: ViewDimension={}, TextureCube.MipLevels={}\n",
		static_cast<int>(srvDesc.ViewDimension),
		static_cast<unsigned int>(srvDesc.TextureCube.MipLevels)
	).c_str());


	// 設定を基にSRVの生成
	dxCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);


	// ここでSRVの内容を出力
	OutputDebugStringA(std::format(
		"[SRV登録] filePath={}, srvIndex={}, ViewDimension={}, IsCubemap={}, arraySize={}\n",
		filePath,
		textureData.srvIndex,
		static_cast<int>(srvDesc.ViewDimension),
		textureData.metadata.IsCubemap(),
		textureData.metadata.arraySize
	).c_str());
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	// テクスチャが存在するか確認
	auto it = textureDatas.find(filePath);
	if (it == textureDatas.end()) {
		// なかったらエラーメッセージ
		Logger::Log("Error: Texture not found for filePath: " + filePath);
		throw std::runtime_error("Texture not found for filePath: " + filePath);
	}

	// テクスチャデータの参照を取得
	TextureData& textureData = it->second;
	return textureData.metadata;
}


uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	// テクスチャが存在するか確認
	auto it = textureDatas.find(filePath);
	if (it == textureDatas.end()) {
		// なかったらエラーメッセージ
		Logger::Log("Error: Texture not found for filePath: " + filePath);
		throw std::runtime_error("Texture not found for filePath: " + filePath);
	}

	// テクスチャデータの参照を取得
	TextureData& textureData = it->second;
	return textureData.srvIndex;
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{

	// テクスチャが存在するか確認
	auto it = textureDatas.find(filePath);
	if (it == textureDatas.end()) {
		// なかったらエラーメッセージ
		Logger::Log("Error: Texture not found for filePath: " + filePath);
		throw std::runtime_error("Texture not found for filePath: " + filePath);
	}

	// テクスチャデータの参照を取得
	TextureData& textureData = it->second;
	return textureData.srvHandleGPU;
}