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
	if (!isFixedMode_ && !isFreeMode_ && !isZoom_) {
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

	//デバッグ以外では止めない
	if (!isFreeMode_) {
		//カメラセグメントの更新
		//目標位置(diff)と現在位置(origin)の距離を比較し離れている距離分足していく
		transform_.translate += GoDestination(cameraSegment_) * moveFrame_;
		cameraSegment_.origin = transform_.translate;//originを常に更新し徐々に距離が狭くなる
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
	//imGuiでなくボタンで変更するように
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
	minEndPoint_ = left;//左端
	maxEndPoint_ = right;//右端
}

void CameraControl::Move() {

	if (playerPos_.x > prevPlayerPos_.x) {
		cameraSegment_.diff.x = playerPos_.x + kInterpolationPointX_;
	}
	else if (playerPos_.x < prevPlayerPos_.x) {
		cameraSegment_.diff.x = playerPos_.x - kInterpolationPointX_;
	}
	else {
		cameraSegment_.diff.x = playerPos_.x;
	}

	prevPlayerPos_ = playerPos_;//前回の座標を測る

	//X座標範囲
	cameraSegment_.diff.x = std::clamp(cameraSegment_.diff.x, minEndPoint_.x, maxEndPoint_.x);

	if (playerPos_.y >= kFixedY_ && !isCameraYFixed_) {
		cameraSegment_.diff.y = playerPos_.y + kFixedY_;
	}
	else {
		cameraSegment_.diff.y = playerPos_.y;
	}
	
	//Y座標範囲
	cameraSegment_.diff.y = std::clamp(cameraSegment_.diff.y, minEndPoint_.y, maxEndPoint_.y);

	//補間の変更
	ChangeInterpolation();
}

void CameraControl::ChangeInterpolation() {
	//補間変更
	if (Length(cameraSegment_.origin.x, cameraSegment_.diff.x) >= kLittleFront_ * kTwice_) {
		moveFrame_ = kMoveFrameWeek_;//補間フレーム「弱」に変更
	}
	else if (Length(cameraSegment_.origin.x, cameraSegment_.diff.x) >= kLittleFront_) {
		moveFrame_ = kMoveFrameMedium_;//補間フレーム「中」に変更
	}
	else {
		moveFrame_ = kMoveFrameStrong_;//補間フレーム「強」に変更
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
		zoomTimer_ += kDeltaTime_;
	}
	else {
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
	SetSegment(data.transform.translate, data.transform.translate);
	//カメラの最小/最大地点
	SetEndPoint(data.transform.translate + data.leftPoint, data.transform.translate + data.rightPoint);
	isFixedMode_ = fixed_Mode_;
}

void CameraControl::CameraInterpolation(const CameraInitData& data, bool fixed_Mode_) {
	//座標
	SetSegment(transform_.translate, data.transform.translate);
	//カメラの最小/最大地点
	SetEndPoint(data.transform.translate + data.leftPoint, data.transform.translate + data.rightPoint);
	isFixedMode_ = fixed_Mode_;
}

void CameraControl::CameraStartPointPlayer(const CameraInitData& data, const Vector3& playerPosition) {
	//座標
	transform_.translate = playerPosition;
	SetSegment(playerPosition, data.transform.translate);
	//カメラの最小/最大地点
	SetEndPoint(data.transform.translate + data.leftPoint, data.transform.translate + data.rightPoint);
}

void CameraControl::SetSegment(const Vector3& origin, const Vector3& diff) {
	cameraSegment_.origin = origin;
	cameraSegment_.diff = diff;
}
