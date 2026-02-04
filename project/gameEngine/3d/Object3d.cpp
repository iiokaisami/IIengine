#include "Object3d.h"

#include <fstream>
#include <sstream>
#include <numbers>

#include "Object3dCommon.h"
#include "Model.h"
#include "ModelManager.h"

namespace IIEngine
{

	void Object3d::Initialize(const std::string& filePath)
	{
		object3dCommon_ = Object3dCommon::GetInstance();
		// モデルを設定
		SetModel(filePath);

		// 座標変換行列リソースを作る
		CreateTransformationMatrixData();

		// 平行光源リソースを作る
		CreateDirectionalLight();

		// Transform変数を作る
		transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };

		camera_ = object3dCommon_->GetDefaultCamera();

		// カメラリソースを作る
		CreateCamera();

		// ポイントライトリソースを作る
		CreatePointLight();

		// スポットライトリソースを作る
		CreateSpotLight();

		// 環境マップリソースを作る
		CreateEnvironment();
	}

	void Object3d::Update()
	{
		Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
		Matrix4x4 worldViewProjectionMatrix;

		model_->SetEnableLighting(enableLighting);
		model_->SetEnableDirectionalLight(directionalLightData_->enable);
		model_->SetEnablePointLight(pointLightData_->enable);
		model_->SetEnableSpotLight(spotLightData_->enable);
		model_->SetEnvironment(environmentData_->enable);
		model_->SetEnvironmentStrength(environmentData_->strength);



		if (camera_)
		{
			const Matrix4x4& viewProjectionMatrix = camera_->GetViewProjectionMatrix();
			worldViewProjectionMatrix = (worldMatrix * viewProjectionMatrix);
		} else
		{
			worldViewProjectionMatrix = worldMatrix;
		}

		//transformationMatrixData_->WVP = worldViewProjectionMatrix;
		//transformationMatrixData_->World = worldMatrix;

		transformationMatrixData_->WVP = model_->GetModelData().rootNode.localMatrix * worldViewProjectionMatrix;
		transformationMatrixData_->World = model_->GetModelData().rootNode.localMatrix * worldMatrix;
	}

	void Object3d::Draw()
	{
		// TransformationMatrixCBufferの場所を設定
		object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
		// 平行光源CBufferの場所を設定
		object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
		// カメラCBufferの場所を設定
		object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
		// ポイントライトCBufferの場所を設定
		object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
		// スポットライトCBufferの場所を設定
		object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());

		if (environmentMapHandle_.ptr != 0)
		{
			//object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(2, environmentMapHandle_);
			//SetGraphicsRootDescriptorTable
			object3dCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(8, environmentMapHandle_);
		}

		if (model_)
		{
			model_->Draw();
		}
	}

	void Object3d::SetModel(const std::string& filePath)
	{
		model_ = ModelManager::GetInstance()->FindModel(filePath);
		modelFilePath_ = filePath;
	}

	void Object3d::SetEnvironmentMapHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle, bool useEnvironmentMap)
	{
		environmentMapHandle_ = handle;
		environmentData_->enable = useEnvironmentMap;
	}

	void Object3d::CreateTransformationMatrixData()
	{
		// 座標変換行列リソースを作る
		transformationMatrixResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(TransformationMatrix));
		//書き込むためのアドレス
		transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
		//単位行列を書き込んでおく
		transformationMatrixData_->World = MakeIdentity4x4();
		transformationMatrixData_->WVP = MakeIdentity4x4();
		transformationMatrixData_->WorldInvTranspose = InverseTranspose(transformationMatrixData_->World);
	}

	void Object3d::CreateDirectionalLight()
	{
		// 平行光源リソースを作る
		directionalLightResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(DirectionalLight));
		//書き込むためのアドレス
		directionalLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData_));
		//デフォルト値は以下のようにしておく
		directionalLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
		directionalLightData_->direction = Normalize({ 0.0f,-1.0f,0.0f });
		directionalLightData_->intensity = 1.0f;
		directionalLightData_->enable = false;
	}

	void Object3d::CreateCamera()
	{
		// カメラリソースを作る
		cameraResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(CameraForGPU));
		//書き込むためのアドレス
		cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));
		//デフォルト値は以下のようにしておく
		cameraData_->worldPosition = camera_->GetPosition();
	}

	void Object3d::CreatePointLight()
	{
		// ポイントライトリソースを作る
		pointLightResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(PointLight));
		// 書き込むためのアドレス
		pointLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&pointLightData_));
		// デフォルト値は以下のようにしておく
		pointLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
		pointLightData_->position = { 0.0f,0.0f,0.0f };
		pointLightData_->intensity = 1.0f;
		pointLightData_->radius = 10.0f;
		pointLightData_->decay = 1.0f;
		pointLightData_->enable = false;
	}

	void Object3d::CreateSpotLight()
	{
		// スポットライトリソースを作る
		spotLightResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(SpotLight));
		// 書き込むためのアドレス
		spotLightResource_->Map(0, nullptr, reinterpret_cast<void**>(&spotLightData_));
		// デフォルト値は以下のようにしておく
		spotLightData_->color = { 1.0f,1.0f,1.0f,1.0f };
		spotLightData_->position = { 2.0f,1.25f,0.0f };
		spotLightData_->intensity = 4.0f;
		spotLightData_->direction = Normalize({ 0.0f,-1.0f,0.0f });
		spotLightData_->distance = 7.0f;
		spotLightData_->decay = 2.0f;
		spotLightData_->consAngle = std::cos(std::numbers::pi_v<float> / 3.0f);
		spotLightData_->cosFalloffStart = 1.0f;
		spotLightData_->enable = false;
	}

	void Object3d::CreateEnvironment()
	{
		// 環境マップリソースを作る
		environmentResource_ = object3dCommon_->GetDxCommon()->CreateBufferResource(sizeof(Environment));
		// 書き込むためのアドレス
		environmentResource_->Map(0, nullptr, reinterpret_cast<void**>(&environmentData_));
		// デフォルト値は以下のようにしておく
		environmentData_->enable = false;
		environmentData_->strength = 0.5f;
	}

	std::string Object3d::GetModel() const
	{
		return modelFilePath_;
	}
}