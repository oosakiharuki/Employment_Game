#include "CameraControl.h"
#include <sstream>
#include <random>
#include "ImGuiManager.h"
#include "Input.h"

using namespace MyMath;
using namespace UseEveryOne;

void CameraControl::Initialize() {
	wt_.Initialize();
	isFixedMode_ = false;
	//Transform更新処理
	transform_ = wt_.UpdateTransform();
}

void CameraControl::Update(Camera* camera) {

	//固定モードでないなら
	if (!isFixedMode_ && !isFreeMode_) {
		Move();
	}

	if (isFreeMode_) {
		DebugMove();
	}

	//ズーム
	if (isZoom_) {
		Zoom();
	}
	//シェイク
	if (isShakeMode_) {
		Shaking();
	}

	//imGui更新処理
	ImGuiUpdate();

	wt_.UpdateMatrix(transform_);

	camera->SetRotate(transform_.rotate);
	camera->SetTranslate(transform_.translate);
	camera->Update();
}

void CameraControl::ImGuiUpdate() {
#ifdef  USE_IMGUI
	//imguiでなくボタンで変更するように
	if (Input::GetInstance().TriggerKey(DIK_P)) {
		isFreeMode_ = !isFreeMode_;
	}

	ImGui::Begin("camera");
	//カメラ
	ImGui::InputFloat3("cameraTranslate", &transform_.translate.x);
	ImGui::SliderFloat3("cameraTranslateSlider", &transform_.translate.x, -30.0f, 30.0f);

	ImGui::InputFloat3("cameraRotate", &transform_.rotate.x);
	ImGui::SliderFloat("cameraRotateX", &transform_.rotate.x, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateY", &transform_.rotate.y, -360.0f, 360.0f);
	ImGui::SliderFloat("cameraRotateZ", &transform_.rotate.z, -360.0f, 360.0f);

	ImGui::Checkbox("free_Mode", &isFreeMode_);
	ImGui::InputFloat("cameraRotate", &movePower_);
	ImGui::End();
#endif //  USE_IMGUI
}

void CameraControl::SetEndPoint(const Vector3& left, const Vector3& right){
	leftEndPoint_ = left;//左端
	rightEndPoint_ = right;//右端
}

void CameraControl::Move() {
	
	//X座標
	//プレイヤーが左端を超えたら
	if (playerPos_.x <= leftEndPoint_.x) {
		transform_.translate.x = leftEndPoint_.x;
	}//プレイヤーが右端を超えたら
	else if (playerPos_.x >= rightEndPoint_.x) {
		transform_.translate.x = rightEndPoint_.x;
	}//両端の間
	else {
		transform_.translate.x = playerPos_.x;//プレイヤーX座標
	}
	 
	//Y座標
	if (isCameraYFixed_) {
		return;
	}

	if (playerPos_.y >= kFixedY_) {
		transform_.translate.y = playerPos_.y + kFixedY_;
	}
	else {
		transform_.translate.y = fixedPos_.y;
	}
}

void CameraControl::DebugMove() {
	//上に移動
	if (Input::GetInstance().PushKey(DIK_UP)) {
		transform_.translate.y += movePower_;
	}
	//下に移動
	if (Input::GetInstance().PushKey(DIK_DOWN)) {
		transform_.translate.y -= movePower_;
	}
	//左に移動
	if (Input::GetInstance().PushKey(DIK_LEFT)) {
		transform_.translate.x -= movePower_;
	}
	//右に移動
	if (Input::GetInstance().PushKey(DIK_RIGHT)) {
		transform_.translate.x += movePower_;
	}
	//後ろに移動
	if (Input::GetInstance().PushKey(DIK_Y)) {
		transform_.translate.z -= movePower_;
	}
	//前に移動
	if (Input::GetInstance().PushKey(DIK_U)) {
		transform_.translate.z += movePower_;
	}
}


void CameraControl::Zoom() {
	//ズームがMax値ではないとき
	if (zoomTimer_ < kMaxZoomTime_) {
		transform_.translate = cameraSegment_.diff + EaseOut(cameraSegment_.origin - cameraSegment_.diff, zoomTimer_, kMaxZoomTime_);
		zoomTimer_ += kDeltaTime_;
	}
	else {
		//Max値
		transform_.translate = cameraSegment_.diff;
		zoomTimer_ = kMaxZoomTime_;
	}
}

void CameraControl::Shaking() {
	//時間が0になった時
	if (shakeTimer_ <= 0.0f) {
		isShakeMode_ = false;
		//カメラ位置を戻す
		transform_.translate = preTranslate_;
	}
	else {
		//シェイクする前に元々のカメラ位置を設定
		if (shakeTimer_ == kShakeMaxTime_) {
			//シェイク前のカメラ位置
			preTranslate_ = transform_.translate;
		}


		shakeTimer_ -= kDeltaTime_;

		std::random_device seed;
		std::mt19937 random(seed());

		std::uniform_real_distribution<float> yure(-kShakePower, kShakePower);

		//上下左右にシェイク(z軸は関係ない)
		transform_.translate += Vector3{ yure(random), yure(random), 0.0f };
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
	cameraSegment_.origin = transform_.translate;
	cameraSegment_.diff = goal;
	isZoom_ = true;
}

void CameraControl::CameraSetting(const CameraInitData& data, bool fixed_Mode_) {
	//座標と回転
	transform_.rotate = data.transform.rotate;
	transform_.translate = data.transform.translate;

	//カメラの最小/最大地点
	leftEndPoint_ = transform_.translate + data.leftPoint;
	rightEndPoint_ = transform_.translate + data.rightPoint;

	isFixedMode_ = fixed_Mode_;

	fixedPos_ = transform_.translate;
}
