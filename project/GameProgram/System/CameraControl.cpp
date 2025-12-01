#include "CameraControl.h"
#include <sstream>
#include <random>
#include "ImGuiManager.h"

using namespace MyMath;
using namespace UseEveryOne;

void CameraControl::Initialize() {
	wt_.Initialize();
	isFixedMode_ = false;
}

void CameraControl::Update(Camera* camera) {

	//固定モードでないなら
	if (!isFixedMode_ && !isFreeMode_) {
		Move();
	}

	//ズーム
	Zoom();
	//シェイク
	Shaking();

#ifdef  USE_IMGUI

	ImGui::Begin("camera");
	ImGui::Text("ImGuiText");

	//カメラ
	ImGui::InputFloat3("cameraTranslate", &wt_.translation_.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &wt_.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &wt_.rotation_.x);
	ImGui::SliderFloat("cameraRotateX", &wt_.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &wt_.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &wt_.rotation_.z, -360.0f, 360.0f);

	ImGui::Checkbox("free_Mode",&isFreeMode_);

	ImGui::End();

#endif //  USE_IMGUI

	wt_.UpdateMatrix();

	camera->SetRotate(wt_.rotation_);
	camera->SetTranslate(wt_.translation_);
	camera->Update();

}

void CameraControl::SetEndPoint(const Vector3& left, const Vector3& right){
	leftEndPoint_ = left;//左端
	rightEndPoint_ = right;//右端
}

void CameraControl::Move() {
	
	//X座標
	//プレイヤーが左端を超えたら
	if (playerPos_.x <= leftEndPoint_.x) {
		wt_.translation_.x = leftEndPoint_.x;
	}//プレイヤーが右端を超えたら
	else if (playerPos_.x >= rightEndPoint_.x) {
		wt_.translation_.x = rightEndPoint_.x;
	}//両端の間
	else {
		wt_.translation_.x = playerPos_.x;//プレイヤーX座標
	}
	 
	//Y座標
	if (playerPos_.y >= kFixedY_) {
		wt_.translation_.y = playerPos_.y + kFixedY_;
	}
	else {
		wt_.translation_.y = fixedPos_.y;
	}

}

void CameraControl::Zoom() {
	if (isZoom_) {
		if (zoomTimer_ < kMaxZoomTime_) {
			wt_.translation_ = cameraSegment_.diff + EaseOut(cameraSegment_.origin - cameraSegment_.diff, zoomTimer_, kMaxZoomTime_);
			zoomTimer_ += kDeltaTime_;
		}
		else {
			wt_.translation_ = cameraSegment_.diff;
			zoomTimer_ = kMaxZoomTime_;
		}
	}
}

void CameraControl::Shaking() {

	if (shakeTimer_ <= 0.0f) {
		isShakeMode_ = false;
		//カメラ位置を戻す
		wt_.translation_ = preTranslate_;
		return;
	}
	else if (isShakeMode_) {
		//シェイクする前に元々のカメラ位置を設定
		if (shakeTimer_ == kShakeMaxTime_) {
			//シェイク前のカメラ位置
			preTranslate_ = wt_.translation_;
		}


		shakeTimer_ -= kDeltaTime_;

		std::random_device seed;
		std::mt19937 random(seed());

		std::uniform_real_distribution<float> yure(-kShakePower, kShakePower);

		//上下左右にシェイク(z軸は関係ない)
		wt_.translation_ += Vector3{ yure(random), yure(random), 0.0f };
	}
}

const bool CameraControl::ZoomEnd() {
	//zoomTimer_がkMaxZoomTime_と同じ値ならTrue
	if (zoomTimer_ >= kMaxZoomTime_) {
		return true;
	}
	return false;
}

void CameraControl::ZoomStart(const Vector3& goal) {
	cameraSegment_.origin = wt_.translation_;
	cameraSegment_.diff = goal;
	isZoom_ = true;
}

void CameraControl::CameraSetting(const CameraInitData& data, const bool& fixed_Mode_) {
	//座標と回転
	wt_.rotation_ = data.rotation;
	wt_.translation_ = data.translation;

	//カメラの最小/最大地点
	leftEndPoint_ = wt_.translation_ + data.Point1;
	rightEndPoint_ = wt_.translation_ + data.Point2;

	isFixedMode_ = fixed_Mode_;

	fixedPos_ = wt_.translation_;
}
