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
	/// <returns></returns>
	bool PushKey(BYTE keyNumber);
	/// <summary>
	/// キーボードインプット(単押し)
	/// </summary>
	/// <param name="keyNumber">キーを選択</param>
	/// <returns></returns>
	bool TriggerKey(BYTE keyNumber);
	/// <summary>
	/// ゲームパットボタン操作(長押し)
	/// </summary>
	/// <param name="state"></param>
	/// <param name="button">ボタンを選択</param>
	/// <returns></returns>
	bool PushButton(XINPUT_STATE& state,int button);
	/// <summary>
	/// ゲームパットボタン(単押し)
	/// </summary>
	/// <param name="state"></param>
	/// <param name="preState"></param>
	/// <param name="button">ボタンを選択</param>
	/// <returns></returns>
	bool TriggerButton(XINPUT_STATE& state, XINPUT_STATE& preState, int button);

	/// <summary>
	/// ゲームパット用の更新処理
	/// </summary>
	/// <param name="state"></param>
	/// <param name="preState"></param>
	void JoystickUpdate(XINPUT_STATE& state, XINPUT_STATE& preState);	
	/// <summary>
	/// ゲームパット
	/// </summary>
	/// <param name="num">番号</param>
	/// <param name="state">ステート</param>
	/// <returns></returns>
	bool GetJoystickState(uint32_t num, XINPUT_STATE& state);
private:

	/// <summary>
	/// ゲームパット
	/// </summary>
	/// <param name="num"></param>
	/// <param name="state">これInputクラスだけで完結できるかも</param>
	/// <returns></returns>
	bool GetJoystickStatePrevious(uint32_t num, XINPUT_STATE& state);

	ComPtr<IDirectInputDevice8> keyboard_;
	ComPtr<IDirectInput8> directInput_;
	BYTE key_[256] = {};
	BYTE keyPre_[256] = {};
	WinApp* winApp_ = nullptr; //協力関係
	//インスタンス
	static std::unique_ptr<Input> sInstance_;
	//default_deleteを設定(解放処理を行える)
	friend struct std::default_delete<Input>;

	XINPUT_STATE prevState_ = {};
};