#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Primitive.h"

using namespace MyMath;
using namespace Primitive;

Player::Player(){}

Player::~Player() {
	delete object;

	for (auto* bullet : bullets_) {
		delete bullet;
	}
	delete umbrella;
	delete particle_walk;
}

void Player::Initialize() {
	worldTransform.Initialize();

	object = new Object3d();
	object->Initialize();
	object->SetModelFile("playerHead.obj");

	umbrella = new Umbrella();
	umbrella->Initialize();

	wtGun.Initialize();
	wtGun.rotation_.y = 90.0f;

	hitSound = Audio::GetInstance()->LoadWave("resource/Sound/damage.wav");


	particle_walk = new Particle();
	particle_walk->Initialize("resource/Sprite/gradationLine.png",PrimitiveType::ring);
	particle_walk->ChangeMode(BornParticle::Stop);
}

void Player::Update() {

	DeadPlayer();

	if (Hp == 0) {
		return;
	}

	bool pushA = false;
	bool pushD = false;
	bool pushW = false;
	bool pushS = false;

	if (Input::GetInstance()->PushKey(DIK_A)) {
		pushA = true;
	}
	if (Input::GetInstance()->PushKey(DIK_D)) {
		pushD = true;
	}
	if (Input::GetInstance()->PushKey(DIK_W)) {
		pushW = true;
	}	
	if (Input::GetInstance()->PushKey(DIK_S)) {
		pushS = true;
	}
	
	if (isShield) {
		speed = 0.05f;
	}
	else {
		speed = 0.1f;
	}

	if (pushA) {
		worldTransform.translation_.x -= speed;
		direction = left;
		range = Left;
		isChangeDirection = true;

		if (pushW) {
			range = UpLeft;
		}
		else if (pushS) {
			range = DownLeft;
		}

	}
	else if (pushD) {
		worldTransform.translation_.x += speed;
		direction = right;
		range = Right;
		isChangeDirection = true;

		if (pushW) {
			range = UpRight;
		}
		else if (pushS) {
			range = DownRight;
		}
	}
	else if (pushW) {
		range = Up;
	}
	else if (pushS) {
		range = Down;
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

	switch (range)
	{
	case Player::Up:
		wtGun.rotation_.x = -90.0f;
		break;
	case Player::UpRight:
		wtGun.rotation_.x = -45.0f;
		break;
	case Player::Right:
		wtGun.rotation_.x = 0.0f;
		break;
	case Player::DownRight:
		wtGun.rotation_.x = 45.0f;
		break;
	case Player::Down:
		wtGun.rotation_.x = 90.0f;
		break;
	case Player::DownLeft:
		wtGun.rotation_.x = 135.0f;
		break;
	case Player::Left:
		wtGun.rotation_.x = 180.0f;
		break;
	case Player::UpLeft:
		wtGun.rotation_.x = 225.0f;
		break;
	default:
		break;
	}

	//ジャンプ
	if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !isJump) {
		isJump = true;
		isGround = false;
	}

	grabity -= 0.01f;
	if (isGround) {
		isJump = false;
		//離した時
		if (!Input::GetInstance()->PushKey(DIK_L)) {
			//ブリンクのタイマーリセット
			brinkTimer = 0.0f;
		}
	}


	//傘シールド
	if (Input::GetInstance()->PushKey(DIK_L)) {

		if (isGround && (range == Down || range == DownLeft || range == DownRight)) {
			brinkTimer = brinkTimeMax;
		}

		if (brinkTimer < brinkTimeMax) {
			isGround = false;

			brinkTimer += deltaTime;
			worldTransform.translation_ += EaseIn(TransformNormal({ 0.0f,0.0f,1.5f },wtGun.matWorld_), brinkTimer, brinkTimeMax);
		}



		isShield = true;
	}
	else {
		isShield = false;
	}

	coolTimer += deltaTime;
	if (Input::GetInstance()->TriggerKey(DIK_K) && !isShield) {
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

	//重力
	if (range == Up && isShield) {
		if (isJump) {
			isJump = false;
		}
		grabity = 0.0f;
		worldTransform.translation_.y -= 0.05f;
	}
	else {
		worldTransform.translation_.y += grabity;
	}


	//ノックバック発動
	if (isKnockback) {

		//ゼロならイーズインされない
		if (KnockBackTimeMax == 0.0f) {
			worldTransform.translation_ -= backPower;
			isKnockback = false;
		}
		else {
			KnockBackTimer += deltaTime;

			worldTransform.translation_ -= EaseIn(backPower, KnockBackTimer, KnockBackTimeMax);
			if (KnockBackTimer >= KnockBackTimeMax) {
				isKnockback = false;
				KnockBackTimer = 0.0f;
			}
		}

	}

	if (infinityTimer >= infinityTimeMax) {
		infinityTimer = infinityTimeMax;
	}
	else {
		infinityTimer += deltaTime;
	}

	particle_walk->Update();


	//移動しているとパーティクルを発生
	if (pushA || pushW || pushS || pushD) {
		// 通常のパーティクル
		particle_walk->SetParticleCount(2);
		particle_walk->SetFrequency(0.25f);
		particle_walk->ChangeMode(BornParticle::TimerMode);
		particle_walk->SetTranslate(worldTransform.translation_);
	}
	else {
		particle_walk->ChangeMode(BornParticle::Stop);
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
	wtGun.translation_ = worldTransform.translation_;
	wtGun.UpdateMatrix();

	///傘の銃
	umbrella->SetTranslate(worldTransform.translation_ + 
		TransformNormal(Vector3(0, 1, 0), worldTransform.matWorld_) +
		TransformNormal(Vector3(0, 0, 1.25f), wtGun.matWorld_));

	umbrella->SetRotate(wtGun.rotation_);
	umbrella->ShieldMode(isShield);

	umbrella->Update();

}

void Player::Draw() {
	if (Hp == 0) {
		return;
	}

	object->Draw(worldTransform);
	for (auto* bullet : bullets_) {
		bullet->Draw();
	}

	umbrella->Draw();
}

void Player::DrawP() {
	particle_walk->Draw();
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

	Vector3 translate = umbrella->GetTranslate();

	float halfCount = (float(bulletCount) - 1) / 2;

	for (float i = -(halfCount); i <= halfCount; ++i) {
		Vector3 velocity = { 0.0f,float(i) * 0.1f,0.5f};
		velocity = TransformNormal(velocity, wtGun.matWorld_);

		PlayerBullet* bullet = new PlayerBullet();
		bullet->Initialize();
		bullet->SetTranslate(translate);
		bullet->SetVelocty(velocity);
		bullets_.push_back(bullet);
	}

	///ノックバック
	Vector3 playerknockback = { 0.0f,0.0f,0.25f };
	KnockBackPlayer(playerknockback,0.5f);
}

void Player::IsDamage() {
	if (infinityTimer >= infinityTimeMax) {
		if (Hp == 0) {
			return;
		}
		Hp--;
		Audio::GetInstance()->SoundPlayWave(hitSound, 0.4f);
		infinityTimer = 0.0f;
	}
}

void Player::IsFall() {
	if (Hp == 0) {
		return;
	}
	Hp--;
	Audio::GetInstance()->SoundPlayWave(hitSound, 0.4f);
}

void Player::KnockBackPlayer(const Vector3 Power, const float TimerMax) {
	backPower = TransformNormal(Power, wtGun.matWorld_);
	isKnockback = true;
	KnockBackTimeMax = TimerMax;
}


Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform.matWorld_.m[3][0];
	worldPos.y = worldTransform.matWorld_.m[3][1];
	worldPos.z = worldTransform.matWorld_.m[3][2];

	return worldPos;
}

void Player::DeadPlayer() {
	if (Hp == 0) {
		deadTimer += deltaTime;
		isPlayerDown = true;
		if (deadTimer >= deadTimeMax) {
			Hp = MaxHp;
			worldTransform.translation_ = respownPosition;
			isRespown = true;
			deadTimer = 0.0f;
		}
	}
}