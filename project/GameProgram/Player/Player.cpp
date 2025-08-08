#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"

using namespace MyMath;

Player::Player(){}

Player::~Player() {
	delete object;

	for (auto* bullet : bullets_) {
		delete bullet;
	}
	delete umbrella;
}

void Player::Initialize() {
	worldTransform.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("playerHead.obj");

	umbrella = new Umbrella();
	umbrella->Initialize();

}

void Player::Update() {



	if (Input::GetInstance()->PushKey(DIK_A)) {
		worldTransform.translation_.x -= speed;
		direction = left;
		isChangeDirection = true;
	}
	else if (Input::GetInstance()->PushKey(DIK_D)) {
		worldTransform.translation_.x += speed;
		direction = right;
		isChangeDirection = true;
	}

	if (isChangeDirection) {
		switch (direction)
		{
		case Player::right:
			worldTransform.rotation_.y = 90.0f;
			break;
		case Player::left:
			worldTransform.rotation_.y = -90.0f;
			break;
		default:
			break;
		}
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

	//傘シールド
	if (Input::GetInstance()->PushKey(DIK_S)) {
		isShield = true;
		umbrella->SetTranslate(worldTransform.translation_ + TransformNormal(Vector3(0, 0, 2),worldTransform.matWorld_));
		umbrella->SetRotate(worldTransform.rotation_);
	}
	else {
		isShield = false;
	}

	coolTimer += 1.0f / 60.0f;
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !isShield) {
		if (coolTimer >= coolMax) {
			ShootBullet();
			coolTimer = 0;
		}
	}

	for (auto* bullet : bullets_) {
		bullet->Update();
	}

	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});


	if (isJump) {
		worldTransform.translation_.y += 0.2f;
	}


	worldTransform.translation_.y += grabity;

	if (isNockback) {

		//やり方が微妙
		switch (direction)
		{
		case Player::right:
			worldTransform.translation_.x -= backDistance;
			break;
		case Player::left:
			worldTransform.translation_.x += backDistance;
			break;
		}
		backDistance = 0;
		isNockback = false;
	}

#ifdef  USE_IMGUI

	ImGui::Begin("player");

	ImGui::InputFloat3("worldTransform.translate", &worldTransform.translation_.x);
	ImGui::SliderFloat3("worldTransform.translateSlider", &worldTransform.translation_.x, -30.0f, 30.0f);

	ImGui::InputFloat3("Rotate", &worldTransform.rotation_.x);
	ImGui::SliderFloat("RotateX", &worldTransform.rotation_.x, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateY", &worldTransform.rotation_.y, -360.0f, 360.0f);
	ImGui::SliderFloat("RotateZ", &worldTransform.rotation_.z, -360.0f, 360.0f);

	ImGui::Text("体力:%d", Hp);

	ImGui::End();

#endif //  USE_IMGUI

	worldTransform.UpdateMatrix();
	umbrella->Update();

}

void Player::Draw() {
	object->Draw(worldTransform);
	for (auto* bullet : bullets_) {
		bullet->Draw();
	}

	if (isShield) {
		umbrella->Draw();
	}

}

AABB Player::GetAABB() {
	AABB aabb;
	aabb.min = worldTransform.translation_ + playerAABB.min;
	aabb.max = worldTransform.translation_ + playerAABB.max;
	return aabb;
}

void Player::SetModelFile(std::string filename) {

}

void Player::ShootBullet() {	

	Vector3 translate = {
		worldTransform.translation_.x,
		worldTransform.translation_.y + 1.0f,
		worldTransform.translation_.z
	};

	float halfCount = (float(bulletCount) - 1) / 2;

	for (float i = -(halfCount); i <= halfCount; ++i) {
		Vector3 velocity = { 0.0f,float(i) * 0.1f,0.5f};
		velocity = TransformNormal(velocity, worldTransform.matWorld_);

		PlayerBullet* bullet = new PlayerBullet();
		bullet->Initialize();
		bullet->SetTranslate(translate);
		bullet->SetVelocty(velocity);
		bullets_.push_back(bullet);
	}
}

void Player::IsDamage() {
	if (Hp == 0) {
		return;
	}
	Hp--;
}

void Player::UmbrellaHit(const float back) {
	backDistance = back;
	isNockback = true;
}
