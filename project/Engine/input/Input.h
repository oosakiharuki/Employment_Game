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
	static std::shared_ptr<Input> GetInstance();
	/// <summary>
	/// 解放処理
	/// </summary>
	void Finalize();

	//using namespace Microsoft::WRL の代わり
	template <class T> using ComPtr =  Microsoft::WRL::ComPtr<T>;

	void Initialize(WinApp* winApp);
	void Update();

	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);

	bool GetJoystickState(uint32_t num,XINPUT_STATE& state);
	bool GetJoystickStatePrevious(uint32_t num, XINPUT_STATE& state);

	bool PushBotton(XINPUT_STATE& state,int botton);
	bool TriggerBotton(XINPUT_STATE& state, XINPUT_STATE& preState, int botton);

	/// <summary>
	/// ゲームパット用の更新処理
	/// </summary>
	/// <param name="state"></param>
	/// <param name="preState"></param>
	void JoystickUpdate(XINPUT_STATE& state, XINPUT_STATE& preState);
private:
	ComPtr<IDirectInputDevice8> keyboard_;
	ComPtr<IDirectInput8> directInput_;
	BYTE key_[256] = {};
	BYTE keyPre_[256] = {};
	WinApp* winApp_ = nullptr; //協力関係

	static std::shared_ptr<Input> sInstance_;
	
	XINPUT_STATE prevState_ = {};
};