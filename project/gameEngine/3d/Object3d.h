#pragma once

#include "MyMath.h"
#include "TextureManager.h"
#include "CameraManager.h"


class Object3dCommon;
class Model;

/// <summary>
/// 3Dオブジェクト
/// </summary>
class Object3d
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	void Initialize(const std::string& filePath);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

public: // セッター

	/// <summary>
	/// カメラ設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(std::shared_ptr<Camera> camera) { camera_ = camera; }

	/// <summary>
	/// モデル設定
	/// </summary>
	/// <param name="model">モデル</param>
	void SetModel(Model* model) { model_ = model; }

	/// <summary>
	/// モデル設定(ファイルパスから)
	/// </summary>
	/// <param name="filePath">モデルファイルのパス</param>
	void SetModel(const std::string& filePath);

	/// <summary>
	/// スケール設定
	/// </summary>
	/// <param name="scale">スケール</param>
	void SetScale(const Vector3& scale) { transform_.scale = scale; }

	/// <summary>
	/// 回転設定
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(const Vector3& rotate) { transform_.rotate = rotate; }

	/// <summary>
	/// 位置設定
	/// </summary>
	/// <param name="translate">位置</param>
	void SetPosition(const Vector3& translate) { transform_.translate = translate; }

	/// <summary>
	/// 環境マップ設定
	/// </summary>
	/// <param name="handle">環境マップのディスクリプタハンドル</param>
	/// <param name="useEnvironmentMap">環境マップを使用するかどうか</param>
	void SetEnvironmentMapHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle, bool useEnvironmentMap);

public: // ゲッター

	// スケール取得
	const Vector3& GetScale() const { return transform_.scale; }

	// 回転取得
	const Vector3& GetRotate() const { return transform_.rotate; }

	// 位置取得
	const Vector3& GetPosition() const { return transform_.translate; }

	// モデル取得
	std::string GetModel() const;

private:

	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Matrix4x4 WorldInvTranspose;
	};

	struct DirectionalLight
	{
		Vector4 color;
		Vector3 direction;
		float intensity;
		bool enable;
	};

	struct Transform
	{
		Vector3 scale;
		Vector3 rotate;
		Vector3 translate;
	};

	struct CameraForGPU
	{
		Vector3 worldPosition;
	};

	struct PointLight
	{
		Vector4 color;
		Vector3 position;
		float intensity;
		float radius;
		float decay;
		bool enable;
		char padding[3];
	};

	struct SpotLight
	{
		Vector4 color;
		Vector3 position;
		Vector3 direction;
		float intensity;
		float distance;
		float decay;
		float consAngle;
		float cosFalloffStart;
		bool enable;
	};

	struct Environment
	{
		bool enable;
		float strength;
	};

public: // ライト等

	/// <summary>
	/// 座標変換行列データ生成
	/// </summary>
	void CreateTransformationMatrixData();

	/// <summary>
	/// 平行光源データ生成
	/// </summary>
	void CreateDirectionalLight();
	/// <summary>
	/// 平行光源設定
	/// </summary>
	/// <param name="directionalLight">平行光源データ</param>
	void SetDirectionalLight(const DirectionalLight& directionalLight) { *directionalLightData_ = directionalLight; }
	/// <summary>
	/// 平行光源取得
	/// </summary>
	/// <returns>平行光源データ</returns>
	DirectionalLight GetDirectionalLight() { return *directionalLightData_; }
	/// <summary>
	/// 平行光源の方向設定
	/// </summary>
	/// <param name="direction">方向ベクトル</param>
	void SetDirectionalLightDirection(const Vector3& direction) { directionalLightData_->direction = direction; }
	/// <summary>
	/// 平行光源の色設定
	/// </summary>
	/// <param name="color">色ベクトル</param>
	void SetDirectionalLightColor(const Vector4& color) { directionalLightData_->color = color; }
	/// <summary>
	/// 平行光源の強さ設定
	/// </summary>
	/// <param name="intensity">強さ</param>
	void SetDirectionalLightIntensity(float intensity) { directionalLightData_->intensity = intensity; }
	
	/// <summary>
	/// 平行光源のオンオフ設定
	/// </summary>
	/// <param name="enable"> オンオフフラグ</param>
	void SetDirectionalLightEnable(bool enable) { directionalLightData_->enable = enable; }

	/// <summary>
	/// カメラデータ生成
	/// </summary>
	void CreateCamera();

	/// <summary>
	/// ポイントライトデータ生成
	/// </summary>
	void CreatePointLight();
	/// <summary>
	/// ポイントライト設定
	/// </summary>
	/// <param name="pointLight">ポイントライトデータ</param>
	void SetPointLight(const PointLight& pointLight) { *pointLightData_ = pointLight; }
	/// <summary>
	/// ポイントライト取得
	/// </summary>
	/// <returns>ポイントライトデータ</returns>
	PointLight GetPointLight() { return *pointLightData_; }
	/// <summary>
	/// ポイントライトの位置設定
	/// </summary>
	/// <param name="position">位置ベクトル</param>
	void SetPointLightPosition(const Vector3& position) { pointLightData_->position = position; }
	/// <summary>
	/// ポイントライトの色設定
	/// </summary>
	/// <param name="color">色ベクトル</param>
	void SetPointLightColor(const Vector4& color) { pointLightData_->color = color; }
	/// <summary>
	/// ポイントライトの強さ設定
	/// </summary>
	/// <param name="intensity">強さ</param>
	void SetPointLightIntensity(float intensity) { pointLightData_->intensity = intensity; }
	/// <summary>
	/// ポイントライトの距離設定
	/// </summary>
	/// <param name="radius">距離</param>
	void SetPointLightRadius(float radius) { pointLightData_->radius = radius; }
	/// <summary>
	/// ポイントライトの距離取得
	/// </summary>
	/// <returns>距離</returns>
	float GetPointLightRadius() { return pointLightData_->radius; }
	/// <summary>
	/// ポイントライトの減衰率設定
	/// </summary>
	/// <param name="decay">減衰率</param>
	void SetPointLightDecay(float decay) { pointLightData_->decay = decay; }
	/// <summary>
	/// ポイントライトの減衰率取得
	/// </summary>
	/// <returns>減衰率</returns>
	float GetPointLightDecay() { return pointLightData_->decay; }
	/// <summary>
	/// ポイントライトのオンオフ設定
	/// </summary>
	/// <param name="enable">オンオフフラグ</param>
	void SetPointLightEnable(bool enable) { pointLightData_->enable = enable; }

	
	/// <summary>
	/// スポットライトデータ生成
	/// </summary>
	void CreateSpotLight();
	/// <summary>
	/// スポットライト設定
	/// </summary>
	/// <param name="spotLight">スポットライトデータ</param>
	void SetSpotLight(const SpotLight& spotLight) { *spotLightData_ = spotLight; }
	/// <summary>
	/// スポットライト取得
	/// </summary>
	/// <returns>スポットライトデータ</returns>
	SpotLight GetSpotLight() { return *spotLightData_; }
	/// <summary>
	/// スポットライトの位置
	/// </summary>
	/// <param name="position">位置ベクトル</param>
	void SetSpotLightPosition(const Vector3& position) { spotLightData_->position = position; }
	/// <summary>
	/// スポットライトの方向設定
	/// </summary>
	/// <param name="direction">方向ベクトル</param>
	void SetSpotLightDirection(const Vector3& direction) { spotLightData_->direction = direction; }
	/// <summary>
	/// スポットライトの色設定
	/// </summary>
	/// <param name="color">色ベクトル</param>
	void SetSpotLightColor(const Vector4& color) { spotLightData_->color = color; }
	/// <summary>
	/// スポットライトの強さ設定
	/// </summary>
	/// <param name="intensity">強さ</param>
	void SetSpotLightIntensity(float intensity) { spotLightData_->intensity = intensity; }
	/// <summary>
	/// スポットライトの距離設定
	/// </summary>
	/// <param name="distance">距離</param>
	void SetSpotLightDistance(float distance) { spotLightData_->distance = distance; }
	/// <summary>
	/// スポットライトの減衰率設定
	/// </summary>
	/// <param name="decay">減衰率</param>
	void SetSpotLightDecay(float decay) { spotLightData_->decay = decay; }
	/// <summary>
	/// スポットライトのコーン角度設定
	/// </summary>
	/// <param name="consAngle">コーン角度</param>
	void SetSpotLightConsAngle(float consAngle) { spotLightData_->consAngle = consAngle; }
	/// <summary>
	/// スポットライトのコーン外側減衰開始角度の余弦設定
	/// </summary>
	/// <param name="cosFalloffStart">コーン外側減衰開始角度の余弦</param>
	void SetSpotLightCosFalloffStart(float cosFalloffStart) { spotLightData_->cosFalloffStart = cosFalloffStart; }
	/// <summary>
	/// スポットライトのオンオフ設定
	/// </summary>
	/// <param name="enable">オンオフフラグ</param>
	void SetSpotLightEnable(bool enable) { spotLightData_->enable = enable; }

	
	/// <summary>
	/// ライティングのオンオフ設定
	/// </summary>
	/// <param name="enable">オンオフフラグ</param>
	void SetLighting(bool enable) { enableLighting = enable; }

	/// <summary>
	/// 環境マップデータ生成
	/// </summary>
	void CreateEnvironment();
	/// <summary>
	/// 環境マップのオンオフ設定
	/// </summary>
	/// <param name="enable">オンオフフラグ</param>
	void IsEnvironment(bool enable) { environmentData_->enable = enable; }
	/// <summary>
	/// 環境マップの強さ設定
	/// </summary>
	/// <param name="strength">強さ</param>
	void SetEnvironmentStrength(float strength) { environmentData_->strength = strength; }

private:

	Object3dCommon* object3dCommon_ = nullptr;
	
	std::shared_ptr<Camera> camera_ = nullptr;

	Model* model_ = nullptr;
	
	//TransformationMatrix用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_{};
	// 平行光源リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_{};
	// カメラリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_{};
	// ポイントライトリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> pointLightResource_{};
	// スポットライトリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> spotLightResource_{};
	// 環境マップリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> environmentResource_{};


	//// バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;
	CameraForGPU* cameraData_ = nullptr;
	PointLight* pointLightData_ = nullptr;
	SpotLight* spotLightData_ = nullptr;
	Environment* environmentData_ = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE environmentMapHandle_ = { 0 };

	Transform transform_{};

	//ライトのオンオフ
	bool enableLighting = false;

	std::string modelFilePath_;
};

