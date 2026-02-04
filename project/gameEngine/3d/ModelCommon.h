#pragma once

#include <d3d12.h>
//#include "externals/DirectXTex/d3dx12.h"
#include "../../externals/DirectXTex/d3dx12.h"
#include "DirectXCommon.h"
#include "Logger.h"

namespace IIEngine
{

	/// <summary>
	/// モデル共通
	/// </summary>
	class ModelCommon
	{
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		/// <param name="dxCommon"> DirectX共通機能管理クラスへのポインタ</param>
		void Initialize(DirectXCommon* dxCommon);

	public: // ゲッター

		/// <summary>
		/// DirectX共通機能管理クラスの取得
		/// </summary>
		DirectXCommon* GetDxCommon() const { return dxCommon_; }

	private:

		DirectXCommon* dxCommon_;


	};

}