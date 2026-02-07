#include "Input.h"

#include <cassert>

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"Xinput.lib")

#include "ImGuiManager.h"
#include <algorithm>

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


	result = directInput_->CreateDevice(GUID_SysMouse, &mouseInput_, NULL);
	assert(SUCCEEDED(result));

	result = mouseInput_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(result));


	//ウィンドウズのクライアント領域の真ん中座標をとる処理
	p.x = centerX;
	p.y = centerY;

	ClientToScreen(winApp_->GetHwnd(), &p);
	SetCursorPos(p.x, p.y);

}

void Input::Update() {
	HRESULT result;

	memcpy(keyPre_, key_, sizeof(key_));

	result = keyboard_->Acquire();

	result = keyboard_->GetDeviceState(sizeof(key_), key_);

	result = mouseInput_->Acquire();

	result = mouseInput_->GetDeviceState(sizeof(mouseState_), &mouseState_);

	mouseX_ += float(mouseState_.lX);
	mouseY_ += float(mouseState_.lY);


	if (mouseState_.rgbButtons[0] & 0x80) {
		//SetCursorPos(centerX, centerY);
	}

	mouseX_ = std::clamp(mouseX_, float(p.x - centerX) , float(p.x + centerX));
	mouseY_ = std::clamp(mouseY_, float(p.y - centerY), float(p.y + centerY));

#ifdef USE_IMGUI
	
	ImGui::Begin("input");
	ImGui::Text("%f,%f",mouseX_, mouseY_);
	ImGui::End();

#endif // USE_IMGUI

	//現在の状態から前回の状態に
	preState_ = state_;
	//ゲームパットボタン更新処理	
	GetJoystickState();
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

	ZeroMemory(&state_, sizeof(XINPUT_STATE));

	// Simply get the state of the controller from XInput.
	dwResult = XInputGetState(0, &state_);

	//コントローラが作動してるか
	if (dwResult == ERROR_SUCCESS)
	{
		// Controller is connected
		isActiveGamePad_ = true;

		return true;
	}
	else
	{
		// Controller is not connected
		isActiveGamePad_ = false;

		return false;
	}

	return false;
}

bool Input::GetActiveGamePad() {
	return isActiveGamePad_;
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

bool Input::LeftTriggerLongPress() {
	if (state_.Gamepad.bLeftTrigger) {
		return true;
	}
	return false;
}

bool Input::RightTriggerLongPress() {
	if (state_.Gamepad.bRightTrigger) {
		return true;
	}
	return false;
}

bool Input::LeftTrigger() {
	if (state_.Gamepad.bLeftTrigger && !preState_.Gamepad.bLeftTrigger) {
		return true;
	}
	return false;
}

bool Input::RightTrigger() {
	if (state_.Gamepad.bRightTrigger && !preState_.Gamepad.bRightTrigger) {
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
