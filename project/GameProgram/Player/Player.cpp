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
	delete particle_fire;
	delete particle_brink;
	delete particle_damage;
	delete particle_pari;

	delete shadow_;
}

void Player::Initialize() {
	worldTransform.Initialize();

	object = new Object_glTF();
	object->Initialize();
	object->SetModelFile("stop.gltf");
	object->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	worldTransform.scale_ = { 2,2,2 };
	animation_mode = Animation_Mode::mode_stop;
	PreAnimation_mode = animation_mode;

	umbrella = new Umbrella();
	umbrella->Initialize();

	wtGun.Initialize();
	wtGun.rotation_.y = 90.0f;

	hitSound = Audio::GetInstance()->LoadWave("resource/Sound/damage.wav");

	pariSound = Audio::GetInstance()->LoadWave("resource/Sound/bane.wav");

	particle_walk = new Particle();
	particle_walk->Initialize("resource/Sprite/gradationLine.png",PrimitiveType::ring);
	particle_walk->ChangeMode(BornParticle::Stop);
	particle_walk->SetParticleMosion(ParticleMosion::Smaller);

	particle_brink = new Particle();
	particle_brink->Initialize("resource/Sprite/cone.png", PrimitiveType::cone);
	particle_brink->SetParticleCount(1);
	particle_brink->ChangeMode(BornParticle::Stop);
	particle_brink->SetParticleMosion(ParticleMosion::Fixed);
	particle_brink->SetFrequency(1.0f);
	particle_brink->SetScale({2,2,2});


	particle_fire = new Particle();
	particle_fire->Initialize("resource/Sprite/cone.png", PrimitiveType::cone);
	particle_fire->SetParticleCount(1);
	particle_fire->ChangeMode(BornParticle::Stop);
	particle_fire->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.1f);


	particle_damage = new Particle();
	particle_damage->Initialize("resource/Sprite/circle.png", PrimitiveType::ring);
	particle_damage->SetParticleCount(20);
	particle_damage->ChangeMode(BornParticle::Stop);
	particle_damage->SetParticleMosion(ParticleMosion::Exprosion);
	particle_damage->SetFrequency(1.0f);


	particle_pari = new Particle();
	particle_pari->Initialize("resource/Sprite/uvChecker.png", PrimitiveType::cone);
	particle_pari->ChangeMode(BornParticle::Stop);
	particle_pari->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.5f);

	shadow_ = new Shadow();
	shadow_->Initialize();
	shadow_->SetScale({ 1,0,1 });
}

void Player::Update() {

	DeadPlayer();

	if (!isPlayerDown) {

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
			speed = standard_speed / 2.0f;
		}
		else {
			speed = standard_speed;
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

		if (isShield && !isGround && brinkTimer == 0.0f) {
			range = Up;
			if (pushA) {
				worldTransform.translation_.x -= speed;
			}
			else if (pushD) {
				worldTransform.translation_.x += speed;
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
		if (Input::GetInstance()->TriggerKey(DIK_SPACE) && isGround && !isShield) {
			isJump = true;
			isGround = false;
		}

		grabity -= 0.01f;
		if (isGround) {
			isJump = false;
			isOneBrink = false;//ブリンク可能

			shadowPosition = worldTransform.translation_;
			shadowPosition.y -= 1.0f;

		}


		//傘シールド
		if (Input::GetInstance()->PushKey(DIK_L)) {
			//押した瞬間に移動キーを押している場合ブリンクが発動+一度ブリンクしていないとき
			if (Input::GetInstance()->TriggerKey(DIK_L) && (pushA || pushD || pushW || pushS) && !isOneBrink) {
				isBrink = true;
			}
			isShield = true;

			pariTime -= deltaTime;
			//パリィ時間がすぎるとき+ダメージを食らうとパリィ失敗
			if (pariTime > 0.0f && infinityTimer >= infinityTimeMax) {
				isPari = true;
			}
			else {
				isPari = false;
			}

			pariCoolTime = 0.0f;
		}
		else {
			isShield = false;
			pariCoolTime += deltaTime;
		}			
		//連打してもすぐにパリィできないようにする
		if (pariCoolTime >= pariTimeMax) {
			pariTime = pariTimeMax;
		}

		if (isBrink) {
			//ブリンクの時は傘は開いたまま
			isShield = true;
			brinkTimer += deltaTime;

			//地面についている場合、下向きのブリンクは発動しない
			if (isGround && (range == Down || range == DownLeft || range == DownRight)) {
				brinkTimer = brinkTimeMax;
			}

			isOneBrink = true;
			worldTransform.translation_ += EaseIn(TransformNormal({ 0.0f,0.0f,1.5f }, wtGun.matWorld_), brinkTimer, brinkTimeMax);


			//飛んだ瞬間パーティクルをだす
			if (brinkTimer <= deltaTime) {
				Vector3 translate = worldTransform.translation_ + TransformNormal({ 0.0f,0.0f,-1.5f }, wtGun.matWorld_);
				particle_brink->SetTranslate(translate);
				particle_brink->SetRotate({ wtGun.rotation_.x + 90.0f,wtGun.rotation_.y,wtGun.rotation_.z });
				particle_brink->ChangeMode(BornParticle::MomentMode);
			}

			if (brinkTimer >= brinkTimeMax) {
				isBrink = false;
				brinkTimer = 0.0f;
			}
		}

		coolTimer += deltaTime;
		if (Input::GetInstance()->TriggerKey(DIK_K) && !isShield) {
			if (coolTimer >= coolMax) {
				ShootBullet();
				coolTimer = 0;
			}
		}

		shadow_->SetTranslate(shadowPosition);
		shadow_->Update();

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
		worldTransform.translation_.y += 0.25f;
	}

	//重力
	if (isShield && !isGround && range == Up) {
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

	//移動しているとパーティクルを発生
	if (worldTransform.translation_.x != PrePosition.x || worldTransform.translation_.y != PrePosition.y) {
		// 通常のパーティクル
		particle_walk->SetParticleCount(2);
		particle_walk->SetFrequency(0.25f);
		particle_walk->ChangeMode(BornParticle::TimerMode);
		particle_walk->SetTranslate(worldTransform.translation_);
	}
	else {
		particle_walk->ChangeMode(BornParticle::Stop);
	}

	//パーティクル
	particle_walk->Update();
	particle_fire->Update();
	particle_brink->Update();
	particle_damage->Update();
	particle_pari->Update();

	///アニメーション

	if (isShield) {
		animation_mode = Animation_Mode::mode_sield;
	}
	else if (worldTransform.translation_.x != PrePosition.x || worldTransform.translation_.y != PrePosition.y) {
		animation_mode = Animation_Mode::mode_move;
	}
	else {
		animation_mode = Animation_Mode::mode_stop;
	}

	if (animation_mode != PreAnimation_mode) {
		isChangeAnimation = true;
	}

	if (isChangeAnimation) {
		switch (animation_mode)
		{
		case Player::Animation_Mode::mode_stop:
			object->ChangeAnimation("stop.gltf");
			break;
		case Player::Animation_Mode::mode_move:
			object->ChangeAnimation("walk.gltf");
			break;
		case Player::Animation_Mode::mode_sield:
			object->ChangeAnimation("sneakWalk.gltf");
			break;
		case Player::Animation_Mode::mode_damage:
			object->ChangeAnimation("stop.gltf");
			break;
		default:
			break;
		}
		object->SetEnvironment("resource/rostock_laage_airport_4k.dds");

		PreAnimation_mode = animation_mode;
		isChangeAnimation = false;
	}
	
	PrePosition = worldTransform.translation_;

	object->Update(worldTransform);

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
		TransformNormal(Vector3(0, 0, 1.25f), wtGun.matWorld_));

	umbrella->SetRotate(wtGun.rotation_);
	umbrella->ShieldMode(isShield);

	umbrella->Update();

}

void Player::Draw() {
	if (Hp == 0) {
		return;
	}

	GLTFCommon::GetInstance()->Command();

	object->Draw();

	shadow_->Draw();

	Object3dCommon::GetInstance()->Command();

	
	for (auto* bullet : bullets_) {
		bullet->Draw();
	}

	umbrella->Draw();
}

void Player::DrawP() {
	particle_walk->Draw();
	particle_fire->Draw();
	particle_brink->Draw();
	particle_damage->Draw();
	particle_pari->Draw();

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

	particle_fire->SetTranslate(translate);
	particle_fire->SetRotate({ wtGun.rotation_.x + 90.0f,wtGun.rotation_.y,wtGun.rotation_.z });
	particle_fire->ChangeMode(BornParticle::MomentMode);

	///ノックバック
	Vector3 playerknockback = { 0.0f,0.0f,0.25f };
	KnockBackUmbrella(playerknockback,0.5f);
}

void Player::IsDamage() {
	if (infinityTimer >= infinityTimeMax) {
		if (Hp == 0) {
			return;
		}
		Hp--;
		particle_damage->SetTranslate(worldTransform.translation_);
		particle_damage->ChangeMode(BornParticle::MomentMode);
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
	backPower = TransformNormal(Power, worldTransform.matWorld_);
	backPower.z = 0.0f;
	isKnockback = true;
	KnockBackTimeMax = TimerMax;
}

void Player::KnockBackUmbrella(const Vector3 Power, const float TimerMax) {
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

//パリィ成功 = 連続弾も跳ね返す
void Player::PariSuccess() {
	//パリィ時間延長
	pariTime = pariTimeMax;

	Audio::GetInstance()->StopWave(pariSound);
	Audio::GetInstance()->SoundPlayWave(pariSound, 0.3f);

	Vector3 translate = umbrella->GetTranslate();
	translate.x += TransformNormal({0,0,2}, wtGun.matWorld_).x;

	particle_pari->SetTranslate(translate);
	particle_pari->SetRotate({ wtGun.rotation_.x + 90.0f,wtGun.rotation_.y,wtGun.rotation_.z });
	particle_pari->SetScale({2.0f,0.2f,2.0f});

	particle_pari->ChangeMode(BornParticle::MomentMode);
}