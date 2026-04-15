#include "Input.h"

#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

namespace IIEngine
{

	Input* Input::GetInstance()
	{
		static Input instance;
		return &instance;
	}

	void Input::Finalize() {}

	void Input::Initialize(WinApp* winApp)
	{
		// 借りてきたWinAppのインスタンスを記録
		this->winApp_ = winApp;

		HRESULT result;

		// DirectInputの初期化
		result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
		assert(SUCCEEDED(result));

		// キーボードデバイスの生成

		result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
		assert(SUCCEEDED(result));

		// 入力データ形式のセット
		result = keyboard->SetDataFormat(&c_dfDIKeyboard);
		assert(SUCCEEDED(result));

		// 排他的制御レベルのセット
		result = keyboard->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
		assert(SUCCEEDED(result));

		// コントローラーの初期化
		controller_.Initialize();
	}

	void Input::Update()
	{

		// 前回のキー入力を保存
		memcpy(keyPre, key, sizeof(key));

		// キーボード情報の取得開始
		keyboard->Acquire();
		// 全キーの入力状態を取得する
		keyboard->GetDeviceState(sizeof(key), key);

		// コントローラーの更新
		controller_.Update();

	}

	bool Input::PushKey(BYTE keyNumber)const
	{
		// 指定キーを押していれば trueを返す
		if (key[keyNumber])
		{
			return true;
		}

		// そうでなければ falseを返す
		return false;
	}

	bool Input::TriggerKey(BYTE keyNumber)const
	{
		if (!keyPre[keyNumber] && key[keyNumber])
		{
			return true;
		}

		return false;
	}

	bool Input::IsPadConnected() const
	{
		XINPUT_STATE st{};
		DWORD r = XInputGetState(0, &st);
		return (r == ERROR_SUCCESS);
	}
	
	bool Input::PushPadButton(ControllerButtonType button) const
	{
		return controller_.PushButton(0, button);
	}
	
	bool Input::TriggerPadButton(ControllerButtonType button) const
	{
		return controller_.TriggerButton(0, button);
	}
	
	bool Input::ReleasePadButton(ControllerButtonType button) const
	{
		return controller_.ReleaseButton(0, button);
	}
	
	StickState Input::GetLeftStick() const
	{
		return controller_.GetLeftStickState(0);
	}
	
	StickState Input::GetRightStick() const
	{
		return controller_.GetRightStickState(0);
	}
	
	float Input::GetTriggerValue(ControllerButtonType button) const
	{
		return controller_.GetTriggerValue(0, button);
	}
}