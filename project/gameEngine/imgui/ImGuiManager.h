#pragma once

#include "DirectXCommon.h"

namespace IIEngine
{

	/// <summary>
	/// ImGui管理クラス
	/// <para>ImGuiの初期化、終了、描画を行う</para>
	/// </summary>
	class ImGuiManager
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="winApp">WindowsAPI</param>
		/// <param name="dxCommon">DirectX共通機能管理クラスへのポインタ</param>
		void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

		/// <summary>
		/// 終了
		/// </summary>
		void Finalize();

		/// <summary>
		/// ImGui受付開始
		/// </summary>
		void Begin();

		/// <summary>
		/// ImGui受付終了
		/// </summary>
		void End();

		/// <summary>
		/// 描画
		/// </summary>
		void Draw();

	private:

		// WindowsAPI
		WinApp* winApp_ = nullptr;

		DirectXCommon* dxCommon_ = nullptr;

		// SRV用デスクリプタヒープ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;

	};

}