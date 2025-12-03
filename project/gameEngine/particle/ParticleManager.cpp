#include "ParticleManager.h"

#include <wrl.h>
#include <stdexcept>
#include <vector>
#include <random>
#include <numbers>
#include <MyMath.h>

#include "Object3dCommon.h"
#include "ModelManager.h"
#include "TimeManager.h"

ParticleManager* ParticleManager::GetInstance()
{
    static ParticleManager instance;
    return &instance;
}

void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, ModelCommon* modelCommon)
{
    dxCommon_ = dxCommon;
    srvManager_ = srvManager;
    modelCommon_ = modelCommon;
    object3dCommon_ = Object3dCommon::GetInstance();
    // ランダムエンジンの初期化
    randomEngine_ = std::mt19937{ std::random_device{}() };

    // モーション登録
	ParticleMotion::Initialize();

    // （Cylinder方向など必要あれば指定）
    ParticleMotion::SetDirection("UP");

    // パイプライン生成
    CreatePipeline();

    transform_ = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

    // ビルボード行列の作成
    backToFrontMatrix_ = MakeRotateYMatrix(std::numbers::pi_v<float>);

    //マテリアル
    //modelマテリアる用のリソースを作る。今回color1つ分のサイズを用意する
    materialResource_ = dxCommon_->CreateBufferResource(sizeof(Material));
    //マテリアルにデータを書き込む
    materialData_ = nullptr;
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    //色
    materialData_->color = { Vector4(1.0f, 1.0f, 1.0f, 1.0f) };
    materialData_->enableLighting = false;//有効にするか否か
    materialData_->uvTransform = MakeIdentity4x4();

    camera_ = object3dCommon_->GetDefaultCamera();
}

void ParticleManager::Finalize(){}

void ParticleManager::CreatePipeline()
{
    HRESULT result = S_FALSE;

    CreateRootSignature();

    //PSOを生成する
    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
    graphicsPipelineStateDesc.pRootSignature = rootSignature_.Get();
    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc_;
    graphicsPipelineStateDesc.VS = { vertexShaderBlob_->GetBufferPointer(),vertexShaderBlob_->GetBufferSize() };
    graphicsPipelineStateDesc.PS = { pixelShaderBlob_->GetBufferPointer(),pixelShaderBlob_->GetBufferSize() };
    graphicsPipelineStateDesc.BlendState = blendDesc_;
    graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;
    //DepthStencilの設定
    graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc_;
    graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //書き込むRTVの情報
    graphicsPipelineStateDesc.NumRenderTargets = 1;
    graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    //利用するトポロジ（形状）のタイプ。三角形
    graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    //どのように画面に色を打ち込むかの設定（気にしなくていい）
    graphicsPipelineStateDesc.SampleDesc.Count = 1;
    graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    //実際に生成
    result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
    assert(SUCCEEDED(result));

    // カリングしない（裏面も表示させる）
    rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
}

void ParticleManager::CreateRootSignature()
{
    HRESULT result = S_FALSE;

    //ディスクリプタレンジの生成
    descriptorRange_[0].BaseShaderRegister = 0;
    descriptorRange_[0].NumDescriptors = 1;
    descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    descriptionRootSignature_.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    //RootParameter作成。複数設定できるので配列。今回は結果１つだけなので長さ１の配列
    D3D12_ROOT_PARAMETER rootParameter[3] = {};
    rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameter[0].Descriptor.ShaderRegister = 0;

    rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameter[1].DescriptorTable.pDescriptorRanges = descriptorRange_;
    rootParameter[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);

    rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameter[2].DescriptorTable.pDescriptorRanges = descriptorRange_;
    rootParameter[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_);

    descriptionRootSignature_.pParameters = rootParameter;
    descriptionRootSignature_.NumParameters = _countof(rootParameter);

    //Smaplerの設定
    staticSamplers_[0].Filter = D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
    staticSamplers_[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    staticSamplers_[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers_[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers_[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSamplers_[0].ShaderRegister = 0;
    staticSamplers_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    descriptionRootSignature_.pStaticSamplers = staticSamplers_;
    descriptionRootSignature_.NumStaticSamplers = _countof(staticSamplers_);



    // シリアライズしてバイナリにする
    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    result = D3D12SerializeRootSignature(&descriptionRootSignature_,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(result)) {
        Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }
    // バイナリを元に生成
    result = dxCommon_->GetDevice()->CreateRootSignature(0,
        signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(result));


    // InputLayoutの設定
    inputElementDescs_[0].SemanticName = "POSITION";
    inputElementDescs_[0].SemanticIndex = 0;
    inputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs_[1].SemanticName = "TEXCOORD";
    inputElementDescs_[1].SemanticIndex = 0;
    inputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputElementDescs_[2].SemanticName = "NORMAL";
    inputElementDescs_[2].SemanticIndex = 0;
    inputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    inputLayoutDesc_.pInputElementDescs = inputElementDescs_;
    inputLayoutDesc_.NumElements = _countof(inputElementDescs_);

    // BlendStateの設定
    // 全ての色要素を書き込む
    blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc_.RenderTarget[0].BlendEnable = TRUE;
    blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//zero
    blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    // RasterizerStateの設定
    // 裏面(時計回り)の表示の有無 (NONE / BACK)
    rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
    // 塗りつぶすかどうか
    rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

    // Shaderをコンパイル 
    vertexShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Particle.VS.hlsl", L"vs_6_0");
    assert(vertexShaderBlob_ != nullptr);
    pixelShaderBlob_ = dxCommon_->CompileShader(L"resources/shaders/Particle.PS.hlsl", L"ps_6_0");
    assert(pixelShaderBlob_ != nullptr);

    // DepthStencilStateの設定
    // Depthの機能を有効化
    depthStencilDesc_.DepthEnable = true;
    depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;//D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilePath, const std::string& modelFilePath, const std::string& type, const std::string& motionName)
{
    ModelManager::GetInstance()->LoadModel(modelFilePath);

    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Initialize(modelCommon_, "resources/models", modelFilePath);
    models_[name] = std::move(model);

    if (particleGroups.contains(name))
    {
        return;
    }

    // パーティクルグループを作成、コンテナに登録
    ParticleGroup newGroup = {};
    newGroup.motionName = motionName;
    particleGroups.insert(std::make_pair(name, std::move(newGroup)));

    // テクスチャファイルパスを登録
    particleGroups.at(name).materialData.textureFilePath = textureFilePath;
    // テクスチャを読み込んでSRVを生成
    TextureManager::GetInstance()->LoadTexture(textureFilePath);
    // SRVインデックスを登録
    particleGroups.at(name).materialData.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
    // 最大数インスタンス
    uint32_t MaxInstanceCount = 1024;
    // インスタンス数を初期化
    particleGroups.at(name).instanceCount = 0;
    // インスタンス用リソースを生成
    particleGroups.at(name).instancingResource = dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * MaxInstanceCount);
    // インスタンス用リソースをマップ
    particleGroups.at(name).instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroups.at(name).instancingData));
    // インスタンスのデータを初期化
    ParticleForGPU particleForGPU = {};
    particleForGPU.WVP = MakeIdentity4x4();
    particleForGPU.world = MakeIdentity4x4();
    particleForGPU.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    // インスタンスのデータを登録
    for (uint32_t i = 0; i < MaxInstanceCount; ++i)
    {
        particleGroups.at(name).instancingData[i] = particleForGPU;

        particleGroups.at(name).instancingData[i].WVP = MakeIdentity4x4();
        particleGroups.at(name).instancingData[i].world = MakeIdentity4x4();
        particleGroups.at(name).instancingData[i].color = Vector4(0, 0, 0, 0); // 完全に透明
    }

    // インスタンス用のSRVインデックス
    particleGroups.at(name).srvIndex = srvManager_->Allocate();
    // srvを生成
    srvManager_->CreateSRVforStructuredBuffer(particleGroups.at(name).srvIndex, particleGroups.at(name).instancingResource.Get(), MaxInstanceCount, sizeof(ParticleForGPU));

    // モデルの頂点を構築
    using BuildFunc = std::function<void(Model*)>;

    static const std::unordered_map<std::string, BuildFunc> shapeBuilders = {
        { "Ring",     MeshBuilder::BuildRing },
        { "Cylinder", MeshBuilder::BuildCylinder },
        { "Cone",     MeshBuilder::BuildCone },
        { "Spiral",   MeshBuilder::BuildSpiral },
        { "Torus",    MeshBuilder::BuildTorus },
        { "Helix",    MeshBuilder::BuildHelix },
        { "Sphere",   MeshBuilder::BuildSphere },
        { "Triangle", MeshBuilder::BuildTriangle },
        { "Petal",    MeshBuilder::BuildPetal},
		{ "Cube",     MeshBuilder::BuildCube },
		{ "Line",     MeshBuilder::BuildLine },
    };

    // モデル構築後に呼ぶ
    auto it = shapeBuilders.find(type);
    if (it != shapeBuilders.end())
    {
        it->second(models_[name].get());
    }
}

void ParticleManager::Update()
{
	// TimeManagerからデルタタイムを取得
	const float dt = TimeManager::Instance().GetDeltaTime();
	
    camera_ = object3dCommon_->GetDefaultCamera();

    Matrix4x4 viewMatrix = camera_->GetViewMatrix();
    Matrix4x4 projectionMatrix = camera_->GetProjectionMatrix();
    Matrix4x4 billboardMatrix = backToFrontMatrix_ * viewMatrix;
    billboardMatrix.m[3][0] = 0.0f;
    billboardMatrix.m[3][1] = 0.0f;
    billboardMatrix.m[3][2] = 0.0f;

    if (std::isnan(billboardMatrix.m[0][0]) || std::isinf(billboardMatrix.m[0][0])) {
        Logger::Log("billboardMatrix に異常な値が含まれています！");
    }

    for (auto& [name, Particlegroup] : particleGroups)
    {
        uint32_t count = 0;
        // 各パーティクルの処理
        for (auto it = Particlegroup.particleList.begin(); it != Particlegroup.particleList.end();)
        {
            // 寿命が終わったパーティクルを削除
            if ((*it).currentTime >= (*it).lifeTime)
            {
                it = Particlegroup.particleList.erase(it);
                continue;
            }

            // パーティクルの位置を更新
            (*it).transform.translate += (*it).velocity * dt;
            // パーティクルの回転を更新
            (*it).transform.rotate += (*it).angularVelocity * dt;
            // パーティクルのスケールを更新
            (*it).transform.scale += (*it).scaleVelocity * dt;
            // パーティクルの寿命
            (*it).currentTime += dt;
            float alpha = 1.0f - ((*it).currentTime / (*it).lifeTime);

            // アルファ値をパーティクルの色に適用
            (*it).color.w = alpha;


            // SRTからTranslateを分離
            Matrix4x4 SR =
                MakeScaleMatrix((*it).transform.scale) *
                MakeRotateXMatrix((*it).transform.rotate.x) *
                MakeRotateYMatrix((*it).transform.rotate.y) *
                MakeRotateZMatrix((*it).transform.rotate.z);

            // ビルボード行列をSRに掛ける
            Matrix4x4 billboardSR = SR * billboardMatrix;

            // 最後にTranslateを掛ける
            Matrix4x4 worldMatrix = billboardSR * MakeTranslateMatrix((*it).transform.translate);

            Matrix4x4 wVPMatrix = worldMatrix * viewMatrix * projectionMatrix;

            // SRVバッファの最大数に安全チェック
            if (count < 1024)
            { 
                Particlegroup.instancingData[count].WVP = wVPMatrix;
                Particlegroup.instancingData[count].world = worldMatrix;
                Particlegroup.instancingData[count].color = it->color;
                ++count;
            }


            // 次のパーティクルへ
            ++it;
        }

        Particlegroup.instanceCount = count;

    }

    for (auto& setting : emitSettings_)
    {
        if (setting.isLooping && particleGroups.contains(setting.groupName))
        {
            // パーティクルを発生
            Emit(setting.groupName, setting.emitPosition, setting.emitCount);

        }
    }
}

void ParticleManager::Draw()
{
    if (particleGroups.empty())
    {
        return;
    }

    // 共通設定
    dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
    dxCommon_->GetCommandList()->SetPipelineState(pipelineState_.Get());
    dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (const auto& [name, ParticleGroup] : particleGroups)
    {
        if (ParticleGroup.instanceCount == 0) continue;

        // ---- グループごとのモデルを取得 ----
        Model* model = models_[name].get();
        assert(model != nullptr);
        UINT indexCount = static_cast<UINT>(model->GetModelData().indices.size());
        assert(indexCount < 10000);
        indexCount;

        // ---- モデルに合わせて Vertex/Index Buffer を設定 ----
        D3D12_VERTEX_BUFFER_VIEW vbv = model->GetVertexBufferView();
        D3D12_INDEX_BUFFER_VIEW ibv = model->GetIndexBufferView();

        dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vbv);
        dxCommon_->GetCommandList()->IASetIndexBuffer(&ibv);

        // ---- マテリアルは共通でも良い（必要なら個別対応可能） ----
        dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

        // ---- テクスチャとインスタンスSRV（逆にしているかも）----
        srvManager_->SetGraphicsRootDescriptorTable(1, ParticleGroup.srvIndex); // StructuredBuffer (instancing)
        srvManager_->SetGraphicsRootDescriptorTable(2, ParticleGroup.materialData.textureIndex); // Texture SRV

        // ---- 描画 ----
        dxCommon_->GetCommandList()->DrawIndexedInstanced(
            static_cast<UINT>(model->GetModelData().indices.size()),
            ParticleGroup.instanceCount,
            0, 0, 0);
    }

}

void ParticleManager::Emit(const std::string groupName, const Vector3& position, uint32_t count)
{
    // グループごとのmotionNameを使うEmit
    auto it = particleGroups.find(groupName);
    if (it == particleGroups.end())
    {
        return;
    }

    EmitSetting newSetting;
    newSetting.groupName = groupName;
    newSetting.motionName = it->second.motionName;
    newSetting.emitCount = count;
    newSetting.emitPosition = position;
    newSetting.isLooping = false; // 初期状態はループ無し
    emitSettings_.push_back(newSetting);


    // パーティクル生成
    ParticleGroup& group = it->second;
    for (uint32_t i = 0; i < count; ++i)
    {
        Particle p = ParticleMotion::Create(group.motionName, randomEngine_, position);
        p.motionName = group.motionName;
        group.particleList.push_back(p);
    }

    group.instanceCount = static_cast<uint32_t>(group.particleList.size());

}

void ParticleManager::AddEmitterSetting(const EmitSetting& setting)
{
    EmitSetting native;
    native.groupName = setting.groupName;
    native.motionName = setting.motionName;
    native.emitPosition = setting.emitPosition;
    native.emitCount = setting.emitCount;
    native.timer = setting.timer;
    native.isLooping = setting.isLooping;
    emitSettings_.push_back(native);
}

void ParticleManager::DebugUI()
{
    static std::string newGroupName = "MyGroup";
    static std::string selectedShape = "Ring";
    static const char* shapeOptions[] = { "Ring", "Cylinder", "Cone", "Spiral", "Torus", "Helix", "Sphere", "Petal","Triangle","Petal","Cube"};
    static int currentShape = 0;

    static int selectedGroupIndex = 0;
    static int currentMotion = 0;
    static std::string selectedMotion = "Homing";

    static int emitCount = 10;
    static Vector3 emitPosition = { 0.0f, 1.0f, 0.0f };

    // Motion 一覧取得
    const auto& motionMap = ParticleMotion::GetAll();
    static std::vector<std::string> motionNames;
    if (motionNames.empty())
    {
        for (const auto& [name, _] : motionMap)
        {
            motionNames.push_back(name);
        }
    }

    // グループ一覧取得
    std::vector<std::string> groupNames;
    for (const auto& [name, _] : particleGroups) {
        groupNames.push_back(name);
    }

#ifdef USE_IMGUI

    if (ImGui::Begin("Particle Control")) {

        // --- 新規作成用グループ名入力 ---
        char groupNameBuffer[128] = {};
        strcpy_s(groupNameBuffer, newGroupName.c_str());

        if (ImGui::InputText("New GroupName", groupNameBuffer, sizeof(groupNameBuffer))) {
            newGroupName = groupNameBuffer;
        }

        // --- 形状選択 ---
        if (ImGui::Combo("Shape", &currentShape, shapeOptions, IM_ARRAYSIZE(shapeOptions))) {
            selectedShape = shapeOptions[currentShape];
        }

        if (ImGui::Button("Create Group")) {
            CreateParticleGroup(newGroupName, "resources/images/monsterBall.png", "plane.obj", selectedShape, selectedMotion);

        }

        // --- モーション選択 ---
        if (ImGui::Combo("Motion", &currentMotion, [](void* data, int idx, const char** out_text) {
            const auto& names = *static_cast<std::vector<std::string>*>(data);
            if (idx < 0 || idx >= names.size()) return false;
            *out_text = names[idx].c_str();
            return true;
            }, (void*)&motionNames, (int)motionNames.size())) {
            selectedMotion = motionNames[currentMotion];
        }

        ImGui::Separator();

        ImGui::SliderInt("Emit Count", &emitCount, 1, 100);
        ImGui::InputFloat3("Emit Position", reinterpret_cast<float*>(&emitPosition));

        // --- 既存グループ選択 ---
        if (!groupNames.empty()) {
            ImGui::Text("Emit to Group:");
            ImGui::Combo("##GroupList", &selectedGroupIndex, [](void* data, int idx, const char** out_text) {
                const auto& names = *static_cast<std::vector<std::string>*>(data);
                if (idx < 0 || idx >= names.size()) return false;
                *out_text = names[idx].c_str();
                return true;
                }, (void*)&groupNames, (int)groupNames.size());
        }

        // --- Emit 一回だけ ---
        if (ImGui::Button("Emit Particle") && selectedGroupIndex < groupNames.size()) 
        {
            const std::string& groupToEmit = groupNames[selectedGroupIndex];
            Emit(groupToEmit, emitPosition, emitCount);
        }

        // --- ループ Emit の開始・停止 ---
        if (ImGui::Button("Emit Loop") && selectedGroupIndex < groupNames.size()) 
        {
            const std::string& selectedGroup = groupNames[selectedGroupIndex];

            // 同じグループが既に設定されていれば更新、なければ追加
            auto it = std::find_if(emitSettings_.begin(), emitSettings_.end(),
                [&](const EmitSetting& s) { return s.groupName == selectedGroup; });

            if (it != emitSettings_.end())
            {
                // 既にある → 上書き
                it->motionName = selectedMotion;
                it->emitPosition = emitPosition;
                it->emitCount = emitCount;
                it->isLooping = true;
            } 
            else
            {
                // 新規追加
                EmitSetting newSetting;
                newSetting.groupName = selectedGroup;
                newSetting.motionName = selectedMotion;
                newSetting.emitPosition = emitPosition;
                newSetting.emitCount = emitCount;
                newSetting.isLooping = true;
                emitSettings_.push_back(newSetting);
            }
        }
        else 
        {
            if (ImGui::Button("Stop Emit") && selectedGroupIndex < groupNames.size()) 
            {
                const std::string& selectedGroup = groupNames[selectedGroupIndex];

                // 該当グループだけ停止
                for (auto& setting : emitSettings_)
                {
                    if (setting.groupName == selectedGroup)
                    {
                        setting.isLooping = false;
                    }
                }
            }
        }

        
    }

    ImGui::End();

#endif // USE_IMGUI

}
