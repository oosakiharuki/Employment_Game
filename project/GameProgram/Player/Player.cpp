#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Primitive.h"

#include "SpriteCommon.h"
#include "SkinningCommon.h"
#include <numbers>

using namespace MyMath;
using namespace Primitive;

Player::Player(){}

Player::~Player() {
	bullets_.clear();
	sprites_Hp.clear();
}

void Player::Initialize() {
	worldTransform.Initialize();

	object = std::make_unique<Object_glTF>();
	object->Initialize();
	object->SetModelFile("NewPlayer.gltf");
	object->SetEnvironment("resource/rostock_laage_airport_4k.dds");

	worldTransform.scale_ = { 1,1,1 };
	animation_mode = Animation_Mode::mode_stop;
	PreAnimation_mode = animation_mode;

	umbrella = std::make_unique<Umbrella>();
	umbrella->Initialize();

	wtGun.Initialize();
	wtGun.rotation_.y = 90.0f;

	hitSound = Audio::GetInstance()->LoadWave("resource/Sound/damage.wav");

	pariSound = Audio::GetInstance()->LoadWave("resource/Sound/bane.wav");

	particle_walk = std::make_unique<Particle>();
	particle_walk->Initialize("resource/Sprite/ground.png",PrimitiveType::box);
	particle_walk->ChangeMode(BornParticle::Stop);
	particle_walk->SetParticleMosion(ParticleMosion::Smaller);

	particle_brink = std::make_unique<Particle>();
	particle_brink->Initialize("resource/Sprite/cone.png", PrimitiveType::cone);
	particle_brink->SetParticleCount(1);
	particle_brink->ChangeMode(BornParticle::Stop);
	particle_brink->SetParticleMosion(ParticleMosion::Fixed);
	particle_brink->SetFrequency(1.0f);
	particle_brink->SetScale({2,2,2});


	particle_fire = std::make_unique<Particle>();
	particle_fire->Initialize("resource/Sprite/cone.png", PrimitiveType::cone);
	particle_fire->SetParticleCount(1);
	particle_fire->ChangeMode(BornParticle::Stop);
	particle_fire->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.1f);


	particle_damage = std::make_unique<Particle>();
	particle_damage->Initialize("resource/Sprite/circle.png", PrimitiveType::ring);
	particle_damage->SetParticleCount(20);
	particle_damage->ChangeMode(BornParticle::Stop);
	particle_damage->SetParticleMosion(ParticleMosion::Exprosion);
	particle_damage->SetFrequency(1.0f);


	particle_pari = std::make_unique<Particle>();
	particle_pari->Initialize("resource/Sprite/uvChecker.png", PrimitiveType::cone);
	particle_pari->ChangeMode(BornParticle::Stop);
	particle_pari->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.5f);

	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();
	shadow_->SetScale({ 1,0,1 });


	for (uint32_t i = 0; i < 3; i++) {
		std::unique_ptr <Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize("Hp.png");
		sprite->SetPosition({ 20.0f + 64.0f * i , 45.0f - i * 10.0f });
		sprite->SetSize({64,64});
		sprites_Hp.push_back(std::move(sprite));
	}

	input_ = Input::GetInstance();
}

void Player::Update() {

	DeadPlayer();

	input_->GetJoyStickState(0, state);
	input_->GetJoystickStatePrevious(0, preState);

	bool pushA = false;
	bool pushD = false;
	bool pushW = false;
	bool pushS = false;

	if (!isPlayerDown && !isAnimationOnlyUpdate) {

		if (input_->PushKey(DIK_A)) {
			pushA = true;
		}
		if (input_->PushKey(DIK_D)) {
			pushD = true;
		}
		if (input_->PushKey(DIK_W)) {
			pushW = true;
		}
		if (input_->PushKey(DIK_S)) {
			pushS = true;
		}


		if (input_->GetJoyStickState(0, state)) {
			float padX = static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f;
			float padY = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;

			if (padX > 0.5f) {
				pushD = true;
			}
			else if (padX < -0.5f) {
				pushA = true;
			}

			if (padY > 0.5f) {
				pushW = true;
			}
			else if (padY < -0.5f) {
				pushS = true;
			}
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
		if ((input_->TriggerKey(DIK_SPACE) || input_->TriggerBotton(state, preState, XINPUT_GAMEPAD_A))
			&& isGround && !isShield) {
			isJump = true;
			isGround = false;
		}

		grabity -= 0.01f;
		if (isGround) {
			isJump = false;
			isOneBrink = false;//ブリンク可能
		}


		//傘シールド
		if (input_->PushKey(DIK_L) || input_->PushBotton(state, XINPUT_GAMEPAD_B)) {
			//押した瞬間に移動キーを押している場合ブリンクが発動+一度ブリンクしていないとき
			if ((input_->TriggerKey(DIK_L) || input_->TriggerBotton(state, preState, XINPUT_GAMEPAD_B))
				&& (pushA || pushD || pushW || pushS) && !isOneBrink) {
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
		if ((input_->TriggerKey(DIK_K) || input_->TriggerBotton(state, preState, XINPUT_GAMEPAD_X)) && !isShield) {
			if (coolTimer >= coolMax) {
				ShootBullet();
				coolTimer = 0;
			}
		}

		if (isDamageMosion) {
			ScaleUpdate(&isDamageMosion, damageScale, damageMaxTime);
		}

		if (isShildMosion) {
			umbrella->ScaleUpdate(&isShildMosion, damageScale, damageMaxTime);
		}

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

		//影の更新
		shadow_->SetTranslate(worldTransform.translation_);
	}

	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	bullets_.remove_if([](auto& bullet) {
		if (bullet->IsDead()) {
			bullet.reset();
			return true;
		}
		return false;
	});

	if (infinityTimer >= infinityTimeMax) {
		infinityTimer = infinityTimeMax;
	}
	else {
		infinityTimer += deltaTime;
	}

	//移動しているとパーティクルを発生
	if (isGround && (worldTransform.translation_.x != PrePosition.x || worldTransform.translation_.y != PrePosition.y)) {
		// 通常のパーティクル
		particle_walk->SetParticleCount(10);
		particle_walk->SetFrequency(0.25f);
		particle_walk->ChangeMode(BornParticle::TimerMode);
		particle_walk->SetTranslate(worldTransform.translation_ + TransformNormal(Vector3{0.0f,-1.0f,-0.3f},worldTransform.matWorld_));

		particle_walk->SetScale({ 0.5f,0.5f,0.5f });
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
	if (isAnimationOnlyUpdate) {
		animation_mode = Animation_Mode::mode_NextStage;
		worldTransform.rotation_.y = 0.0f;
	}
	else if (isShield) {
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
			object->ChangeAnimation("NewPlayer.gltf");
			break;
		case Player::Animation_Mode::mode_move:
			object->ChangeAnimation("NewPlayer.gltf");
			break;
		case Player::Animation_Mode::mode_sield:
			object->ChangeAnimation("NewPlayer_umbrella.gltf");
			break;
		case Player::Animation_Mode::mode_damage:
			//object->ChangeAnimation("stop.gltf");
			break;
		case Player::Animation_Mode::mode_NextStage:
			object->ChangeAnimation("NewPlayer.gltf");
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

	for (auto& sprite : sprites_Hp) {
		sprite->Update();
	}		
}

void Player::Draw() {
	if (Hp != 0) {
		GLTFCommon::GetInstance()->Command();

		object->Draw();

		umbrella->Draw();

		shadow_->Draw();
	}

	
	Object3dCommon::GetInstance()->Command();

	for (auto& bullet : bullets_) {
		bullet->Draw();
	}

}

void Player::DrawP() {
	particle_walk->Draw();
	particle_fire->Draw();
	particle_brink->Draw();
	particle_damage->Draw();
	particle_pari->Draw();

	SpriteCommon::GetInstance()->Command();
	
	for (auto& sprite : sprites_Hp) {
		sprite->Draw();
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

	Vector3 translate = umbrella->GetTranslate();

	float halfCount = (float(bulletCount) - 1) / 2;

	for (float i = -(halfCount); i <= halfCount; ++i) {
		Vector3 velocity = { 0.0f,float(i) * 0.1f,0.5f};
		velocity = TransformNormal(velocity, wtGun.matWorld_);

		std::unique_ptr<PlayerBullet> bullet = std::make_unique<PlayerBullet>();
		bullet->Initialize();
		bullet->SetTranslate(translate);
		bullet->SetVelocty(velocity);
		bullets_.push_back(std::move(bullet));
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
		backPower = TransformNormal({ 0,0,0.5f } ,worldTransform.matWorld_);
		isKnockback = true;
		KnockBackTimeMax = infinityTimeMax / 3;
	}

	isDamageMosion = true;
	
	//連続ヒット時、元に戻す
	scaleTimer = 0.0f;
	worldTransform.scale_ = { 1,1,1 };

	SpriteUpdate();

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

	//連続ヒット時、元に戻す
	umbrella->SetScale({ 1,1,1 });

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

			for (auto& sprite : sprites_Hp) {
				sprite->SetTextureFile("Hp.png");
			}

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

void Player::ShadowUpdate() {
	shadow_->Update();
}

void Player::SpriteUpdate() {
	if (Hp < 3) {
		sprites_Hp[2]->SetTextureFile("NoHp.png");
		if (Hp < 2) {
			sprites_Hp[1]->SetTextureFile("NoHp.png");
			if (Hp < 1) {
				sprites_Hp[0]->SetTextureFile("NoHp.png");
			}
		}
	}
}

void Player::ScaleUpdate(bool* mosionOn, Vector3 scale, const float maxTime) {
	if (scaleTimer >= maxTime / 2.0f) {
		worldTransform.scale_ -= scale;
		if (scaleTimer >= maxTime) {
			scaleTimer = 0.0f;
			worldTransform.scale_ = { 1,1,1 };

			//モーションを終了する
			*mosionOn = false;
		}
	}
	else {
		worldTransform.scale_ += scale;	
	}
	scaleTimer += deltaTime;
}