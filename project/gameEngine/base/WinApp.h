#pragma once

#include <windows.h>
#include <cstdint>

#ifdef USE_IMGUI

//ImGui
#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_dx12.h"

#endif // USE_IMGUI

namespace IIEngine
{

	/// <summary>
	/// Windowsアプリケーションクラス
	/// </summary>
	class WinApp
	{
	public:

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="hwnd">ウィンドウハンドル</param>
		/// <param name="msg">メッセージ</param>
		/// <param name="wparam">追加情報1</param>
		/// <param name="lparam">追加情報2</param>
		/// <returns>処理結果</returns>
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	public:

		//クライアント領域のサイズ
		static const int32_t kClientWidth = 1280;
		static const int32_t kClientHeight = 720;


		// 初期化
		void Initialize();
		// 更新
		void Update();
		// 終了
		void Finalize();

		/// <summary>
		/// メッセージの処理
		/// </summary>
		/// <returns>処理結果</returns>
		bool ProcessMessage();

	public: // ゲッター

		// ウィンドウハンドルのゲッター
		HWND GetHwnd() const { return hwnd; }

		// インスタンスハンドルのゲッター
		HINSTANCE GetHInstance() const { return wc.hInstance; }


	private:

		// ウィンドウハンドル
		HWND hwnd = nullptr;

		//ウィンドウクラスの設定
		WNDCLASS wc{};

	};
}