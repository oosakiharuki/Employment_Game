#include "Player.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "Primitive.h"

#include "SpriteCommon.h"
#include "SkinningCommon.h"
#include <numbers>

using namespace MyMath;

Player::Player(){}

Player::~Player() {}

void Player::Initialize() {
	worldTransform.Initialize();

	//プレイヤー初期化/オブジェクト読み込み
	object = std::make_unique<Object_glTF>();
	object->Initialize();
	object->SetModelFile("NewPlayer.gltf");

	//アニメーションモード設定
	animation_mode = Animation_Mode::mode_stop;
	PreAnimation_mode = animation_mode;

	//傘の初期化
	umbrella = std::make_unique<Umbrella>();
	umbrella->Initialize();

	//傘の行列
	wtGun.Initialize();
	wtGun.rotation_.y = 90.0f;

	hitSound = Audio::GetInstance()->LoadWave("resource/Sound/damage.wav");

	pariSound = Audio::GetInstance()->LoadWave("resource/Sound/bane.wav");

	particle_walk = std::make_unique<Particle>();
	particle_walk->Initialize("player_walk", "resource/Sprite/ground.png", PrimitiveType::box);
	particle_walk->SetParticleMosion(ParticleMosion::Smaller);

	particle_brink = std::make_unique<Particle>();
	particle_brink->Initialize("player_brink", "resource/Sprite/cone.png", PrimitiveType::cone);
	particle_brink->SetParticleCount(1);
	particle_brink->SetParticleMosion(ParticleMosion::Fixed);
	particle_brink->SetFrequency(1.0f);
	particle_brink->SetScale({2,2,2});

	particle_fire = std::make_unique<Particle>();
	particle_fire->Initialize("player_fire", "resource/Sprite/cone.png", PrimitiveType::cone);
	particle_fire->SetParticleCount(1);
	particle_fire->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.1f);

	particle_damage = std::make_unique<Particle>();
	particle_damage->Initialize("player_damage", "resource/Sprite/circle.png", PrimitiveType::ring);
	particle_damage->SetParticleCount(20);
	particle_damage->SetParticleMosion(ParticleMosion::Exprosion);
	particle_damage->SetFrequency(1.0f);

	particle_pari = std::make_unique<Particle>();
	particle_pari->Initialize("player_pari", "resource/Sprite/uvChecker.png", PrimitiveType::cone);
	particle_pari->SetParticleMosion(ParticleMosion::Fixed);
	particle_fire->SetFrequency(0.5f);

	particle_dead = std::make_unique<Particle>();
	particle_dead->Initialize("player_dead", "resource/Sprite/ground.png", PrimitiveType::sphere);
	particle_dead->SetParticleMosion(ParticleMosion::Smaller);
	particle_dead->SetFrequency(0.1f);
	particle_dead->SetScale({ 0.5f,0.5f,0.5f });

	//影の初期化
	shadow_ = std::make_unique<Shadow>();
	shadow_->Initialize();

	//UI_体力
	for (uint32_t i = 0; i < 3; i++) {
		std::unique_ptr <Sprite> sprite = std::make_unique<Sprite>();
		sprite->Initialize("Hp.png");
		sprite->SetPosition({ initializePoint_Hp.x + textureSize_Hp.x * i , initializePoint_Hp.y - i * distanceY_Hp });
		sprite->SetSize(textureSize_Hp);
		sprites_Hp.push_back(std::move(sprite));
	}

	input_ = Input::GetInstance();
}

void Player::Update() {

	//倒された時
	if (Hp == 0) {
		//ノックバック、ダメージリアクションをリセット
		isKnockback = false;
		isDamageMosion = false;
		DeadPlayer();
	}

	if (!isPlayerDown && !performance_mode) {
		//プレイヤー操作/アクション
		PlayUpdate();
	}

	//地面にいるとき
	if (isGround) {
		isJump = false;//ジャンプ可能
		isOneBrink = false;//ブリンク可能
	}

	// - 滑空 - 
	//開いた状態で地面についていない
	//傘が上向き(斜め上も)の場合かつプレイヤーが倒されていないとき
	if (isShield && !isGround && 
		(wtGun.rotation_.x >= upDis - Naname_Value && wtGun.rotation_.x <= upDis + Naname_Value) && !isPlayerDown) {
		//ジャンプ後だとずっと浮くためfalseに
		if (isJump) {
			isJump = false;
		}
		//重力を固定することでゆっくり落ちる
		grabity = fixed_grabity;
		//ブリンクが終了した時
		if (!isBrink) {
			//滑空中は上向きのみ(斜めにはならない)
			wtGun.rotation_.x = upDis;
		}
	}

	//ジャンプ
	if (isJump) {
		worldTransform.translation_.y += 0.25f;
	}

	//演出時は関係なし
	if (!performance_mode) {
		//重力
		grabity -= standard_grabity;
		worldTransform.translation_.y += grabity;
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
		particle_walk->SetParticleCount(5);
		particle_walk->SetFrequency(0.15f);
		particle_walk->ChangeMode(BornParticle::TimerMode);
		particle_walk->SetTranslate(worldTransform.translation_ + TransformNormal(Vector3{ 0.0f,-1.0f,-0.3f }, worldTransform.matWorld_));

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
	particle_dead->Update();

	///アニメーション
	if (isShield) {
		animation_mode = Animation_Mode::mode_sield;
	}//前回の座標と現在の座標が違う = 動いた場合
	else if (worldTransform.translation_.x != PrePosition.x || worldTransform.translation_.y != PrePosition.y) {
		animation_mode = Animation_Mode::mode_move;
	}
	else {
		animation_mode = Animation_Mode::mode_stop;
	}

	//animationが変わった場合切り替える
	if (animation_mode != PreAnimation_mode) {
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
		}

		PreAnimation_mode = animation_mode;
	}

	//現在座標に前回座標を代入
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
		TransformNormal(playerFront, wtGun.matWorld_));

	umbrella->SetRotate(wtGun.rotation_);
	umbrella->ShieldMode(isShield);

	umbrella->Update();

	SpriteUpdate();

	for (auto& sprite : sprites_Hp) {
		sprite->Update();
	}
}

void Player::PlayUpdate() {

	input_->JoystickUpdate(state, preState);

	//それぞれのボタンフラグ
	pushA = false;
	pushD = false;
	pushW = false;
	pushS = false;

	//左
	if (input_->PushKey(DIK_A)) {
		pushA = true;
	}
	//右
	if (input_->PushKey(DIK_D)) {
		pushD = true;
	}
	//上
	if (input_->PushKey(DIK_W)) {
		pushW = true;
	}
	//下
	if (input_->PushKey(DIK_S)) {
		pushS = true;
	}

	//Lスティック
	if (input_->GetJoystickState(0, state)) {
		float padX = static_cast<float>(state.Gamepad.sThumbLX) / 32768.0f;
		float padY = static_cast<float>(state.Gamepad.sThumbLY) / 32768.0f;

		if (padX > 0.5f) {
			//左
			pushD = true;
		}
		else if (padX < -0.5f) {
			//右
			pushA = true;
		}

		if (padY > 0.5f) {
			//上
			pushW = true;
		}
		else if (padY < -0.5f) {
			//下
			pushS = true;
		}
	}

	//元の速さ
	speed = standard_speed;

	//シールド中足が遅くなる
	//(滑空中は影響しない)
	if (isShield && isGround) {
		speed = standard_speed / 2.0f;
	}

	if (pushA) {
		worldTransform.translation_.x -= speed;//左に移動
		worldTransform.rotation_.y = -playerDirection;//左が正面に
		UmbrellaRange(leftDis);//傘を左に
	}
	else if (pushD) {
		worldTransform.translation_.x += speed;//右に移動
		worldTransform.rotation_.y = playerDirection;//右が正面に
		UmbrellaRange(rightDis);//傘を右に
	}
	else if (pushW) {
		UmbrellaRange(upDis);//傘を上に
	}
	else if (pushS) {
		UmbrellaRange(downtDis);//傘を下に
	}

	//ジャンプ
	//指定したボタン、地面についていて傘がシールド状態でないとき
	if ((input_->TriggerKey(DIK_SPACE) || input_->TriggerBotton(state, preState, XINPUT_GAMEPAD_A))
		&& isGround && !isShield) {
		isJump = true;
		isGround = false;
	}

	//傘シールド
	if (input_->PushKey(DIK_L) || input_->PushBotton(state, XINPUT_GAMEPAD_B)) {
		//押した瞬間に移動キーを押している場合ブリンクが発動 + 一度ブリンクしていないとき
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

		isOneBrink = true;
		worldTransform.translation_ += EaseIn(TransformNormal(playerFront, wtGun.matWorld_), brinkTimer, brinkTimeMax);

		//飛んだ瞬間後ろにパーティクルをだす
		if (brinkTimer <= deltaTime) {
			Vector3 translate = wtGun.translation_ + TransformNormal(-playerFront, wtGun.matWorld_);
			particle_brink->SetTranslate(translate);
			particle_brink->SetRotate({ wtGun.rotation_.x + 90.0f,wtGun.rotation_.y,wtGun.rotation_.z });
			particle_brink->ChangeMode(BornParticle::MomentMode);
		}
	}
	//地面についている場合、下向きのブリンクは発動しない
	if (isGround && (wtGun.rotation_.x > 0.0f && wtGun.rotation_.x < leftDis)) {
		brinkTimer = brinkTimeMax;
	}
	if (brinkTimer >= brinkTimeMax) {
		isBrink = false;
		brinkTimer = 0.0f;
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

void Player::Draw() {
	GLTFCommon::GetInstance()->Command();

	object->Draw();

	if (Hp != 0) {
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
	particle_dead->Draw();

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

void Player::IsGround(bool result) {
	isGround = result;
	//地面なら重力を0にする(沈まないようにする)
	if (isGround) {
		grabity = 0;
	}
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
		Audio::GetInstance()->SoundPlayWave(hitSound, volume);
		infinityTimer = 0.0f;
		backPower = TransformNormal({ 0,0,0.5f } ,worldTransform.matWorld_);
		isKnockback = true;
		KnockBackTimeMax = infinityTimeMax / 3;
	}

	isDamageMosion = true;
	
	//連続ヒット時、元に戻す
	scaleTimer = 0.0f;
	worldTransform.scale_ = { 1,1,1 };

}

void Player::IsFall() {
	if (Hp == 0) {
		return;
	}
	Hp = 0;
	Audio::GetInstance()->SoundPlayWave(hitSound, volume);
}

void Player::KnockBackPlayer(const Vector3 Power, const float TimerMax) {
	backPower = TransformNormal(Power, worldTransform.matWorld_);
	backPower.z = 0.0f;//z方向はなし
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

	deadTimer += deltaTime;
	isPlayerDown = true;

	if (deadTimer >= hitStopTime) {
		
		//倒されたパーティクル配置+発動
		particle_dead->SetTranslate(worldTransform.translation_);
		particle_dead->ChangeMode(BornParticle::TimerMode);

		worldTransform.rotation_.y = 180.0f;//カメラのほうに向く
		worldTransform.rotation_.z += 10.0f;//回転する
		IsJumping();
		isGround = false;
		if (deadTimer >= deadTimeMax) {
			isRespown = true;
			//パーティクル発動停止
			particle_dead->ChangeMode(BornParticle::Stop);
		}
	}
	else {
		grabity = 0.0f;
		isGround = true;
	}
}

void Player::AllRespownEnd() {

	Hp = MaxHp;
	worldTransform.translation_ = respownPosition;
	
	deadTimer = 0.0f;
	worldTransform.rotation_.z = 0.0f;
	for (auto& sprite : sprites_Hp) {
		sprite->SetTextureFile("Hp.png");
	}	
	
	isRespown = false;
	isPlayerDown = false;		
}

//パリィ成功
void Player::PariSuccess() {
	//パリィ時間延長(連続弾でも跳ね返せるように)
	pariTime = pariTimeMax;

	Audio::GetInstance()->StopWave(pariSound);
	Audio::GetInstance()->SoundPlayWave(pariSound, volume);

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

void Player::UmbrellaRange(const float& direction) {
	//ブリンク中は角度を変更しない
	if (!isBrink) {
		//上下左右
		wtGun.rotation_.x = direction;

		//斜めの時
		//左上と右下
		if ((pushA && pushW) || (pushD && pushS)) {
			wtGun.rotation_.x += Naname_Value;
		}
		//左下と右上
		else if ((pushA && pushS) || (pushD && pushW)) {
			wtGun.rotation_.x -= Naname_Value;
		}
	}

	//360度を超えたらマイナスする
	if (wtGun.rotation_.x > 360.0f) {
		wtGun.rotation_.x -= 360.0f;
	}
}
