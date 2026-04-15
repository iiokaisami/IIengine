#include "Controller.h"

#include <cassert>
#include <algorithm>

namespace IIEngine
{
	Controller::~Controller(){}

	void Controller::Initialize()
	{
		ZeroMemory(currentState_, sizeof(currentState_));
		ZeroMemory(previousState_, sizeof(previousState_));
		ZeroMemory(currentDIState_, sizeof(currentDIState_));
		ZeroMemory(previousDIState_, sizeof(previousDIState_));

		// XInput / DirectInput のボタンマッピング
		buttonMapping_ = {
			{ControllerButtonType::A, {static_cast<WORD>(XINPUT_GAMEPAD_A), 0}},  // Aボタン
			{ControllerButtonType::B, {static_cast<WORD>(XINPUT_GAMEPAD_B), 1}},  // Bボタン
			{ControllerButtonType::X, {static_cast<WORD>(XINPUT_GAMEPAD_X), 2}},  // Xボタン
			{ControllerButtonType::Y, {static_cast<WORD>(XINPUT_GAMEPAD_Y), 3}},  // Yボタン
			{ControllerButtonType::RB, {static_cast<WORD>(XINPUT_GAMEPAD_RIGHT_SHOULDER), 5}},  // RB
			{ControllerButtonType::LB, {static_cast<WORD>(XINPUT_GAMEPAD_LEFT_SHOULDER), 4}},   // LB
			{ControllerButtonType::DPadUP, {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_UP), 10}},    // DPad ↑
			{ControllerButtonType::DPadDOWN, {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_DOWN), 12}}, // DPad ↓
			{ControllerButtonType::DPadLEFT, {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_LEFT), 13}}, // DPad ←
			{ControllerButtonType::DPadRIGHT, {static_cast<WORD>(XINPUT_GAMEPAD_DPAD_RIGHT), 11}}, // DPad →
			{ControllerButtonType::LeftStick, {static_cast<WORD>(XINPUT_GAMEPAD_LEFT_THUMB), 6}},   // Lスティック押し込み
			{ControllerButtonType::RightStick, {static_cast<WORD>(XINPUT_GAMEPAD_RIGHT_THUMB), 7}}, // Rスティック押し込み
			{ControllerButtonType::Select, {static_cast<WORD>(XINPUT_GAMEPAD_BACK), 8}},    // セレクトボタン
			{ControllerButtonType::Start, {static_cast<WORD>(XINPUT_GAMEPAD_START), 9}},    // スタートボタン
		};
	}

	void Controller::Update() 
	{
		for (int i = 0; i < XUSER_MAX_COUNT; ++i) 
		{
			previousState_[i] = currentState_[i];
			ZeroMemory(&currentState_[i], sizeof(XINPUT_STATE));
			XInputGetState(i, &currentState_[i]);

			previousDIState_[i] = currentDIState_[i];
			ZeroMemory(&currentDIState_[i], sizeof(DIJOYSTATE2));
		}
	}


	bool Controller::GetJoystickState(int stickNo, XINPUT_STATE& out) const
	{
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return false;
		out = currentState_[stickNo];
		return true;
	}
	
	bool Controller::GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out) const
	{
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return false;
		out = previousState_[stickNo];
		return true;
	}
	
	bool Controller::GetJoystickState(int stickNo, DIJOYSTATE2& out) const
	{
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return false;
		out = currentDIState_[stickNo];
		return true;
	}
	
	bool Controller::GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out) const
	{
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return false;
		out = previousDIState_[stickNo];
		return true;
	}
	
	bool Controller::PushButton(int stickNo, ControllerButtonType button) const
	{
		auto [xInputButton, dInputButton] = ConvertToButton(button);

		// XInput の通常ボタン
		if ((currentState_[stickNo].Gamepad.wButtons & xInputButton))
		{
			return true;
		}

		// DirectInput のボタン処理
		if (dInputButton >= 0 && (currentDIState_[stickNo].rgbButtons[dInputButton] & 0x80))
		{
			return true;
		}

		// LT(左トリガー) DirectInput 追加
		if (button == ControllerButtonType::LT) 
		{
			return (currentState_[stickNo].Gamepad.bLeftTrigger > TRIGGER_THRESHOLD ||
				currentDIState_[stickNo].lZ > 32767); // DirectInput のトリガー処理
		}

		// RT(右トリガー) DirectInput 追加
		if (button == ControllerButtonType::RT) 
		{
			return (currentState_[stickNo].Gamepad.bRightTrigger > TRIGGER_THRESHOLD ||
				currentDIState_[stickNo].lRz > 32767);
		}

		return false;
	}
	
	bool Controller::TriggerButton(int stickNo, ControllerButtonType button) const
	{
		auto [xInputButton, dInputButton] = ConvertToButton(button);

		// XInput の通常ボタン
		if ((currentState_[stickNo].Gamepad.wButtons & xInputButton) &&
			!(previousState_[stickNo].Gamepad.wButtons & xInputButton))
		{
			return true;
		}

		// DirectInput のボタン処理
		if (dInputButton >= 0 &&
			(currentDIState_[stickNo].rgbButtons[dInputButton] & 0x80) &&
			!(previousDIState_[stickNo].rgbButtons[dInputButton] & 0x80))
		{
			return true;
		}

		// LT(左トリガー)
		if (button == ControllerButtonType::LT)
		{
			return (currentState_[stickNo].Gamepad.bLeftTrigger > TRIGGER_THRESHOLD &&
				previousState_[stickNo].Gamepad.bLeftTrigger <= TRIGGER_THRESHOLD) ||
				(currentDIState_[stickNo].lZ > 32767 && previousDIState_[stickNo].lZ <= 32767);
		}

		// RT(右トリガー)
		if (button == ControllerButtonType::RT)
		{
			return (currentState_[stickNo].Gamepad.bRightTrigger > TRIGGER_THRESHOLD &&
				previousState_[stickNo].Gamepad.bRightTrigger <= TRIGGER_THRESHOLD) ||
				(currentDIState_[stickNo].lRz > 32767 && previousDIState_[stickNo].lRz <= 32767);
		}

		return false;
	}
	
	bool Controller::ReleaseButton(int stickNo, ControllerButtonType button) const
	{
		auto [xInputButton, dInputButton] = ConvertToButton(button);

		// XInput の通常ボタン
		if (!(currentState_[stickNo].Gamepad.wButtons & xInputButton) &&
			(previousState_[stickNo].Gamepad.wButtons & xInputButton)) 
		{
			return true;
		}

		// DirectInput のボタン処理
		if (dInputButton >= 0 &&
			!(currentDIState_[stickNo].rgbButtons[dInputButton] & 0x80) &&
			(previousDIState_[stickNo].rgbButtons[dInputButton] & 0x80))
		{
			return true;
		}

		// LT(左トリガー) DirectInput 追加
		if (button == ControllerButtonType::LT) 
		{
			return (currentState_[stickNo].Gamepad.bLeftTrigger <= TRIGGER_THRESHOLD &&
				previousState_[stickNo].Gamepad.bLeftTrigger > TRIGGER_THRESHOLD) ||
				(currentDIState_[stickNo].lZ <= 32767 && previousDIState_[stickNo].lZ > 32767);
		}

		// RT(右トリガー) DirectInput 追加
		if (button == ControllerButtonType::RT)
		{
			return (currentState_[stickNo].Gamepad.bRightTrigger <= TRIGGER_THRESHOLD &&
				previousState_[stickNo].Gamepad.bRightTrigger > TRIGGER_THRESHOLD) ||
				(currentDIState_[stickNo].lRz <= 32767 && previousDIState_[stickNo].lRz > 32767);
		}

		return false;
	}
	
	float Controller::GetTriggerValue(int stickNo, ControllerButtonType button) const
	{
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return 0.0f;

		switch (button)
		{
			// XInput / DirectInput の LT(左トリガー)
		case ControllerButtonType::LT:
		{
			float lTriggerDI = static_cast<float>(currentDIState_[stickNo].lZ) / 65535.0f;
			float lTriggerX = static_cast<float>(currentState_[stickNo].Gamepad.bLeftTrigger) / 255.0f;
			return std::max<float>(lTriggerX, std::clamp<float>(lTriggerDI, 0.0f, 1.0f));
		}

		// XInput / DirectInput の RT(右トリガー)
		case ControllerButtonType::RT:
		{
			float rTriggerDI = static_cast<float>(currentDIState_[stickNo].lRz) / 65535.0f;
			float rTriggerX = static_cast<float>(currentState_[stickNo].Gamepad.bRightTrigger) / 255.0f;
			return std::max<float>(rTriggerX, std::clamp<float>(rTriggerDI, 0.0f, 1.0f));
		}

		// XInput / DirectInput の LB(左バンパー)
		case ControllerButtonType::LB:
			return ((currentState_[stickNo].Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) ||
				(currentDIState_[stickNo].rgbButtons[4] & 0x80)) ? 1.0f : 0.0f;

			// XInput / DirectInput の RB(右バンパー)
		case ControllerButtonType::RB:
			return ((currentState_[stickNo].Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) ||
				(currentDIState_[stickNo].rgbButtons[5] & 0x80)) ? 1.0f : 0.0f;

		default:
			return 0.0f;
		}
	
	}
	
	StickState Controller::GetLeftStickState(int stickNo) const
	{
		StickState state = { 0.0f, 0.0f };
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return state;

		// XInput のスティック値(-1.0f ～ 1.0f)
		float lxXInput = static_cast<float>(currentState_[stickNo].Gamepad.sThumbLX) / NORMALIZE_RANGE;
		float lyXInput = static_cast<float>(currentState_[stickNo].Gamepad.sThumbLY) / NORMALIZE_RANGE;

		// DirectInput のスティック値(0 ～ 65535 → -1.0f ～ 1.0f に変換)
		float lxDInput = (static_cast<float>(currentDIState_[stickNo].lX) - 32767.0f) / 32767.0f;
		float lyDInput = (static_cast<float>(currentDIState_[stickNo].lY) - 32767.0f) / 32767.0f; // Y軸は反転

		// デッドゾーン処理(微小な入力を無視)
		if (std::abs(lxXInput) < DEADZONE) lxXInput = 0.0f;
		if (std::abs(lyXInput) < DEADZONE) lyXInput = 0.0f;
		if (std::abs(lxDInput) < DEADZONE) lxDInput = 0.0f;
		if (std::abs(lyDInput) < DEADZONE) lyDInput = 0.0f;

		// XInput と DirectInput の大きい方を採用
		state.x = std::max<float>(lxXInput, std::clamp<float>(lxDInput, -1.0f, 1.0f));
		state.y = std::max<float>(lyXInput, std::clamp<float>(lyDInput, -1.0f, 1.0f));

		return state;
	}
	
	StickState Controller::GetRightStickState(int stickNo) const
	{
		StickState state = { 0.0f, 0.0f };
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return state;

		// XInput のスティック値(-1.0f ～ 1.0f)
		float rxXInput = static_cast<float>(currentState_[stickNo].Gamepad.sThumbRX) / NORMALIZE_RANGE;
		float ryXInput = static_cast<float>(currentState_[stickNo].Gamepad.sThumbRY) / NORMALIZE_RANGE;

		// DirectInput のスティック値(0 ～ 65535 → -1.0f ～ 1.0f に変換)
		float rxDInput = (static_cast<float>(currentDIState_[stickNo].lRx) - 32767.0f) / 32767.0f;
		float ryDInput = (static_cast<float>(currentDIState_[stickNo].lRy) - 32767.0f) / 32767.0f; // Y軸は反転

		// デッドゾーン処理(微小な入力を無視)
		if (std::abs(rxXInput) < DEADZONE) rxXInput = 0.0f;
		if (std::abs(ryXInput) < DEADZONE) ryXInput = 0.0f;
		if (std::abs(rxDInput) < DEADZONE) rxDInput = 0.0f;
		if (std::abs(ryDInput) < DEADZONE) ryDInput = 0.0f;

		// XInput と DirectInput の大きい方を採用
		state.x = std::max<float>(rxXInput, std::clamp<float>(rxDInput, -1.0f, 1.0f));
		state.y = std::max<float>(ryXInput, std::clamp<float>(ryDInput, -1.0f, 1.0f));

		return state;
	}
	
	float Controller::GetStickValue(int stickNo, ControllerValueType valueType) const
	{
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return 0.0f;

		// XInput / DirectInput のスティック値
		float xInputValue = 0.0f;
		float dInputValue = 0.0f;

		switch (valueType) 
		{
		case ControllerValueType::LX:
			xInputValue = static_cast<float>(currentState_[stickNo].Gamepad.sThumbLX) / NORMALIZE_RANGE;
			dInputValue = static_cast<float>(currentDIState_[stickNo].lX) / 32767.0f; // DirectInput のスケール調整
			break;
		case ControllerValueType::LY:
			xInputValue = static_cast<float>(currentState_[stickNo].Gamepad.sThumbLY) / NORMALIZE_RANGE;
			dInputValue = static_cast<float>(currentDIState_[stickNo].lY) / 32767.0f;
			break;
		case ControllerValueType::RX:
			xInputValue = static_cast<float>(currentState_[stickNo].Gamepad.sThumbRX) / NORMALIZE_RANGE;
			dInputValue = static_cast<float>(currentDIState_[stickNo].lRx) / 32767.0f;
			break;
		case ControllerValueType::RY:
			xInputValue = static_cast<float>(currentState_[stickNo].Gamepad.sThumbRY) / NORMALIZE_RANGE;
			dInputValue = static_cast<float>(currentDIState_[stickNo].lRy) / 32767.0f;
			break;
		default:
			return 0.0f;
		}

		// デッドゾーン処理(微小な入力を無視)
		if (std::abs(xInputValue) < DEADZONE) xInputValue = 0.0f;
		if (std::abs(dInputValue) < DEADZONE) dInputValue = 0.0f;

		// XInput と DirectInput の値のうち、大きい方を採用
		return std::max<float>(xInputValue, std::clamp<float>(dInputValue, -1.0f, 1.0f));
	}
	
	StickState Controller::GetLeftStickStatePrevious(int stickNo) const
	{
		StickState state = { 0.0f, 0.0f };
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return state;

		float lxXInput = static_cast<float>(previousState_[stickNo].Gamepad.sThumbLX) / NORMALIZE_RANGE;
		float lyXInput = static_cast<float>(previousState_[stickNo].Gamepad.sThumbLY) / NORMALIZE_RANGE;

		float lxDInput = (static_cast<float>(previousDIState_[stickNo].lX) - 32767.0f) / 32767.0f;
		float lyDInput = (static_cast<float>(previousDIState_[stickNo].lY) - 32767.0f) / 32767.0f;

		if (std::abs(lxXInput) < DEADZONE) lxXInput = 0.0f;
		if (std::abs(lyXInput) < DEADZONE) lyXInput = 0.0f;
		if (std::abs(lxDInput) < DEADZONE) lxDInput = 0.0f;
		if (std::abs(lyDInput) < DEADZONE) lyDInput = 0.0f;

		state.x = std::max<float>(lxXInput, std::clamp<float>(lxDInput, -1.0f, 1.0f));
		state.y = std::max<float>(lyXInput, std::clamp<float>(lyDInput, -1.0f, 1.0f));

		return state;
	}
	
	StickState Controller::GetRightStickStatePrevious(int stickNo) const
	{
		StickState state = { 0.0f, 0.0f };
		if (stickNo < 0 || stickNo >= XUSER_MAX_COUNT) return state;

		float rxXInput = static_cast<float>(previousState_[stickNo].Gamepad.sThumbRX) / NORMALIZE_RANGE;
		float ryXInput = static_cast<float>(previousState_[stickNo].Gamepad.sThumbRY) / NORMALIZE_RANGE;

		float rxDInput = (static_cast<float>(previousDIState_[stickNo].lRx) - 32767.0f) / 32767.0f;
		float ryDInput = (static_cast<float>(previousDIState_[stickNo].lRy) - 32767.0f) / 32767.0f;

		if (std::abs(rxXInput) < DEADZONE) rxXInput = 0.0f;
		if (std::abs(ryXInput) < DEADZONE) ryXInput = 0.0f;
		if (std::abs(rxDInput) < DEADZONE) rxDInput = 0.0f;
		if (std::abs(ryDInput) < DEADZONE) ryDInput = 0.0f;

		state.x = std::max<float>(rxXInput, std::clamp<float>(rxDInput, -1.0f, 1.0f));
		state.y = std::max<float>(ryXInput, std::clamp<float>(ryDInput, -1.0f, 1.0f));

		return state;
	}
	
	bool Controller::FlickLeftStick(int stickNo, float threshold) const
	{
		StickState prev = GetLeftStickStatePrevious(stickNo);
		StickState curr = GetLeftStickState(stickNo);

		float prevLen = std::sqrt(prev.x * prev.x + prev.y * prev.y);
		float currLen = std::sqrt(curr.x * curr.x + curr.y * curr.y);

		return (prevLen < DEADZONE && currLen >= threshold);
	}
	
	bool Controller::FlickRightStick(int stickNo, float threshold) const
	{
		StickState prev = GetRightStickStatePrevious(stickNo);
		StickState curr = GetRightStickState(stickNo);

		float prevLen = std::sqrt(prev.x * prev.x + prev.y * prev.y);
		float currLen = std::sqrt(curr.x * curr.x + curr.y * curr.y);

		return (prevLen < DEADZONE && currLen >= threshold);
	}
	
	std::pair<WORD, int> Controller::ConvertToButton(ControllerButtonType button) const
	{
		auto it = buttonMapping_.find(button);
		if (it != buttonMapping_.end()) 
		{
			return it->second;
		}
		return std::make_pair(static_cast<WORD>(0), -1); // 無効なボタン
	}
}