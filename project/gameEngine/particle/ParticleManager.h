#pragma once

#include <unordered_map>
#include <random> 
#include <TextureManager.h>
#include <Object3d.h>
#include <numbers>
#include <MyMath.h>
#include <DirectXCommon.h>
#include <SrvManager.h>
#include <Camera.h>

#include "Particle.h"
#include "ParticleMotion.h"
#include "MeshBuilder.h"
#include "ModelCommon.h"

class Object3dCommon;

// 構造体(Emitterでも使いたい)
// マテリアルデータ
struct MaterialData
{
	std::string textureFilePath;
	uint32_t textureIndex = 0;
};
// GPU用パーティクル構造体
struct ParticleForGPU
{
	Matrix4x4 WVP;
	Matrix4x4 world;
	Vector4 color;
};

// パーティクルグループ構造体
struct ParticleGroup
{
	MaterialData materialData;
	std::list<Particle> particleList;
	uint32_t srvIndex;
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;
	uint32_t instanceCount = 0;
	ParticleForGPU* instancingData;
	std::string motionName = "Homing";
};
// エミット設定構造体
struct EmitSetting {
	std::string groupName;
	std::string motionName;
	Vector3 emitPosition;
	uint32_t emitCount = 1;
	float timer = 0.0f;
	bool isLooping = false;
};

/// <summary>
/// パーティクル管理クラス
/// </summary>
class ParticleManager
{
public:

	// シングルトンインスタンスの取得
	static ParticleManager* GetInstance();


	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon">DirectX共通機能</param>
	/// <param name="srvManager">SrvManagerのポインタ</param>
	/// <param name="modelCommon">ModelCommonのポインタ</param>
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, ModelCommon* modelCommon);

	/// <summary>
	/// 終了
	/// </summary>
	void Finalize();

	/// <summary>
	/// パイプライン生成
	/// </summary>
	void CreatePipeline();

	/// <summary>
	/// ルートシグネチャ生成
	/// </summary>
	void CreateRootSignature();

	/// <summary>
	/// パーティクルグループの生成
	/// </summary>
	/// <param name="name">パーティクルグループの名前</param>
	/// <param name="textureFilePath">テクスチャファイルのパス</param>
	/// <param name="modelFilePath">モデルファイルのパス</param>
	/// <param name="type">パーティクルのタイプ("Default", "Ring", "Cylinder", "Slash"など)</param>
	/// <param name="motionName">動きの名前</param>
	void CreateParticleGroup(const std::string& name, const std::string& textureFilePath, const std::string& modelFilePath, const std::string& type = "Default", const std::string& motionName = "Homing");

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// パーティクルの発生
	/// </summary>
	/// <param name="groupName">パーティクルグループの名前</param>
	/// <param name="position">発生位置</param>
	/// <param name="count">発生数</param>
	void Emit(const std::string groupName, const Vector3& position, uint32_t count);

	/// <summary>
	/// エミッター設定の追加
	/// </summary>
	/// <param name="setting">エミッター設定</param>
	void AddEmitterSetting(const EmitSetting& setting);

	/// <summary>
	/// デバッグUI
	/// </summary>
	void DebugUI();

public: // セッター

	/// <summary>
	/// カメラのセット
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(std::shared_ptr<Camera> camera) { camera_ = camera; }

private: // 構造体

	struct TransformData
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	struct VertexData
	{
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	struct ModelData
	{
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};

	struct AABB
	{
		Vector3 min;//!<最小点
		Vector3 max;//!< 最大点
	};

	struct AccelerationField
	{
		Vector3 acceleration; //!< 加速度
		AABB area;            //!< 範囲
	};

	// 頂点データの初期化（座標など）
	struct Vertex 
	{
		float position[3];
		float color[4];
	};

private:

	//インスタンス
	static ParticleManager* instance_;

	ParticleManager() = default;  // コンストラクタはプライベート

	DirectXCommon* dxCommon_ = nullptr;

	SrvManager* srvManager_ = nullptr;

	Object3dCommon* object3dCommon_ = nullptr;

	std::shared_ptr<Camera> camera_ = nullptr;

	// 頂点バッファ関連
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;

	// パイプライン関連
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

	// 乱数生成
	std::mt19937 randomEngine_;

	//ディスクリプタレンジの生成
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1]{};
	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_{};
	//InputLayout
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_{};
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc_{};
	//RasterizerStateの設定 
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	//shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_{};
	D3D12_ROOT_PARAMETER rootParameters_[4] = {};
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1] = {};
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[3] = {};

	std::unordered_map<std::string, ParticleGroup> particleGroups;


	AccelerationField accelerationField_;

	TransformData transform_;
	Matrix4x4 backToFrontMatrix_;
	//modelマテリアる用のリソースを作る。今回color1つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	//マテリアルにデータを書き込む	
	Material* materialData_ = nullptr; 
	std::unordered_map<std::string, std::unique_ptr<Model>> models_;
	ModelCommon* modelCommon_ = nullptr;


	// モデルのビューをキャッシュする用
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;

	// Cylinderの向き
	std::string direction_ = "UP";

	std::vector<EmitSetting> emitSettings_;

};