#include "Input.h"

#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Xinput.lib")

std::unique_ptr<Input> Input::sInstance_ = nullptr;

Input& Input::GetInstance() {
	if (sInstance_ == nullptr) {
		sInstance_ = std::make_unique<Input>();
	}
	return *sInstance_;
}

void Input::Finalize() {
	sInstance_.reset();
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

bool Input::GetJoystickState() {
	DWORD dwResult;

	preState_ = state_;

	ZeroMemory(&state_, sizeof(XINPUT_STATE));
	
	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState(0, &state_);

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

bool Input::GetJoystickStatePrevious() {
	DWORD dwResult;

	ZeroMemory(&state_, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState(0, &state_);

	//コントローラが作動してるか
	if (dwResult == ERROR_SUCCESS)
	{
		// Controller is connected	

		//現在の状態から前回の状態に
		state_ = preState_;

		return true;
	}
	else
	{
		// Controller is not connected		
		return false;
	}

	return false;
}

bool Input::PushButton(int button) {
	if (state_.Gamepad.wButtons & button) {
		return true;
	}
	return false;
}

bool Input::TriggerButton(int button) {
	if (state_.Gamepad.wButtons & button &&
		!(preState_.Gamepad.wButtons & button)) {
		return true;
	}	
	return false;
}

float Input::LeftStickX() {
	return static_cast<float>(state_.Gamepad.sThumbLX) /  kInclination;
}
float Input::LeftStickY() {
	return static_cast<float>(state_.Gamepad.sThumbLY) /  kInclination;
}

float Input::RightStickX() {
	return static_cast<float>(state_.Gamepad.sThumbRX) /  kInclination;
}
float Input::RightStickY() {
	return static_cast<float>(state_.Gamepad.sThumbRY) /  kInclination;
}

void Input::JoystickUpdate() {
	GetJoystickState();
	GetJoystickStatePrevious();
}