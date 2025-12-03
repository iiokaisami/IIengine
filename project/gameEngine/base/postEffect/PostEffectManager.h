#pragma once

#include "IPostEffectPass.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <vector>

/// <summary>
/// ポストエフェクトマネージャー
/// ポストエフェクトのパスを管理し、描画を行う
/// </summary>
class PostEffectManager 
{
public:

    static PostEffectManager* GetInstance();

    /// <summary>
	/// ポストエフェクトパスの追加
    /// </summary>
	/// <param name="name">パス名</param>
	/// <param name="pass">ポストエフェクトパスのユニークポインタ</param>
    void AddPass(const std::string& name, std::unique_ptr<IPostEffectPass> pass);
    
	/// <summary>
	/// アクティブ設定
    /// 1つのみ有効
    /// </summary>
	/// <param name="name">パス名</param>
	/// <param name="active">アクティブフラグ</param>
    void SetActiveEffect(const std::string& name, bool active);
    
    /// <summary>
	/// アクティブ設定
    /// 複数可
    /// </summary>
	/// <param name="names">パス名リスト</param>
    void SetActiveEffects(const std::vector<std::string>& names); 
    
	/// <summary>
	/// 全ポストエフェクトパス描画
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	/// <param name="inputSrvHandle">入力テクスチャのSRVハンドル</param>
	/// <param name="inputResource">入力リソース</param>
	/// <param name="currentState">入力リソースの現在の状態</param>
    void DrawAll(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList, D3D12_GPU_DESCRIPTOR_HANDLE inputSrvHandle, ID3D12Resource* inputResource, D3D12_RESOURCE_STATES& currentState);

	/// <summary>
	/// Noneエフェクト設定
	/// 何も効果をかけないパス
	/// </summary>
	/// <param name="pass">ポストエフェクトパスのユニークポインタ</param>
    void SetNoneEffect(std::unique_ptr<IPostEffectPass> pass);

    template<typename T>
    T* GetPassAs(const std::string& name)
    {
        auto it = passes_.find(name);
        if (it != passes_.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }

private:

    std::unordered_map<std::string, std::unique_ptr<IPostEffectPass>> passes_;

    std::unique_ptr<IPostEffectPass> noneEffect_;

};
