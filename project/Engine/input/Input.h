#pragma once

#include <windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Xinput.h>

#include "WinApp.h"
#include <memory>

/// <summary>
/// 入力
/// </summary>
class Input{
public:
	/// <summary>
	/// インスタンス生成
	/// </summary>
	/// <returns></returns>
	static Input& GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	//using namespace Microsoft::WRL の代わり
	template <class T> using ComPtr =  Microsoft::WRL::ComPtr<T>;
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="winApp"></param>
	void Initialize(WinApp* winApp);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// キーボードインプット(長押し)
	/// </summary>
	/// <param name="keyNumber">キーを選択</param>
	/// <returns>trueで押している</returns>
	bool PushKey(BYTE keyNumber);
	/// <summary>
	/// キーボードインプット(短押し)
	/// </summary>
	/// <param name="keyNumber">キーを選択</param>
	/// <returns>trueで押している</returns>
	bool TriggerKey(BYTE keyNumber);
	/// <summary>
	/// ゲームパットボタン操作(長押し)
	/// </summary>
	/// <param name="button">ボタンを選択</param>
	/// <returns>trueで押している</returns>
	bool PushButton(int button);
	/// <summary>
	/// ゲームパットボタン(短押し)
	/// </summary>
	/// <param name="button">ボタンを選択</param>
	/// <returns>trueで押している</returns>
	bool TriggerButton(int button);
	/// <summary>
	/// ゲームパットのLT(長押し)
	/// </summary>
	/// <returns>trueで押している</returns>
	bool LeftTriggerLongPress();
	/// <summary>
	/// ゲームパットのRT(長押し)
	/// </summary>
	/// <returns>trueで押している</returns>
	bool RightTriggerLongPress();
	/// <summary>
	/// ゲームパットのLT(短押し)
	/// </summary>
	/// <returns>trueで押している</returns>
	bool LeftTrigger();
	/// <summary>
	/// ゲームパットのRT(短押し)
	/// </summary>
	/// <returns>trueで押している</returns>
	bool RightTrigger();
	/// <summary>
	/// ゲームパットのLスティックの横の傾き具合
	/// </summary>
	/// <returns>横の傾き具合</returns>
	float LeftStickX();
	/// <summary>
	/// ゲームパットのLスティックの縦の傾き具合
	/// </summary>
	/// <returns>縦の傾き具合</returns>
	float LeftStickY();
	/// <summary>
	/// ゲームパットのRスティックの横の傾き具合
	/// </summary>
	/// <returns>横の傾き具合</returns>
	float RightStickX();
	/// <summary>
	/// ゲームパットのRスティックの縦の傾き具合
	/// </summary>
	/// <returns>縦の傾き具合</returns>
	float RightStickY();
	/// <summary>
	/// ゲームパット
	/// </summary>
	/// <returns>アクティブであるか+更新</returns>
	bool GetJoystickState();
	/// <summary>
	/// ゲームパット(アクティブかのみ)
	/// </summary>
	/// <returns>アクティブであるか</returns>
	bool GetActiveGamePad();
private:

	ComPtr<IDirectInputDevice8> keyboard_;
	ComPtr<IDirectInput8> directInput_;
	BYTE key_[256] = {};
	BYTE keyPre_[256] = {};
	WinApp* winApp_ = nullptr; //協力関係
	//インスタンス
	static std::unique_ptr<Input> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<Input>;

	//ゲームパット用の入力変数
	XINPUT_STATE state_, preState_;
	//傾け度合
	const float kInclination = 32768.0f;

	ComPtr<IDirectInputDevice8> mouseInput_;

	DIMOUSESTATE2 mouseState_;
	DIMOUSESTATE2 mousePreState_;

	POINT p{};

	int centerX = WinApp::kClientWidth_ / 2;
	int centerY = WinApp::kClientHeight_ / 2;

	float mouseX_ = WinApp::kClientWidth_ / 2;
	float mouseY_ = WinApp::kClientHeight_ / 2;

	bool isActiveGamePad_ = false;
	bool mouseSensitivity_;
};