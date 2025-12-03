#pragma once
#include <windows.h>
#include <wrl.h>
using namespace Microsoft::WRL;

//Input
#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>

#include "WinApp.h"

/// <summary>
/// 入力クラス
/// DirectInputを使用してキーボードの入力を管理
/// </summary>
class Input
{
#pragma region シングルトンインスタンス
private:

	Input() = default;
	~Input() = default;
	Input(Input&) = delete;
	Input& operator = (Input&) = delete;

public:
	// シングルトンインスタンスの取得
	static Input* GetInstance();
	// 終了
	void Finalize();
#pragma endregion シングルトンインスタンス

public:

	// namespace省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApp">WindowsAPI</param>
	void Initialize(WinApp* winApp);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

    /// <summary>
    /// キーの押下をチェック
    /// </summary>
    /// <perm name="keyNumber">キー番号( DIK_0 等)</param>
	///<returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

    /// <summary>
    /// キーのトリガーをチェック
    /// </summary>
	/// <param name="KeyNumber">キー番号( DIK_0 等 )</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

private:

	// WindowsAPI
	WinApp* winApp_ = nullptr;

	// DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput;

	// キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	// 全キーの状態
	BYTE key[256] = {};
	// 前回の全キーの状態
	BYTE keyPre[256] = {};
};

