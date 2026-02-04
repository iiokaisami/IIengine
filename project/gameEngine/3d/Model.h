#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "MyMath.h"
#include "TextureManager.h"

namespace IIEngine
{

	// 前方宣言
	class ModelCommon;

	/// <summary>
	/// 3Dモデル
	/// Objファイルを読み込む
	/// </summary>
	class Model
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="modelCommon">モデル共通機能管理クラスへのポインタ</param>	
		/// <param name="directoryPath">ディレクトリパス</param>
		/// <param name="filename">ファイル名</param>
		void Initialize(ModelCommon* modelCommon, const std::string& directoryPath, const std::string& filename);

		/// <summary>
		/// 更新
		/// </summary>
		void UpData();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

		/// <summary>
		/// 頂点バッファ更新
		///	</summary>
		void UpdateVertexBuffer();

		/// <summary>
		/// インデックスバッファ更新
		/// </summary>
		void UpdateIndexBuffer();

		/// <summary>
		/// 頂点データクリア
		/// </summary>
		void ClearVertexData();

		/// <summary>
		/// 他のモデルデータをコピー
		/// </summary>
		/// <param name="other">コピー元モデルデータ</param>
		void CopyFrom(const Model& other);

	private: // 構造体、関数

		struct Transform
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

		struct MaterialData
		{
			std::string textureFilePath;
			uint32_t textureIndex = 0;
		};

		struct Material
		{
			Vector4 color;
			int32_t enableLighting;
			float padding[3];
			Matrix4x4 uvTransform;
			float shininess;
			int32_t phongReflection;
			int32_t halfPhongReflection;
			int32_t pointLight;
			int32_t spotLight;
			int32_t environment;
			float environmentStrength;
		};

		struct  Node
		{
			Matrix4x4 localMatrix = {};
			std::string name;
			std::vector<Node> children;
		};

		struct  ModelData
		{
			std::vector<VertexData> vertices;
			std::vector<uint32_t> indices;
			MaterialData material;
			Node rootNode;
		};

		//mtlファイルを読む関数
		static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

		//Objファイルを読む関数
		static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

		// 頂点データ生成
		void CreateVertexData();

		// マテリアルデータ生成
		void CreateMaterialData();

		static Node ReadNode(aiNode* node);


	public: // ゲッター

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView()const { return vertexBufferView_; }
		ModelData GetModelData() { return modelData_; }

		uint32_t GetVertexCount() const { return static_cast<uint32_t>(modelData_.vertices.size()); }

		uint32_t GetIndexCount() const { return static_cast<uint32_t>(modelData_.indices.size()); }

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const { return indexBufferView_; }

	public: // セッター

		// Vertexを入れる
		void AddVertex(const Vector4& position, const Vector2& texcoord, const Vector3& normal);

		// indexを入れる
		void AddIndex(uint32_t index);

		void SetEnableLighting(bool enable);
		void SetEnableDirectionalLight(bool enable);
		void SetEnablePointLight(bool enable);
		void SetEnableSpotLight(bool enable);
		void SetEnvironment(bool enable);
		void SetEnvironmentStrength(float strength);

		void SetModelCommon(ModelCommon* modelCommon) { modelCommon_ = modelCommon; }

	private:

		ModelCommon* modelCommon_ = nullptr;

		// Objファイルのデータ
		ModelData modelData_;

		// バッファリソース
		// 頂点リソース
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_{};
		// マテリアルリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_{};

		// バッファリソース内のデータを指すポインタ
		VertexData* vertexData_ = nullptr;
		Material* materialData_ = nullptr;

		// バッファリソースの使い道を補足するバッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

		// Transform
		Transform transform_;

		// GPU上のインデックスバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_{};
		// GPU上にセットするためのビュー
		D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	};

}