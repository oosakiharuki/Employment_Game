#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"

using namespace MyMath;

Player::Player(){}

Player::~Player() {
	delete object;
}

void Player::Initialize() {
	worldTransform.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("playerHead.obj");
}

void Player::Update() {
	if (Input::GetInstance()->PushKey(DIK_A)) {
		worldTransform.translation_.x -= 0.1f;
	}
	else if (Input::GetInstance()->PushKey(DIK_D)) {
		worldTransform.translation_.x += 0.1f;
	}

	if (isGround) {
		grabity = 0.0f;
		isJump = false;
	}
	else {
		grabity -= 0.01f;
	}	

	//ジャンプ
	if (Input::GetInstance()->TriggerKey(DIK_W) && !isJump) {
		isJump = true;
	}

	if (isJump) {
		worldTransform.translation_.y += 0.2f;
	}


	worldTransform.translation_.y += grabity;

#ifdef  USE_IMGUI

	ImGui::Begin("player");

	//カメラ
	ImGui::InputFloat3("worldTransform.translate", &worldTransform.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &worldTransform.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &worldTransform.rotation_.x);
	ImGui::SliderFloat("RotateX", &worldTransform.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateY", &worldTransform.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateZ", &worldTransform.rotation_.z, -360.0f, 360.0f);

	ImGui::End();

#endif //  USE_IMGUI

	worldTransform.UpdateMatrix();
}

void Player::Draw() {
	object->Draw(worldTransform);
}

AABB Player::GetAABB() {
	AABB aabb;
	aabb.min = worldTransform.translation_ + playerAABB.min;
	aabb.max = worldTransform.translation_ + playerAABB.max;
	return aabb;
}

void Player::SetModelFile(std::string filename) {

}

void Player::OverReturn(const Vector3& over) {
	worldTransform.translation_.y -= 0.0f;
}