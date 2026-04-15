#pragma once

#include <cstdint>
#include <map>
#include <cmath>

#include "ComPtr.h"
#include "InputData.h"

namespace IIEngine 
{
	/// <summary>
	/// コントローラー
	/// </summary>
	class Controller
	{
	public:

        Controller() = default;
        ~Controller();

        /// <summary>
        /// 初期化処理
        /// </summary>
        void Initialize();

        /// <summary>
        /// 更新処理
        /// </summary>
        void Update();

        /// <summary>
        /// コントローラーのスティックを取得
        /// </summary>
        /// <param name="stickNo">コントローラーの番号</param>
        /// <param name="out">スティックの場所</param>
        /// <returns>フラグを返す</returns>
        bool GetJoystickState(int stickNo, XINPUT_STATE& out) const;
        bool GetJoystickStatePrevious(int stickNo, XINPUT_STATE& out) const;
        bool GetJoystickState(int stickNo, DIJOYSTATE2& out) const;
        bool GetJoystickStatePrevious(int stickNo, DIJOYSTATE2& out) const;

        /// <summary>
        /// コントローラーの押下チェック
        /// </summary>
        /// <param name="stickNo">コントローラーの番号</param>
        /// <param name="button">ボタンの種類</param>
        /// <returns>フラグを返す</returns>
        bool PushButton(int stickNo, ControllerButtonType button) const;
        bool TriggerButton(int stickNo, ControllerButtonType button) const;
        bool ReleaseButton(int stickNo, ControllerButtonType button) const;

        /// <summary>
        /// ボタンの押し込み量を取得
        /// </summary>
        /// <param name="stickNo">コントローラーの番号</param>
        /// <param name="button">ボタンの種類</param>
        /// <returns>押し込みの数値を返す</returns>
        float GetTriggerValue(int stickNo, ControllerButtonType button) const;

        /// <summary>
        /// スティックの状況を取得
        /// </summary>
        /// <param name="stickNo">コントローラーの番号</param>
        /// <returns>スティックの状態を返す</returns>
        StickState GetLeftStickState(int stickNo) const;
        StickState GetRightStickState(int stickNo) const;
        float GetStickValue(int stickNo, ControllerValueType valueType) const;

        /// <summary>
        /// スティックの前フレーム状態を取得する関数
        /// </summary>
        /// <param name="stickNo">コントローラーの番号</param>
        /// <returns>スティックの状態を返す</returns>
        StickState GetLeftStickStatePrevious(int stickNo) const;
        StickState GetRightStickStatePrevious(int stickNo) const;

        /// <summary>
        /// スティックのはじき（ Flick ）を検出する関数
        /// </summary>
        /// <param name="stickNo">コントローラーの番号</param>
        /// <param name="threshold">はじきを感知する大きさ</param>
        /// <returns>フラグを返す</returns>
        bool FlickLeftStick(int stickNo, float threshold = 0.7f) const;
        bool FlickRightStick(int stickNo, float threshold = 0.7f) const;

    private:

        /// <summary>
        /// 指定したボタンの XInput / DirectInput マッピングを取得
        /// </summary>
        /// <param name="button">ボタンの種類</param>
        /// <returns>ペアを返す</returns>
        std::pair<WORD, int> ConvertToButton(ControllerButtonType button) const;

    private:

        static constexpr float NORMALIZE_RANGE = 32768.0f; // XInputのスティック最大値
        static constexpr float TRIGGER_THRESHOLD = 128.0f; // トリガーボタン判定のしきい値
        static constexpr float DEADZONE = 0.15f; // スティックのデッドゾーン（15%）

        // XInput 対応用
        XINPUT_STATE currentState_[XUSER_MAX_COUNT]{};
        XINPUT_STATE previousState_[XUSER_MAX_COUNT]{};

        // DirectInput 対応用
        DIJOYSTATE2 currentDIState_[XUSER_MAX_COUNT]{};
        DIJOYSTATE2 previousDIState_[XUSER_MAX_COUNT]{};

        std::map<ControllerButtonType, std::pair<WORD, int>> buttonMapping_;

	};

}