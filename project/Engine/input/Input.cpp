#include "Input.h"

#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Xinput.lib")

std::shared_ptr<Input> Input::sInstance_ = nullptr;

std::shared_ptr<Input> Input::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<Input>();
	}
	return sInstance_;
}

void Input::Finalize() {
	sInstance_.reset();
	sInstance_ = nullptr;
}

void Input::Initialize(WinApp* winApp) {

	//メンバ変数のwinApp <-代入- ローカル変数のwinAppの値
	this->winApp_ = winApp;

	HRESULT result;

	result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	result = keyboard_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

}

void Input::Update() {
	HRESULT result;

	memcpy(keyPre_, key_, sizeof(key_));

	result = keyboard_->Acquire();

	result = keyboard_->GetDeviceState(sizeof(key_), key_);
}

bool Input::PushKey(BYTE keyNumber) {
	
	if (key_[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::TriggerKey(BYTE keyNumber) {
	
	if (key_[keyNumber] && !keyPre_[keyNumber]) {
		return true;
	}

	return false;
}

bool Input::GetJoystickState(uint32_t num, XINPUT_STATE& state) {
	DWORD dwResult;

	prevState_ = state;

	ZeroMemory(&state, sizeof(XINPUT_STATE));
	
	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState(num, &state);

	//コントローラが作動してるか
	if (dwResult == ERROR_SUCCESS)
	{
		// Controller is connected
		return true;
	}
	else
	{
		// Controller is not connected
		return false;
	}

	return false;
}

bool Input::GetJoystickStatePrevious(uint32_t num, XINPUT_STATE& state) {
	DWORD dwResult;

	ZeroMemory(&state, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState(num, &state);

	//コントローラが作動してるか
	if (dwResult == ERROR_SUCCESS)
	{
		// Controller is connected	

		//現在の状態から前回の状態に
		state = prevState_;

		return true;
	}
	else
	{
		// Controller is not connected		
		return false;
	}

	return false;
}

bool Input::PushButton(XINPUT_STATE& state, int button) {
	if (state.Gamepad.wButtons & button) {
		return true;
	}
	return false;
}

bool Input::TriggerButton(XINPUT_STATE& state, XINPUT_STATE& preState, int button) {
	if (state.Gamepad.wButtons & button &&
		!(preState.Gamepad.wButtons & button)) {
		return true;
	}	
	return false;
}

void Input::JoystickUpdate(XINPUT_STATE& state, XINPUT_STATE& preState) {
	GetJoystickState(0, state);
	GetJoystickStatePrevious(0,preState);
}